modded class PlayerBase
{
	protected const bool BQBOT_VERBOSE_LOGS = false;
	protected const bool BQBOT_FF_DAMAGE_DIAG = false;
	protected const bool BQBOT_DMG_MULT_DIAG = false;
	protected bool m_BQIsBot;
	protected int m_BQShotTick;
	protected int m_BQServerShotCount;
	protected int m_BQLastShotTickClient;
	protected int m_BQPendingVanillaFireRequests;
	protected bool m_BQVanillaTriggerHoldWanted;
	protected int m_BQFireIntentMode;
	protected bool m_BQCombatPoseWanted;
	protected bool m_BQMoveIntentValid;
	protected float m_BQMoveIntentSpeed;
	protected float m_BQMoveIntentAngle;
	protected bool m_BQMoveNavGoalValid;
	protected vector m_BQMoveNavGoal;
	protected bool m_BQMoveUseNavPath;
	protected bool m_BQMoveNavGoalIsFinal;
	protected bool m_BQMoveAllowJumpClimb;
	protected float m_BQMoveNavGoalMaxDistance;
	protected int m_BQMoveNavGoalVersion;
	// Expansion-style position override lock: while held, BQBot_SetMoveNavGoal
	// is ignored so cover/flank movement cannot be preempted by per-tick
	// positioning logic.
	protected bool m_BQMoveGoalLocked;
	protected int m_BQMoveGoalLockUntilTime;
	protected bool m_BQMoveContextValid;
	protected int m_BQMoveContextState;
	protected bool m_BQMoveContextHasLos;
	protected bool m_BQMoveContextForceMedium;
	protected float m_BQMoveContextDistToTarget;
	protected float m_BQMoveContextStopDist;
	protected float m_BQMoveContextJogDist;
	protected float m_BQMoveContextSprintDist;
	protected bool m_BQMoveContextCombatPose;
	protected bool m_BQMoveTargetUnreachable;
	protected vector m_BQMoveTargetUnreachablePos;
	protected int m_BQMoveTargetUnreachableTime;
	protected string m_BQMoveTargetUnreachableReason;
	protected bool m_BQSurvivalInitDone;
	protected bool m_BQSurvivalPipelineBypassLogged;
	protected bool m_BQPostReloadSyncNeeded;
	protected bool m_BQAimHeadingValid;
	protected float m_BQAimHeading;
	protected bool m_BQAimPitchValid;
	protected float m_BQAimPitch;
	protected bool m_BQAimTargetValid;
	protected vector m_BQAimTargetPos;
	protected bool m_BQAimTurnFeetWanted;
	protected float m_BQAccuracyBase;
	protected float m_BQAccuracyNearDistance;
	protected float m_BQAccuracyFarDistance;
	protected float m_BQAccuracyNearFactor;
	protected float m_BQAccuracyFarFactor;
	protected float m_BQAccuracyMaxSpreadDeg;
	protected float m_BQDamageMultiplier;
	protected bool m_BQLastShotTelemetryValid;
	protected int m_BQLastShotServerTime;
	protected vector m_BQLastShotFirePos;
	protected vector m_BQLastShotFireDir;
	protected PlayerBase m_BQCombatTarget;
	protected int m_BQLastPlayerShotNoiseTime;
	protected vector m_BQLastPlayerShotNoisePos;
	protected float m_BQThreatToSelfActive;
	protected float m_BQThreatToSelfAbsolute;

	void PlayerBase()
	{
		RegisterNetSyncVariableBool("m_BQIsBot");
		RegisterNetSyncVariableBool("m_BQCombatPoseWanted");
		RegisterNetSyncVariableInt("m_BQShotTick", 0, 2000000000);
		m_BQAccuracyBase = 0.8;
		m_BQAccuracyNearDistance = 20.0;
		m_BQAccuracyFarDistance = 300.0;
		m_BQAccuracyNearFactor = 1.0;
		m_BQAccuracyFarFactor = 0.2;
		m_BQAccuracyMaxSpreadDeg = 8.0;
		m_BQDamageMultiplier = 1.0;
		m_BQLastShotTelemetryValid = false;
		m_BQLastShotServerTime = 0;
		m_BQLastShotFirePos = "0 0 0";
		m_BQLastShotFireDir = "0 0 0";
		m_BQCombatTarget = null;
		m_BQLastPlayerShotNoiseTime = 0;
		m_BQLastPlayerShotNoisePos = "0 0 0";
		m_BQThreatToSelfActive = 0.0;
		m_BQThreatToSelfAbsolute = 0.0;
		m_BQMoveNavGoalIsFinal = true;
		m_BQMoveAllowJumpClimb = true;
		m_BQMoveNavGoalMaxDistance = 1.0;
		m_BQMoveTargetUnreachable = false;
		m_BQMoveTargetUnreachablePos = "0 0 0";
		m_BQMoveTargetUnreachableReason = "";
		m_BQMoveTargetUnreachableTime = 0;
	}

	void BQBot_SetIsBot(bool isBot)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (isBot && !BQBot_IsSupportedClass())
			isBot = false;

		m_BQIsBot = isBot;
		SetSynchDirty();
	}

	bool BQBot_IsBot()
	{
		return m_BQIsBot && BQBot_IsSupportedClass();
	}

	protected bool BQBot_IsSupportedClass()
	{
		string t = GetType();
		if (GetGame())
			return GetGame().IsKindOf(t, "bqBots_Boris");

		// Fallback for edge initialization moments.
		return t == "bqBots_Boris" || t == "bqBots_Boris_AI";
	}

	bool BQBot_IsAIBase()
	{
		return BQBot_IsSupportedClass();
	}

	void BQBot_SetCombatPoseWanted(bool wanted)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		if (m_BQCombatPoseWanted == wanted)
			return;

		m_BQCombatPoseWanted = wanted;
		SetSynchDirty();
	}

	bool BQBot_IsCombatPoseWanted()
	{
		return m_BQCombatPoseWanted;
	}

	void BQBot_SetCombatTargetContext(PlayerBase target, float activeThreat, float absoluteThreat)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQCombatTarget = target;
		m_BQThreatToSelfActive = Math.Clamp(activeThreat, 0.0, 1.0);
		m_BQThreatToSelfAbsolute = Math.Clamp(absoluteThreat, 0.0, 1.0);
	}

	PlayerBase BQBot_GetCombatTarget()
	{
		return m_BQCombatTarget;
	}

	float BQBot_GetThreatToSelf(bool ignoreLOS = false)
	{
		if (ignoreLOS)
			return m_BQThreatToSelfAbsolute;

		return m_BQThreatToSelfActive;
	}

	void BQBot_SetMoveIntent(float speed, float angleDeg)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQMoveIntentSpeed = Math.Clamp(speed, 0.0, 3.0);
		m_BQMoveIntentAngle = angleDeg;
		while (m_BQMoveIntentAngle > 180.0)
			m_BQMoveIntentAngle = m_BQMoveIntentAngle - 360.0;
		while (m_BQMoveIntentAngle < -180.0)
			m_BQMoveIntentAngle = m_BQMoveIntentAngle + 360.0;
		m_BQMoveIntentValid = true;
	}

	void BQBot_ClearMoveIntent()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		m_BQMoveIntentValid = false;
		m_BQMoveIntentSpeed = 0.0;
		m_BQMoveIntentAngle = 0.0;
	}

	void BQBot_SetMoveNavGoal(vector goalPos, bool useNavPath = true, bool isFinal = true, bool allowJumpClimb = true, float maxDistance = 1.0)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		// Honour active override lock (cover/flank) unless it expired.
		if (m_BQMoveGoalLocked)
		{
			if (GetGame().GetTime() < m_BQMoveGoalLockUntilTime)
				return;
			m_BQMoveGoalLocked = false;
		}

		bool hadGoal = m_BQMoveNavGoalValid;
		bool useChanged = (m_BQMoveUseNavPath != useNavPath);
		bool jumpClimbChanged = (m_BQMoveAllowJumpClimb != allowJumpClimb);
		bool maxDistanceChanged = Math.AbsFloat(m_BQMoveNavGoalMaxDistance - maxDistance) > 0.01;
		float goalDeltaSq = vector.DistanceSq(m_BQMoveNavGoal, goalPos);
		// Expansion-style: only bump version on meaningful goal change (~1.5m).
		// Path handler does its own overshoot/timer recalculation between bumps,
		// so chasing a moving player no longer forces a hard repath every tick.
		bool goalChanged = goalDeltaSq >= 2.25;

		m_BQMoveNavGoal = goalPos;
		m_BQMoveNavGoalValid = true;
		m_BQMoveUseNavPath = useNavPath;
		m_BQMoveNavGoalIsFinal = isFinal;
		m_BQMoveAllowJumpClimb = allowJumpClimb;
		m_BQMoveNavGoalMaxDistance = maxDistance;

		if (!hadGoal || useChanged || jumpClimbChanged || maxDistanceChanged || goalChanged)
		{
			m_BQMoveNavGoalVersion++;
			BQBot_ClearMoveTargetUnreachable();
		}
	}

	void BQBot_SetMoveUseNavPath(bool useNavPath)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQMoveUseNavPath = useNavPath;
	}

	void BQBot_ClearMoveNavGoal()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		// Force-clear bypasses the override lock (caller must want a hard reset).
		m_BQMoveGoalLocked = false;
		m_BQMoveGoalLockUntilTime = 0;

		if (!m_BQMoveNavGoalValid && !m_BQMoveUseNavPath)
			return;

		m_BQMoveNavGoalValid = false;
		m_BQMoveUseNavPath = false;
		m_BQMoveNavGoalIsFinal = true;
		m_BQMoveAllowJumpClimb = true;
		m_BQMoveNavGoalMaxDistance = 1.0;
		m_BQMoveNavGoal = "0 0 0";
		m_BQMoveNavGoalVersion++;
		BQBot_ClearMoveTargetUnreachable();
	}

	// Lock current goal so that per-tick positioning cannot rewrite it.
	// `durationMs` is a hard cap; lock auto-expires after that to prevent deadlock.
	void BQBot_LockMoveNavGoal(vector goalPos, int durationMs, bool isFinal = true, bool allowJumpClimb = true, float maxDistance = 1.0)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;
		if (!m_BQIsBot)
			return;
		if (durationMs < 250)
			durationMs = 250;
		if (durationMs > 15000)
			durationMs = 15000;

		// Force-set the goal first, then engage the lock.
		bool hadGoal = m_BQMoveNavGoalValid;
		bool useChanged = !m_BQMoveUseNavPath;
		bool jumpClimbChanged = (m_BQMoveAllowJumpClimb != allowJumpClimb);
		bool maxDistanceChanged = Math.AbsFloat(m_BQMoveNavGoalMaxDistance - maxDistance) > 0.01;
		float goalDeltaSq = vector.DistanceSq(m_BQMoveNavGoal, goalPos);
		bool goalChanged = goalDeltaSq >= 2.25;
		m_BQMoveNavGoal = goalPos;
		m_BQMoveNavGoalValid = true;
		m_BQMoveUseNavPath = true;
		m_BQMoveNavGoalIsFinal = isFinal;
		m_BQMoveAllowJumpClimb = allowJumpClimb;
		m_BQMoveNavGoalMaxDistance = maxDistance;
		if (!hadGoal || useChanged || jumpClimbChanged || maxDistanceChanged || goalChanged)
		{
			m_BQMoveNavGoalVersion++;
			BQBot_ClearMoveTargetUnreachable();
		}

		m_BQMoveGoalLocked = true;
		m_BQMoveGoalLockUntilTime = GetGame().GetTime() + durationMs;
	}

	void BQBot_ReleaseMoveNavGoalLock()
	{
		m_BQMoveGoalLocked = false;
		m_BQMoveGoalLockUntilTime = 0;
	}

	bool BQBot_IsMoveNavGoalLocked()
	{
		if (!m_BQMoveGoalLocked)
			return false;
		if (GetGame() && GetGame().GetTime() >= m_BQMoveGoalLockUntilTime)
		{
			m_BQMoveGoalLocked = false;
			return false;
		}
		return true;
	}

	bool BQBot_HasMoveNavGoal()
	{
		return m_BQMoveNavGoalValid && m_BQMoveUseNavPath;
	}

	bool BQBot_GetMoveNavGoal(out vector goalPos, out bool useNavPath, out int version)
	{
		goalPos = "0 0 0";
		useNavPath = false;
		version = m_BQMoveNavGoalVersion;

		if (!m_BQMoveNavGoalValid)
			return false;

		goalPos = m_BQMoveNavGoal;
		useNavPath = m_BQMoveUseNavPath;
		return true;
	}

	bool BQBot_IsMoveTargetPositionFinal()
	{
		return m_BQMoveNavGoalIsFinal;
	}

	bool BQBot_GetMoveAllowJumpClimb()
	{
		return m_BQMoveAllowJumpClimb;
	}

	float BQBot_GetMoveNavGoalMaxDistance()
	{
		if (m_BQMoveNavGoalMaxDistance <= 0.0)
			return 1.0;

		return m_BQMoveNavGoalMaxDistance;
	}

	void BQBot_SetMoveTargetPositionFinal(bool isFinal)
	{
		m_BQMoveNavGoalIsFinal = isFinal;
	}

	void BQBot_MarkMoveTargetUnreachable(vector targetPos, string reason)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;
		if (!m_BQIsBot)
			return;

		m_BQMoveTargetUnreachable = true;
		m_BQMoveTargetUnreachablePos = targetPos;
		m_BQMoveTargetUnreachableReason = reason;
		m_BQMoveTargetUnreachableTime = GetGame().GetTime();
		m_BQMoveNavGoalIsFinal = true;
	}

	bool BQBot_IsMoveTargetUnreachable(out vector targetPos, out string reason, out int ageMs)
	{
		targetPos = "0 0 0";
		reason = "";
		ageMs = 0;

		if (!m_BQMoveTargetUnreachable)
			return false;

		targetPos = m_BQMoveTargetUnreachablePos;
		reason = m_BQMoveTargetUnreachableReason;
		if (GetGame())
			ageMs = GetGame().GetTime() - m_BQMoveTargetUnreachableTime;
		return true;
	}

	void BQBot_ClearMoveTargetUnreachable()
	{
		m_BQMoveTargetUnreachable = false;
		m_BQMoveTargetUnreachablePos = "0 0 0";
		m_BQMoveTargetUnreachableReason = "";
		m_BQMoveTargetUnreachableTime = 0;
	}

	void BQBot_SetMoveContext(int state, bool hasLos, bool forceMedium, float distToTarget, float stopDist, float jogDist, float sprintDist, bool combatPose)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQMoveContextState = state;
		m_BQMoveContextHasLos = hasLos;
		m_BQMoveContextForceMedium = forceMedium;
		m_BQMoveContextDistToTarget = Math.Max(0.0, distToTarget);
		m_BQMoveContextStopDist = Math.Max(0.5, stopDist);
		m_BQMoveContextJogDist = Math.Max(m_BQMoveContextStopDist + 1.0, jogDist);
		m_BQMoveContextSprintDist = Math.Max(m_BQMoveContextJogDist + 1.0, sprintDist);
		m_BQMoveContextCombatPose = combatPose;
		m_BQMoveContextValid = true;
	}

	void BQBot_ClearMoveContext()
	{
		m_BQMoveContextValid = false;
		m_BQMoveContextState = 0;
		m_BQMoveContextHasLos = false;
		m_BQMoveContextForceMedium = false;
		m_BQMoveContextDistToTarget = 0.0;
		m_BQMoveContextStopDist = 0.0;
		m_BQMoveContextJogDist = 0.0;
		m_BQMoveContextSprintDist = 0.0;
		m_BQMoveContextCombatPose = false;
	}

	bool BQBot_GetMoveContext(out int state, out bool hasLos, out bool forceMedium, out float distToTarget, out float stopDist, out float jogDist, out float sprintDist, out bool combatPose)
	{
		state = 0;
		hasLos = false;
		forceMedium = false;
		distToTarget = 0.0;
		stopDist = 0.0;
		jogDist = 0.0;
		sprintDist = 0.0;
		combatPose = false;

		if (!m_BQMoveContextValid)
			return false;

		state = m_BQMoveContextState;
		hasLos = m_BQMoveContextHasLos;
		forceMedium = m_BQMoveContextForceMedium;
		distToTarget = m_BQMoveContextDistToTarget;
		stopDist = m_BQMoveContextStopDist;
		jogDist = m_BQMoveContextJogDist;
		sprintDist = m_BQMoveContextSprintDist;
		combatPose = m_BQMoveContextCombatPose;
		return true;
	}

	bool BQBot_GetMoveIntent(out float speed, out float angleDeg)
	{
		speed = 0.0;
		angleDeg = 0.0;

		if (!m_BQMoveIntentValid)
			return false;

		speed = m_BQMoveIntentSpeed;
		angleDeg = m_BQMoveIntentAngle;
		return true;
	}

	void BQBot_SetAimHeading(float yaw, bool turnFeetWanted)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQAimHeading = yaw;
		m_BQAimHeadingValid = true;
		m_BQAimTurnFeetWanted = turnFeetWanted;

		eAIBase ai = eAIBase.Cast(this);
		if (ai)
			ai.OverrideTurnTarget(true, yaw);
	}

	void BQBot_SetAimPitch(float pitch)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQAimPitch = pitch;
		m_BQAimPitchValid = true;
	}

	void BQBot_SetAimTarget(vector targetPos)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQAimTargetPos = targetPos;
		m_BQAimTargetValid = true;
	}

	bool BQBot_GetAimHeading(out float yaw, out bool turnFeetWanted)
	{
		yaw = 0.0;
		turnFeetWanted = false;

		if (!m_BQAimHeadingValid)
			return false;

		yaw = m_BQAimHeading;
		turnFeetWanted = m_BQAimTurnFeetWanted;
		return true;
	}

	void BQBot_ClearAimHeading()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		m_BQAimHeadingValid = false;
		m_BQAimPitchValid = false;
		m_BQAimTargetValid = false;
		m_BQAimTurnFeetWanted = false;

		eAIBase ai = eAIBase.Cast(this);
		if (ai)
			ai.OverrideTurnTarget(false, 0.0);
	}

	bool BQBot_GetAimPitch(out float pitch)
	{
		pitch = 0.0;

		if (!m_BQAimPitchValid)
			return false;

		pitch = m_BQAimPitch;
		return true;
	}

	bool BQBot_GetAimTarget(out vector targetPos)
	{
		targetPos = "0 0 0";

		if (!m_BQAimTargetValid)
			return false;

		targetPos = m_BQAimTargetPos;
		return true;
	}

	void BQBot_SetAccuracyProfile(float baseAccuracy, float nearDistance, float farDistance, float nearFactor)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQAccuracyBase = Math.Clamp(baseAccuracy, 0.0, 1.0);
		m_BQAccuracyNearDistance = Math.Max(1.0, nearDistance);
		m_BQAccuracyFarDistance = Math.Max(m_BQAccuracyNearDistance + 1.0, farDistance);
		m_BQAccuracyNearFactor = Math.Clamp(nearFactor, 0.0, 1.0);
	}

	void BQBot_SetAccuracyFarFactor(float farFactor)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQAccuracyFarFactor = Math.Clamp(farFactor, 0.0, 1.0);
	}

	void BQBot_SetAccuracyMaxSpreadDeg(float maxSpreadDeg)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQAccuracyMaxSpreadDeg = Math.Clamp(maxSpreadDeg, 0.0, 45.0);
	}

	void BQBot_SetDamageMultiplier(float damageMultiplier)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQDamageMultiplier = Math.Clamp(damageMultiplier, 0.0, 10.0);
	}

	float BQBot_GetDamageMultiplier()
	{
		return Math.Clamp(m_BQDamageMultiplier, 0.0, 10.0);
	}

	void BQBot_RecordShotTelemetry(vector firePos, vector fireDir)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQLastShotTelemetryValid = true;
		m_BQLastShotServerTime = GetGame().GetTime();
		m_BQLastShotFirePos = firePos;
		m_BQLastShotFireDir = fireDir.Normalized();
	}

	bool BQBot_GetShotTelemetry(out int shotTime, out vector firePos, out vector fireDir)
	{
		shotTime = 0;
		firePos = "0 0 0";
		fireDir = "0 0 0";

		if (!m_BQLastShotTelemetryValid)
			return false;

		shotTime = m_BQLastShotServerTime;
		firePos = m_BQLastShotFirePos;
		fireDir = m_BQLastShotFireDir;
		return true;
	}

	void BQBot_RecordPlayerShotNoise(vector shotPos)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;
		if (BQBot_IsBot())
			return;

		m_BQLastPlayerShotNoiseTime = GetGame().GetTime();
		m_BQLastPlayerShotNoisePos = shotPos;
	}

	bool BQBot_GetPlayerShotNoise(out int shotTime, out vector shotPos)
	{
		shotTime = 0;
		shotPos = "0 0 0";
		if (m_BQLastPlayerShotNoiseTime <= 0)
			return false;

		shotTime = m_BQLastPlayerShotNoiseTime;
		shotPos = m_BQLastPlayerShotNoisePos;
		return true;
	}

	float BQBot_GetFinalAccuracyForDistance(float distanceMeters)
	{
		float d = distanceMeters;
		if (d < 0.0)
			d = 0.0;

		float nearDist = Math.Max(1.0, m_BQAccuracyNearDistance);
		float farDist = Math.Max(nearDist + 1.0, m_BQAccuracyFarDistance);
		float nearFactor = Math.Clamp(m_BQAccuracyNearFactor, 0.0, 1.0);
		float farFactor = Math.Clamp(m_BQAccuracyFarFactor, 0.0, 1.0);

		float distanceFactor = nearFactor;
		if (d >= farDist)
			distanceFactor = farFactor;
		else if (d > nearDist)
			distanceFactor = Math.Lerp(nearFactor, farFactor, (d - nearDist) / (farDist - nearDist));

		float finalAccuracy = Math.Clamp(m_BQAccuracyBase, 0.0, 1.0) * distanceFactor;
		return Math.Clamp(finalAccuracy, 0.0, 1.0);
	}

	float BQBot_GetSpreadDegForDistance(float distanceMeters)
	{
		float finalAccuracy = BQBot_GetFinalAccuracyForDistance(distanceMeters);
		return Math.Clamp(1.0 - finalAccuracy, 0.0, 1.0) * Math.Clamp(m_BQAccuracyMaxSpreadDeg, 0.0, 45.0);
	}

	void BQBot_RequestVanillaFire()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		string fireReqLog = "bot=" + ToString();
		fireReqLog = fireReqLog + " pendingBefore=" + m_BQPendingVanillaFireRequests.ToString();
		fireReqLog = fireReqLog + " aimTarget=" + m_BQAimTargetPos.ToString();
		fireReqLog = fireReqLog + " intent=" + m_BQFireIntentMode.ToString();
		BQExpansionDiag.Log("BQ_FIRE_REQ", fireReqLog);
		if (m_BQPendingVanillaFireRequests < 1)
			m_BQPendingVanillaFireRequests++;
	}

	bool BQBot_HasVanillaFireRequest()
	{
		return m_BQPendingVanillaFireRequests > 0;
	}

	bool BQBot_ConsumeVanillaFireRequest()
	{
		if (!GetGame() || !GetGame().IsServer())
			return false;

		if (m_BQPendingVanillaFireRequests <= 0)
			return false;

		BQExpansionDiag.Log("BQ_FIRE_REQ", "CONSUME bot=" + ToString() + " pendingBefore=" + m_BQPendingVanillaFireRequests.ToString());
		m_BQPendingVanillaFireRequests--;
		return true;
	}

	void BQBot_SetVanillaTriggerHoldWanted(bool wanted)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		if (m_BQVanillaTriggerHoldWanted != wanted)
			BQExpansionDiag.Log("BQ_TRIGGER_HOLD", "bot=" + ToString() + " wanted=" + BQExpansionDiag.B(wanted));
		m_BQVanillaTriggerHoldWanted = wanted;
	}

	bool BQBot_IsVanillaTriggerHoldWanted()
	{
		return m_BQVanillaTriggerHoldWanted;
	}

	void BQBot_SetFireIntentMode(int mode)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		if (mode < 0)
			mode = 0;
		if (mode > 2)
			mode = 2;

		m_BQFireIntentMode = mode;
	}

	int BQBot_GetFireIntentMode()
	{
		return m_BQFireIntentMode;
	}

	void BQBot_MarkPostReloadSyncNeeded(string reason = "unspecified")
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_BQIsBot)
			return;

		m_BQPostReloadSyncNeeded = true;
	}

	bool BQBot_ConsumePostReloadSyncNeeded()
	{
		if (!GetGame() || !GetGame().IsServer())
			return false;

		if (!m_BQPostReloadSyncNeeded)
			return false;

		m_BQPostReloadSyncNeeded = false;
		return true;
	}

	void BQBot_MarkTrigger()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;
	}

	void BQBot_MarkShot()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		m_BQShotTick++;
		if (m_BQShotTick > 2000000000)
			m_BQShotTick = 1;

		m_BQServerShotCount++;

		SetSynchDirty();
	}

	int BQBot_GetServerShotCount()
	{
		return m_BQServerShotCount;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		if (!m_BQIsBot)
			return;

		if (m_BQShotTick == m_BQLastShotTickClient)
			return;

		m_BQLastShotTickClient = m_BQShotTick;
	}

	override bool CanConsumeStamina(EStaminaConsumers consumer)
	{
		if (BQBot_IsBot())
			return true;

		return super.CanConsumeStamina(consumer);
	}

	override bool CanStartConsumingStamina(EStaminaConsumers consumer)
	{
		if (BQBot_IsBot())
			return true;

		return super.CanStartConsumingStamina(consumer);
	}

	override void DepleteStamina(EStaminaModifiers modifier, float dT = -1)
	{
		if (BQBot_IsBot())
			return;

		super.DepleteStamina(modifier, dT);
	}

	override void InsertAgent(int agent, float count = 1)
	{
		if (BQBot_IsBot())
			return;

		super.InsertAgent(agent, count);
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		if (BQBot_IsBot())
			return;

		super.OnStoreSave(ctx);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (BQBot_IsBot())
			return true;

		return super.OnStoreLoad(ctx, version);
	}

	override bool Save()
	{
		if (BQBot_IsBot())
			return true;

		return super.Save();
	}

	override void OnScheduledTick(float deltaTime)
	{
		if (BQBot_IsBot())
		{
			return;
		}

		super.OnScheduledTick(deltaTime);
	}

	void BQBot_ApplySurvivalDisabledOnce()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!BQBot_IsBot())
			return;

		if (m_BQSurvivalInitDone)
			return;

		m_BQSurvivalInitDone = true;
		BQBot_ApplySurvivalLocks("init");
	}

	void BQBot_ApplySurvivalLocks(string reason = "runtime")
	{
		if (!GetGame() || !GetGame().IsServer() || !BQBot_IsBot() || !IsAlive())
			return;

		// Infinite stamina for bots.
		StaminaHandler stamina = GetStaminaHandler();
		if (stamina)
			stamina.SetStamina(GameConstants.STAMINA_MAX);

		// Freeze food/water stats.
		PlayerStat<float> water = GetStatWater();
		if (water)
			water.Set(water.GetMax());

		PlayerStat<float> energy = GetStatEnergy();
		if (energy)
			energy.Set(energy.GetMax());

		// Neutralize temperature comfort so environment does not affect bot behavior.
		PlayerStat<float> heatComfort = GetStatHeatComfort();
		if (heatComfort)
			heatComfort.Set(0.0);

		// Keep wetness static to avoid cold/wet gameplay penalties.
		PlayerStat<int> wet = GetStatWet();
		if (wet)
			wet.Set(0);

		// Clear all disease agents.
		RemoveAllAgents();

		// Disable bleeding and blood drain.
		BleedingSourcesManagerServer bleedMgr = GetBleedingManagerServer();
		if (bleedMgr)
		{
			bleedMgr.SetBloodLoss(true);
			if (GetBleedingBits() != 0)
				bleedMgr.RemoveAllSources();
		}

		// Keep blood at max so the bot does not suffer blood mechanics.
		float maxBlood = GetMaxHealth("GlobalHealth", "Blood");
		if (maxBlood > 0.0)
			SetHealth("GlobalHealth", "Blood", maxBlood);

		// Keep shock at max to avoid uncon states from melee/other hit logic.
		float maxShock = GetMaxHealth("GlobalHealth", "Shock");
		if (maxShock > 0.0)
			SetHealth("GlobalHealth", "Shock", maxShock);

		if (IsUnconscious())
			DayZPlayerSyncJunctures.SendPlayerUnconsciousness(this, false);
	}

	void BQBot_OnCombatDamageApplied()
	{
		if (!GetGame() || !GetGame().IsServer() || !BQBot_IsBot() || !IsAlive())
			return;

		// Event-based lock refresh after hits (no per-tick maintenance).
		BQBot_ApplySurvivalLocks("after-hit");
	}

	void BQBot_ResetSurvivalInitFlag()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		m_BQSurvivalInitDone = false;
	}

	protected bool BQBot_IsProjectileAmmo(string ammo)
	{
		if (ammo == "")
			return false;

		string lower = ammo;
		lower.ToLower();
		if (lower.Contains("bullet_"))
			return true;
		if (lower.Contains("ammo_"))
			return true;
		return false;
	}

	protected PlayerBase BQBot_GetDamageSourcePlayer(EntityAI source)
	{
		if (!source)
			return null;

		PlayerBase srcPlayer = PlayerBase.Cast(source);
		if (srcPlayer)
			return srcPlayer;

		Man rootMan = source.GetHierarchyRootPlayer();
		if (rootMan)
		{
			srcPlayer = PlayerBase.Cast(rootMan);
			if (srcPlayer)
				return srcPlayer;
		}

		Object parent = source.GetHierarchyParent();
		int guard = 0;
		while (parent && guard < 10)
		{
			srcPlayer = PlayerBase.Cast(parent);
			if (srcPlayer)
				return srcPlayer;

			EntityAI parentEnt = EntityAI.Cast(parent);
			if (parentEnt)
			{
				rootMan = parentEnt.GetHierarchyRootPlayer();
				if (rootMan)
				{
					srcPlayer = PlayerBase.Cast(rootMan);
					if (srcPlayer)
						return srcPlayer;
				}

				parent = parentEnt.GetHierarchyParent();
			}
			else
			{
				parent = null;
			}

			guard++;
		}

		return null;
	}

	protected PlayerBase BQBot_FindLikelyBotShooterFromTelemetry(string ammo)
	{
		if (!GetGame() || !GetGame().IsServer())
			return null;

		if (!BQBot_IsProjectileAmmo(ammo))
			return null;

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		if (!players || players.Count() == 0)
			return null;

		vector victimPos = GetPosition();
		int now = GetGame().GetTime();
		PlayerBase best = null;
		float bestScore = -2.0;

		foreach (Man man : players)
		{
			PlayerBase bot = PlayerBase.Cast(man);
			if (!bot || !bot.IsAlive() || !bot.BQBot_IsBot())
				continue;

			int shotTime;
			vector shotPos;
			vector shotDir;
			if (!bot.BQBot_GetShotTelemetry(shotTime, shotPos, shotDir))
				continue;

			int ageMs = now - shotTime;
			if (ageMs < 0 || ageMs > 900)
				continue;

			vector toVictim = victimPos - shotPos;
			float toVictimLenSq = toVictim[0] * toVictim[0] + toVictim[1] * toVictim[1] + toVictim[2] * toVictim[2];
			if (toVictimLenSq < 0.04 || toVictimLenSq > 640000.0)
				continue;

			vector dirToVictim = toVictim.Normalized();
			float dot = vector.Dot(shotDir, dirToVictim);
			if (dot < 0.95)
				continue;

			float score = dot - (ageMs * 0.0001);
			if (score > bestScore)
			{
				bestScore = score;
				best = bot;
			}
		}

		return best;
	}

	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (BQBot_IsBot())
		{
			PlayerBase srcPlayer = BQBot_GetDamageSourcePlayer(source);
			bool sourceIsBot = false;
			if (srcPlayer && srcPlayer.BQBot_IsBot())
				sourceIsBot = true;
			bool projectileAmmo = BQBot_IsProjectileAmmo(ammo);

			// Hard deny bot-vs-bot damage. Also deny unresolved projectile source as safe fallback.
			if (sourceIsBot || (!srcPlayer && projectileAmmo))
			{
				if (BQBOT_FF_DAMAGE_DIAG)
				{
					string sourceType = "null";
					if (source)
						sourceType = source.GetType();
					string srcType = "null";
					if (srcPlayer)
						srcType = srcPlayer.GetType();
					Print("[EAI] [ffdamage] blocked dmg victim=" + GetType() + " source=" + sourceType + " srcPlayer=" + srcType + " ammo=" + ammo);
				}

				return false;
			}
		}
		else if (GetGame() && GetGame().IsServer() && damageResult)
		{
			PlayerBase srcPlayerHuman = BQBot_GetDamageSourcePlayer(source);
			if (!srcPlayerHuman)
				srcPlayerHuman = BQBot_FindLikelyBotShooterFromTelemetry(ammo);

			if (srcPlayerHuman && srcPlayerHuman.BQBot_IsBot())
			{
				float damageMultiplier = srcPlayerHuman.BQBot_GetDamageMultiplier();
				if (Math.AbsFloat(damageMultiplier - 1.0) > 0.001)
				{
					string applyZone = dmgZone;
					float healthDamage = damageResult.GetDamage(applyZone, "Health");
					float shockDamage = damageResult.GetDamage(applyZone, "Shock");
					float bloodDamage = damageResult.GetDamage(applyZone, "Blood");
					if ((Math.AbsFloat(healthDamage) <= 0.0001 && Math.AbsFloat(shockDamage) <= 0.0001 && Math.AbsFloat(bloodDamage) <= 0.0001) && applyZone != "")
					{
						applyZone = "";
						healthDamage = damageResult.GetDamage(applyZone, "Health");
						shockDamage = damageResult.GetDamage(applyZone, "Shock");
						bloodDamage = damageResult.GetDamage(applyZone, "Blood");
					}

					// Read vanilla damage from the hit zone, but apply scaled damage to
					// global player stats so HUD health and death state stay consistent.
					if (Math.AbsFloat(healthDamage) > 0.0001)
						AddHealth("", "Health", -healthDamage * damageMultiplier);
					if (Math.AbsFloat(shockDamage) > 0.0001)
						AddHealth("", "Shock", -shockDamage * damageMultiplier);
					if (Math.AbsFloat(bloodDamage) > 0.0001)
						AddHealth("", "Blood", -bloodDamage * damageMultiplier);

					if (BQBOT_DMG_MULT_DIAG)
					{
						string dmgLog = "[EAI] [dmgmult] pre-apply source=" + srcPlayerHuman.GetType();
			dmgLog = dmgLog + " mult=" + damageMultiplier.ToString();
			dmgLog = dmgLog + " zone=" + applyZone;
			dmgLog = dmgLog + " h=" + healthDamage.ToString();
			dmgLog = dmgLog + " s=" + shockDamage.ToString();
			dmgLog = dmgLog + " b=" + bloodDamage.ToString();
			Print(dmgLog);
					}

					// Cancel vanilla application so damage is applied exactly once with multiplier.
					return false;
				}
			}
		}

		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		// Re-apply bot survival locks after legit damage events.
		if (BQBot_IsBot())
			BQBot_OnCombatDamageApplied();
	}

	// Defensive dedicated-server guard:
	// some mod stacks can trigger EEHitByRemote on server, while vanilla PlayerBase
	// tries to play blocking sound there (EffectSound), which throws VM exceptions.
	override void EEHitByRemote(int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
	{
		if (GetGame() && !GetGame().IsClient())
			return;

		super.EEHitByRemote(damageType, source, component, dmgZone, ammo, modelPos);
	}
}