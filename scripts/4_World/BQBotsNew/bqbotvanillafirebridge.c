modded class DayZPlayerImplement
{
	protected const bool BQBOT_VERBOSE_LOGS = false;
	protected const bool BQBOT_FIRE_DIAG_LOGS = false;
	protected const bool BQBOT_MOVE_DIAG_LOGS = false;
	protected const bool BQBOT_POSE_DIAG_LOGS = false;
	protected const int BQBOT_FIRE_DIAG_INTERVAL_MS = 120;
	protected const int BQBOT_POSE_DIAG_INTERVAL_MS = 2500;
	protected const int BQBOT_BURST_MIN_SHOTS = 3;
	protected const int BQBOT_BURST_MAX_SHOTS = 5;
	protected const int BQBOT_BURST_CYCLE_COOLDOWN_MS = 220;
	protected int m_BQNextBridgeDebugTime;
	protected int m_BQNextPoseBridgeDebugTime;
	protected int m_BQNextFireDiagTime;
	protected bool m_BQPoseWasWanted;
	protected bool m_BQPoseBridgeActive;
	protected ref BQBotCommandMove m_BQMoveCmd;
	protected bool m_BQTriggerHoldActive;
	protected bool m_BQPoseRecoverQueued;
	protected int m_BQNextSafeRecoverTime;
	protected bool m_BQPitchFilterReady;
	protected float m_BQPitchFilteredDeg;
	protected bool m_BQPitchDeadbandLocked;
	protected int m_BQNextAnimRefreshTime;
	protected int m_BQBurstTargetShots;
	protected int m_BQBurstStartShotCount;
	protected int m_BQNextBurstStartTime;
	protected int m_BQRaisedAnimVar = -1;
	protected int m_BQAimYAnimVar = -1;
	protected int m_BQAimXAnimVar = -1;
	protected bool m_BQRaisedAnimInit;
	protected bool m_BQRaisedAnimState;
	protected float m_BQRaisedAnimTimer;

	override bool ModCommandHandlerBefore(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		bool handled = super.ModCommandHandlerBefore(pDt, pCurrentCommandID, pCurrentCommandFinished);

		if (!GetGame())
			return handled;

		bqBots_Boris pb = bqBots_Boris.Cast(this);
		if (!pb || !pb.BQBot_IsBot())
			return handled;

		// Force ballistic origin from weapon, not camera, for AI bots.
		// Otherwise muzzle visuals can look correct while projectile direction is wrong.
		OverrideShootFromCamera(false);

		bool wantPose = pb.BQBot_IsCombatPoseWanted();
		if (wantPose && (pb.IsClimbing() || pb.IsFalling() || pb.IsSwimming()))
			wantPose = false;
		if (GetGame().IsServer() && wantPose != m_BQPoseWasWanted)
		{
			m_BQPoseWasWanted = wantPose;
		}
		BQBot_UpdateRaisedAnimState(pb, wantPose, pDt);
		BQBot_UpdateAimAnimState(pb);

		// Movement is owned by Expansion-style eAICommandMove/ExpansionPathHandler.
		// Keep this bridge only for BQ weapon/pose support; do not tick BQBotCommandMove here.
		pb.BQBot_ExpansionMovementTick(pDt);

		BQBot_MoveDiag("tick wantPose=" + BQBoolTo01(wantPose));

		BQPoseDiag("before want=" + BQBoolTo01(wantPose));

		return handled;
	}

	override bool ModCommandHandlerAfter(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		bool handled = super.ModCommandHandlerAfter(pDt, pCurrentCommandID, pCurrentCommandFinished);

		if (!GetGame())
			return handled;

		bqBots_Boris pb = bqBots_Boris.Cast(this);
		if (!pb || !pb.BQBot_IsBot())
			return handled;

		// Keep it pinned for bots after command transitions too.
		OverrideShootFromCamera(false);

		if (!pb.BQBot_IsCombatPoseWanted())
			return handled;
		BQPoseDiag("after");

		return handled;
	}

	override bool EvaluateDamageHitAnimation(TotalDamageResult pDamageResult, int pDamageType, EntityAI pSource, string pComponent, string pAmmoType, vector pModelPos, out int pAnimType, out float pAnimHitDir, out bool pAnimHitFullbody)
	{
		bool evaluated = super.EvaluateDamageHitAnimation(pDamageResult, pDamageType, pSource, pComponent, pAmmoType, pModelPos, pAnimType, pAnimHitDir, pAnimHitFullbody);

		bqBots_Boris pb = bqBots_Boris.Cast(this);
		if (!pb || !pb.BQBot_IsBot())
			return evaluated;

		bool isCloseCombat = pDamageType == DamageType.CLOSE_COMBAT;
		bool isExplosion = pDamageType == DamageType.EXPLOSION;
		bool isExplosionLikeAmmo = BQBot_IsExplosionLikeAmmo(pAmmoType);

		if (isCloseCombat || isExplosion || isExplosionLikeAmmo)
		{
			// Keep bot hit reaction lightweight:
			// no heavy fullbody hit command from melee/explosive hits.
			pAnimType = 0;
			pAnimHitFullbody = false;
		}

		return evaluated;
	}

	protected bool BQBot_IsExplosionLikeAmmo(string ammoType)
	{
		if (ammoType == "")
			return false;

		if (!GetGame())
			return false;

		string cfg = "cfgAmmo " + ammoType + " ";

		if (GetGame().ConfigIsExisting(cfg + "indirectHit"))
		{
			if (GetGame().ConfigGetFloat(cfg + "indirectHit") > 0.0)
				return true;
		}

		if (GetGame().ConfigIsExisting(cfg + "indirectHitRange"))
		{
			if (GetGame().ConfigGetFloat(cfg + "indirectHitRange") > 0.0)
				return true;
		}

		if (GetGame().ConfigIsExisting(cfg + "explosive"))
		{
			if (GetGame().ConfigGetInt(cfg + "explosive") > 0)
				return true;
		}

		return false;
	}

	override void HandleWeapons(float pDt, Entity pInHands, HumanInputController pInputs, out bool pExitIronSights)
	{
		super.HandleWeapons(pDt, pInHands, pInputs, pExitIronSights);

		if (!GetGame() || !GetGame().IsServer())
			return;

		bqBots_Boris pb = bqBots_Boris.Cast(this);
		if (!pb || !pb.BQBot_IsBot())
			return;

		// Critical for correct projectile path on AI: shoot from weapon, not camera.
		OverrideShootFromCamera(false);

		Weapon_Base weapon = Weapon_Base.Cast(pInHands);
		if (!weapon)
		{
			if (m_BQTriggerHoldActive)
				m_BQTriggerHoldActive = false;
			m_BQPitchFilterReady = false;
			m_BQPitchDeadbandLocked = false;
			m_BQBurstTargetShots = 0;
			m_BQBurstStartShotCount = 0;
			m_BQNextBurstStartTime = 0;
			return;
		}

		WeaponManager wm = GetWeaponManager();
		if (!wm)
			return;

		bool wantHold = pb.BQBot_IsVanillaTriggerHoldWanted();
		bool hasSingleRequest = pb.BQBot_HasVanillaFireRequest();
		bool wantPose = pb.BQBot_IsCombatPoseWanted();
		int fireIntentMode = pb.BQBot_GetFireIntentMode(); // 0=auto, 1=burst, 2=single
		bool isSingleIntent = fireIntentMode == 2;
		bool canFire = wm.CanFire(weapon);
		bool weaponStable = weapon.IsIdle() && !weapon.IsWaitingForActionFinish();
		bool raisedReady = BQBot_IsRaisedReady(pb);
		bool aimReadyForFire = true;
		float aimAbsErrorDeg = 0.0;
		string fbLog = "bot=" + pb.ToString();
		fbLog = fbLog + " weapon=" + weapon.ToString();
		fbLog = fbLog + " wantHold=" + BQExpansionDiag.B(wantHold);
		fbLog = fbLog + " singleReq=" + BQExpansionDiag.B(hasSingleRequest);
		fbLog = fbLog + " wantPose=" + BQExpansionDiag.B(wantPose);
		fbLog = fbLog + " intent=" + fireIntentMode.ToString();
		fbLog = fbLog + " canFire=" + BQExpansionDiag.B(canFire);
		fbLog = fbLog + " stable=" + BQExpansionDiag.B(weaponStable);
		fbLog = fbLog + " raisedReady=" + BQExpansionDiag.B(raisedReady);
		fbLog = fbLog + " shots=" + pb.BQBot_GetServerShotCount().ToString();
		BQExpansionDiag.Log("FIREBRIDGE", fbLog);

		// If there is no fire intent, still allow pitch tracking while combat pose is active.
		// This removes visible delay when target quickly changes elevation before first shot.
		if (!wantHold && !hasSingleRequest)
		{
			if (m_BQTriggerHoldActive)
			{
				weapon.ResetBurstCount();
				m_BQTriggerHoldActive = false;
			}
			m_BQBurstTargetShots = 0;
			m_BQBurstStartShotCount = 0;
			m_BQNextBurstStartTime = 0;

			if (!wantPose)
			{
				BQExpansionDiag.Log("FIREBRIDGE", "RETURN reason=no_intent_no_pose bot=" + pb.ToString());
				m_BQPitchFilterReady = false;
				m_BQPitchDeadbandLocked = false;
				return;
			}
		}

		HumanCommandWeapons hcwAdjust = pb.GetCommandModifier_Weapons();
		float desiredPitchDeg = 0.0;
		if (hcwAdjust && pInputs && pb.BQBot_GetAimPitch(desiredPitchDeg))
		{
			desiredPitchDeg = BQNormalizePitchDeg(desiredPitchDeg);
			float currentUDDeg = hcwAdjust.GetBaseAimingAngleUD();
			if (!m_BQPitchFilterReady)
			{
				m_BQPitchFilteredDeg = currentUDDeg;
				m_BQPitchFilterReady = true;
				m_BQPitchDeadbandLocked = false;
			}

			// Smooth desired pitch while keeping response fast enough for elevation changes.
			float filterAlpha = 0.50;
			if (wantHold)
				filterAlpha = 0.40;
			else if (isSingleIntent)
				filterAlpha = 0.34;
			m_BQPitchFilteredDeg = m_BQPitchFilteredDeg + (desiredPitchDeg - m_BQPitchFilteredDeg) * filterAlpha;
			float filteredDesiredPitchDeg = m_BQPitchFilteredDeg;

			float diffDeg = filteredDesiredPitchDeg - currentUDDeg;
			while (diffDeg > 180.0)
				diffDeg = diffDeg - 360.0;
			while (diffDeg < -180.0)
				diffDeg = diffDeg + 360.0;

			float stepDeg = 0.0;
			float absDiffDeg = Math.AbsFloat(diffDeg);
			aimAbsErrorDeg = absDiffDeg;
			float aimFireThresholdDeg = 4.6;
			if (wantHold)
				aimFireThresholdDeg = 3.4;
			else if (isSingleIntent)
				aimFireThresholdDeg = 5.8;
			aimReadyForFire = absDiffDeg <= aimFireThresholdDeg;
			float lockEnterDeg = 0.6;
			float lockExitDeg = 1.6;
			if (isSingleIntent)
			{
				lockEnterDeg = 0.9;
				lockExitDeg = 2.2;
			}
			if (wantHold)
			{
				// During sustained fire we prefer visual stability over tiny aim corrections.
				lockEnterDeg = 1.0;
				lockExitDeg = 2.4;
			}

			if (!m_BQPitchDeadbandLocked && absDiffDeg <= lockEnterDeg)
				m_BQPitchDeadbandLocked = true;
			else if (m_BQPitchDeadbandLocked && absDiffDeg >= lockExitDeg)
				m_BQPitchDeadbandLocked = false;

			if (!m_BQPitchDeadbandLocked)
			{
				float stepLimitDeg = 2.20;
				if (absDiffDeg > 10.0)
					stepLimitDeg = 3.40;
				if (absDiffDeg > 20.0)
					stepLimitDeg = 4.80;
				if (isSingleIntent && !wantHold)
				{
					stepLimitDeg = 1.65;
					if (absDiffDeg > 10.0)
						stepLimitDeg = 2.55;
					if (absDiffDeg > 20.0)
						stepLimitDeg = 3.45;
				}
				if (wantHold)
					stepLimitDeg = stepLimitDeg * 0.90;

				stepDeg = Math.Clamp(diffDeg, -stepLimitDeg, stepLimitDeg);
				float stepRad = stepDeg * 0.0174532925;
				pInputs.OverrideAimChangeY(HumanInputControllerOverrideType.ONE_FRAME, stepRad);
			}

			if (BQBOT_FIRE_DIAG_LOGS && absDiffDeg > 0.05)
			{
				string deadbandLocked = "0";
				if (m_BQPitchDeadbandLocked)
					deadbandLocked = "1";
				string aimDiag = "aim_ud current=" + currentUDDeg.ToString();
				aimDiag = aimDiag + " desired=" + desiredPitchDeg.ToString();
				aimDiag = aimDiag + " desiredFiltered=" + filteredDesiredPitchDeg.ToString();
				aimDiag = aimDiag + " diff=" + diffDeg.ToString();
				aimDiag = aimDiag + " step=" + stepDeg.ToString();
				aimDiag = aimDiag + " lock=" + deadbandLocked;
				BQFireDiag(pb, weapon, aimDiag);
			}
		}
		else
		{
			m_BQPitchFilterReady = false;
			m_BQPitchDeadbandLocked = false;
		}

		if (BQBOT_FIRE_DIAG_LOGS)
		{
			int diagMuzzle = weapon.GetCurrentMuzzle();
			string intentName = "auto";
			if (fireIntentMode == 1)
				intentName = "burst";
			else if (fireIntentMode == 2)
				intentName = "single";

			string fireDiagMsg = "handle_weapons intent=" + intentName;
			fireDiagMsg = fireDiagMsg + " wantHold=" + BQBoolTo01(wantHold);
			fireDiagMsg = fireDiagMsg + " singleReq=" + BQBoolTo01(hasSingleRequest);
			fireDiagMsg = fireDiagMsg + " canFire=" + BQBoolTo01(canFire);
			fireDiagMsg = fireDiagMsg + " weaponStable=" + BQBoolTo01(weaponStable);
			fireDiagMsg = fireDiagMsg + " raisedReady=" + BQBoolTo01(raisedReady);
			fireDiagMsg = fireDiagMsg + " aimReady=" + BQBoolTo01(aimReadyForFire);
			fireDiagMsg = fireDiagMsg + " aimErrDeg=" + aimAbsErrorDeg.ToString();
			fireDiagMsg = fireDiagMsg + " mode=" + weapon.GetCurrentMode(diagMuzzle).ToString();
			fireDiagMsg = fireDiagMsg + " modeName=" + weapon.GetCurrentModeName(diagMuzzle);
			fireDiagMsg = fireDiagMsg + " modeAuto=" + BQBoolTo01(weapon.GetCurrentModeAutoFire(diagMuzzle));
			fireDiagMsg = fireDiagMsg + " burstSize=" + weapon.GetCurrentModeBurstSize(diagMuzzle).ToString();
			fireDiagMsg = fireDiagMsg + " burstTarget=" + m_BQBurstTargetShots.ToString();
			fireDiagMsg = fireDiagMsg + " burstStartShots=" + m_BQBurstStartShotCount.ToString();
			fireDiagMsg = fireDiagMsg + " serverShots=" + pb.BQBot_GetServerShotCount().ToString();
			BQFireDiag(pb, weapon, fireDiagMsg);
		}

		if (wantHold && !m_BQTriggerHoldActive)
		{
			weapon.ResetBurstCount();
			m_BQTriggerHoldActive = true;
		}
		else if (!wantHold && m_BQTriggerHoldActive)
		{
			weapon.ResetBurstCount();
			m_BQTriggerHoldActive = false;
		}

		if (fireIntentMode == 1)
		{
			// True burst: hold trigger in AUTO mode and stop after 3-5 real OnFire shots.
			if (!wantHold)
			{
				m_BQBurstTargetShots = 0;
				m_BQBurstStartShotCount = 0;
				m_BQNextBurstStartTime = 0;
				if (hasSingleRequest)
					pb.BQBot_ConsumeVanillaFireRequest();
				return;
			}

			if (!canFire || !weaponStable || !raisedReady || !aimReadyForFire)
			{
				string fbBlock = "BLOCK burst bot=" + pb.ToString();
				fbBlock = fbBlock + " canFire=" + BQExpansionDiag.B(canFire);
				fbBlock = fbBlock + " stable=" + BQExpansionDiag.B(weaponStable);
				fbBlock = fbBlock + " raisedReady=" + BQExpansionDiag.B(raisedReady);
				fbBlock = fbBlock + " aimReady=" + BQExpansionDiag.B(aimReadyForFire);
				fbBlock = fbBlock + " aimErr=" + aimAbsErrorDeg.ToString();
				BQExpansionDiag.Log("FIREBRIDGE", fbBlock);
				if (BQBOT_FIRE_DIAG_LOGS)
				{
					string burstBlockMsg = "wm_fire_block burst canFire=" + BQBoolTo01(canFire);
					burstBlockMsg = burstBlockMsg + " weaponStable=" + BQBoolTo01(weaponStable);
					burstBlockMsg = burstBlockMsg + " raisedReady=" + BQBoolTo01(raisedReady);
					burstBlockMsg = burstBlockMsg + " aimReady=" + BQBoolTo01(aimReadyForFire);
					burstBlockMsg = burstBlockMsg + " aimErrDeg=" + aimAbsErrorDeg.ToString();
					BQFireDiag(pb, weapon, burstBlockMsg);
				}
				return;
			}

			int now = GetGame().GetTime();
			int shotsNow = pb.BQBot_GetServerShotCount();

			if (m_BQBurstTargetShots <= 0)
			{
				if (now < m_BQNextBurstStartTime)
				{
					if (BQBOT_FIRE_DIAG_LOGS)
						BQFireDiag(pb, weapon, "wm_fire_wait burst cooldownLeftMs=" + (m_BQNextBurstStartTime - now).ToString());
					return;
				}

				m_BQBurstTargetShots = Math.RandomIntInclusive(BQBOT_BURST_MIN_SHOTS, BQBOT_BURST_MAX_SHOTS);
				m_BQBurstStartShotCount = shotsNow;
				if (BQBOT_FIRE_DIAG_LOGS)
					BQFireDiag(pb, weapon, "burst_start targetShots=" + m_BQBurstTargetShots.ToString() + " startShots=" + m_BQBurstStartShotCount.ToString(), true);
			}

			int shotsInCycle = shotsNow - m_BQBurstStartShotCount;
			if (shotsInCycle >= m_BQBurstTargetShots)
			{
				int stoppedTargetShots = m_BQBurstTargetShots;
				m_BQBurstTargetShots = 0;
				m_BQBurstStartShotCount = shotsNow;
				m_BQNextBurstStartTime = now + BQBOT_BURST_CYCLE_COOLDOWN_MS;
				if (BQBOT_FIRE_DIAG_LOGS)
					BQFireDiag(pb, weapon, "burst_stop shotsInCycle=" + shotsInCycle.ToString() + " targetShots=" + stoppedTargetShots.ToString() + " serverShots=" + shotsNow.ToString(), true);
				return;
			}

			if (BQBOT_FIRE_DIAG_LOGS)
				BQFireDiag(pb, weapon, "wm_fire burst shotsInCycle=" + shotsInCycle.ToString() + " targetShots=" + m_BQBurstTargetShots.ToString() + " serverShots=" + shotsNow.ToString(), true);
			BQExpansionDiag.Log("FIREBRIDGE", "FIRE burst bot=" + pb.ToString() + " weapon=" + weapon.ToString());
			wm.Fire(weapon);
			BQTryRefreshAnimationState(wm);
			pb.BQBot_MarkTrigger();

			return;
		}
		else if (m_BQBurstTargetShots > 0 || m_BQBurstStartShotCount > 0 || m_BQNextBurstStartTime > 0)
		{
			m_BQBurstTargetShots = 0;
			m_BQBurstStartShotCount = 0;
			m_BQNextBurstStartTime = 0;
		}

		if (fireIntentMode == 0 && wantHold && canFire && raisedReady && aimReadyForFire)
		{
			BQExpansionDiag.Log("FIREBRIDGE", "FIRE auto bot=" + pb.ToString() + " weapon=" + weapon.ToString());
			if (BQBOT_VERBOSE_LOGS)
			{
				HumanCommandWeapons hcw = pb.GetCommandModifier_Weapons();
				HumanInputController hic = pb.GetInputController();
				float yaw = 0;
				float pitch = 0;
				if (hcw)
				{
					yaw = hcw.GetBaseAimingAngleLR();
					pitch = hcw.GetBaseAimingAngleUD();
				}
				vector dir = "0 0 0";
				if (hic)
				{
					dir = hic.GetAimChange();
				}
				// BQBot FIRE diag disabled
			}
			if (BQBOT_FIRE_DIAG_LOGS)
				BQFireDiag(pb, weapon, "wm_fire auto hold=1 serverShots=" + pb.BQBot_GetServerShotCount().ToString(), true);
			wm.Fire(weapon);
			BQTryRefreshAnimationState(wm);
			pb.BQBot_MarkTrigger();
			return;
		}

		if (!hasSingleRequest)
			return;

		string fbSingle = "SINGLE_REQ bot=" + pb.ToString();
		fbSingle = fbSingle + " canFire=" + BQExpansionDiag.B(canFire);
		fbSingle = fbSingle + " stable=" + BQExpansionDiag.B(weaponStable);
		fbSingle = fbSingle + " raisedReady=" + BQExpansionDiag.B(raisedReady);
		fbSingle = fbSingle + " aimReady=" + BQExpansionDiag.B(aimReadyForFire);
		fbSingle = fbSingle + " aimErr=" + aimAbsErrorDeg.ToString();
		BQExpansionDiag.Log("FIREBRIDGE", fbSingle);
		if (!canFire || !weaponStable || !raisedReady || !aimReadyForFire)
		{
			// Single-shot path: do not keep stale fire requests when weapon cannot fire due ammo/reload windows.
			// Otherwise request can linger and occasionally delay vanilla reload recovery.
			if (hasSingleRequest && isSingleIntent && !canFire)
				pb.BQBot_ConsumeVanillaFireRequest();

			if (BQBOT_FIRE_DIAG_LOGS)
				BQFireDiag(pb, weapon, "wm_fire_block single_request=1 aimReady=" + BQBoolTo01(aimReadyForFire) + " aimErrDeg=" + aimAbsErrorDeg.ToString());
			return;
		}

		pb.BQBot_ConsumeVanillaFireRequest();

		if (BQBOT_FIRE_DIAG_LOGS)
			BQFireDiag(pb, weapon, "wm_fire single_request=1 serverShots=" + pb.BQBot_GetServerShotCount().ToString(), true);
		BQExpansionDiag.Log("FIREBRIDGE", "FIRE single bot=" + pb.ToString() + " weapon=" + weapon.ToString());
		wm.Fire(weapon);
		BQTryRefreshAnimationState(wm);
		pb.BQBot_MarkTrigger();
	}

	protected void BQTryRefreshAnimationState(WeaponManager wm)
	{
		if (!wm || !GetGame() || !GetGame().IsServer())
			return;

		int now = GetGame().GetTime();
		if (now < m_BQNextAnimRefreshTime)
			return;

		wm.RefreshAnimationState();
		m_BQNextAnimRefreshTime = now + 180;
	}

	protected void BQBot_ForceExitCombatPoseCommand()
	{
		// BQBot Phase 1: ONE_FRAME raise auto-expires, no explicit DISABLED needed.
		if (m_BQMoveCmd)
		{
			m_BQMoveCmd.SetRaised(false);
			m_BQMoveCmd.Invalidate();
		}

		m_BQMoveCmd = null;
		m_BQPoseBridgeActive = false;
		m_BQPoseRecoverQueued = false;
		BQBot_MoveDiag("forced bridge exit");
	}

	protected void BQBot_MoveDiag(string msg, bool force = false)
	{
		if (!BQBOT_MOVE_DIAG_LOGS || !GetGame() || !GetGame().IsServer())
			return;

		int now = GetGame().GetTime();
		if (!force && now < m_BQNextBridgeDebugTime)
			return;

		m_BQNextBridgeDebugTime = now + 2500;
		Print("[EAI] [move] " + msg + " pos=" + GetPosition().ToString() + " ori=" + GetOrientation().ToString());
	}

	protected void BQBot_EnsureRaisedAnimVar()
	{
		if (m_BQRaisedAnimVar >= 0)
			return;

		HumanAnimInterface anim = GetAnimInterface();
		if (!anim)
			return;

		m_BQRaisedAnimVar = anim.BindVariableBool("eAI_Raised");
		if (m_BQRaisedAnimVar < 0)
			m_BQRaisedAnimVar = anim.BindVariableBool("Raised");
	}

	protected void BQBot_EnsureAimAnimVars()
	{
		if (m_BQAimYAnimVar >= 0 && m_BQAimXAnimVar >= 0)
			return;

		HumanAnimInterface anim = GetAnimInterface();
		if (!anim)
			return;

		if (m_BQAimYAnimVar < 0)
		{
			m_BQAimYAnimVar = anim.BindVariableFloat("eAI_AimY");
			if (m_BQAimYAnimVar < 0)
				m_BQAimYAnimVar = anim.BindVariableFloat("AimY");
		}

		if (m_BQAimXAnimVar < 0)
		{
			m_BQAimXAnimVar = anim.BindVariableFloat("eAI_AimX");
			if (m_BQAimXAnimVar < 0)
				m_BQAimXAnimVar = anim.BindVariableFloat("AimX");
		}
	}

	protected void BQBot_UpdateAimAnimState(bqBots_Boris pb)
	{
		if (!pb)
			return;

		BQBot_EnsureAimAnimVars();
		if (m_BQAimYAnimVar < 0 && m_BQAimXAnimVar < 0)
			return;

		HumanCommandWeapons hcw = GetCommandModifier_Weapons();
		if (!hcw)
			return;

		if (m_BQAimYAnimVar >= 0)
			AnimSetFloat(m_BQAimYAnimVar, hcw.GetBaseAimingAngleUD());

		if (m_BQAimXAnimVar >= 0)
			AnimSetFloat(m_BQAimXAnimVar, hcw.GetBaseAimingAngleLR());
	}

	protected void BQBot_UpdateRaisedAnimState(bqBots_Boris pb, bool wantPose, float pDt)
	{
		if (!pb)
			return;

		BQBot_EnsureRaisedAnimVar();
		if (!m_BQRaisedAnimInit || m_BQRaisedAnimState != wantPose)
		{
			m_BQRaisedAnimInit = true;
			m_BQRaisedAnimState = wantPose;
			m_BQRaisedAnimTimer = 0.0;
			if (m_BQRaisedAnimVar >= 0)
				AnimSetBool(m_BQRaisedAnimVar, wantPose);

			if (!wantPose)
			{
				HumanCommandWeapons hcw = GetCommandModifier_Weapons();
				if (hcw)
					hcw.SetADS(false);
			}
		}

		if (wantPose)
			m_BQRaisedAnimTimer = m_BQRaisedAnimTimer + pDt;
		else
			m_BQRaisedAnimTimer = 0.0;
	}

	protected bool BQBot_IsRaisedReady(bqBots_Boris pb)
	{
		if (!pb)
			return false;

		bool inRaisedErect = pb.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDERECT);
		// Some custom AGR setups may not reliably expose RAISEDERECT mask every frame.
		// For fire gating we treat native raised-complete as authoritative.
		if (IsRaised() && IsWeaponRaiseCompleted())
			return true;

		if (!pb.BQBot_IsCombatPoseWanted())
			return false;

		// AI path can keep native IsRaised()/IsWeaponRaiseCompleted() false even when
		// raised animation is already active. Allow fire gate after short anim settle.
		if (m_BQRaisedAnimState && m_BQRaisedAnimTimer >= 0.12)
			return true;

		if (pb.IsLiftWeapon())
			return true;

		return inRaisedErect;
	}

	protected float BQNormalizePitchDeg(float pitchDeg)
	{
		pitchDeg = BQNormalizeAngleDeg(pitchDeg);

		return Math.Clamp(pitchDeg, -85.0, 85.0);
	}

	protected float BQNormalizeAngleDeg(float angleDeg)
	{
		while (angleDeg > 180.0)
			angleDeg = angleDeg - 360.0;

		while (angleDeg < -180.0)
			angleDeg = angleDeg + 360.0;

		return angleDeg;
	}

	protected void BQBot_EnableCombatPoseBridge(bool wantPose)
	{
		// Deprecated: movement/stance command ownership is now Expansion-style eAICommandMove.
		m_BQPoseBridgeActive = false;
		m_BQPoseRecoverQueued = false;
	}

	BQBotCommandMove BQBot_GetMoveCommandBridge()
	{
		return m_BQMoveCmd;
	}

	bqCombatPathHandler BQBot_GetPathFinding()
	{
		if (!m_BQMoveCmd)
			return null;

		return m_BQMoveCmd.BQBot_GetPathFinding();
	}

	float BQBot_GetCurrentMovementSpeed()
	{
		eAIBase ai = eAIBase.Cast(this);
		if (ai)
			return ai.Expansion_GetMovementSpeed();

		return 0.0;
	}
	protected bool BQBot_IsMoveBridgeCommandCurrent()
	{
		if (!m_BQPoseBridgeActive || !m_BQMoveCmd)
			return false;

		Human human = Human.Cast(this);
		if (!human)
			return false;

		HumanCommandScript cur = human.GetCommand_Script();
		return cur == m_BQMoveCmd;
	}

	protected void BQBot_DisableCombatPoseBridge()
	{
		Weapon_Base handsWpn = Weapon_Base.Cast(GetEntityInHands());
		if (handsWpn)
			handsWpn.ResetBurstCount();

		m_BQTriggerHoldActive = false;

		// BQBot Phase 1: ONE_FRAME raise auto-expires, no explicit DISABLED needed.
		if (m_BQMoveCmd)
		{
			m_BQMoveCmd.SetRaised(false);
			m_BQMoveCmd.Invalidate();
			m_BQMoveCmd = null;
		}

		m_BQPoseBridgeActive = false;
		m_BQPoseRecoverQueued = false;
		BQPoseDiag("disable", true);
	}

	protected string BQBoolTo01(bool v)
	{
		if (v)
			return "1";

		return "0";
	}

	protected void BQPoseDiag(string tag, bool force = false)
	{
		if (!BQBOT_POSE_DIAG_LOGS || !GetGame() || !GetGame().IsServer())
			return;

		int now = GetGame().GetTime();
		if (!force && now < m_BQNextPoseBridgeDebugTime)
			return;
		m_BQNextPoseBridgeDebugTime = now + BQBOT_POSE_DIAG_INTERVAL_MS;

		bqBots_Boris pb = bqBots_Boris.Cast(this);
		if (!pb || !pb.BQBot_IsBot())
			return;

		bool hasMoveCmd = pb.GetCommand_Move() != null;
		bool hasScriptCmd = false;
		Human human = Human.Cast(this);
		if (human)
		{
			HumanCommandScript scriptCmd = human.GetCommand_Script();
			hasScriptCmd = scriptCmd != null;
		}

		string msg = tag;
		msg = msg + " wantPose=" + BQBoolTo01(pb.BQBot_IsCombatPoseWanted());
		msg = msg + " bridgeActive=" + BQBoolTo01(m_BQPoseBridgeActive);
		msg = msg + " raised=" + BQBoolTo01(IsRaised());
		msg = msg + " raiseDone=" + BQBoolTo01(IsWeaponRaiseCompleted());
		msg = msg + " animRaised=" + BQBoolTo01(m_BQRaisedAnimState);
		msg = msg + " animT=" + m_BQRaisedAnimTimer.ToString();
		msg = msg + " trigHold=" + BQBoolTo01(pb.BQBot_IsVanillaTriggerHoldWanted());
		msg = msg + " moveCmd=" + BQBoolTo01(hasMoveCmd);
		msg = msg + " scriptCmd=" + BQBoolTo01(hasScriptCmd);
		msg = msg + " pos=" + GetPosition().ToString();
		msg = msg + " ori=" + GetOrientation().ToString();
		Print("[EAI] [posebridge] " + msg);
	}

	protected void BQBot_QueueRecoverIfCommandLost(bqBots_Boris pb, bool wantPose)
	{
		return;
	}

	protected void BQBot_SafeRecoverPoseCommand()
	{
		m_BQPoseRecoverQueued = false;
	}

	protected void BQFireDiag(bqBots_Boris pb, Weapon_Base wpn, string tag, bool force = false)
	{
		return;
	}
	
}
