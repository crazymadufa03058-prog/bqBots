modded class PlayerBase
{
	private ref BotCommandCombat m_BotCombatCmd;
	private float m_BotDebugTimer;

	private bool   m_BotWantFire;
	private bool   m_BotWantReload;

	private float m_BotReloadCooldown;
	private float m_BotStunTimer;
	private bool  m_BotDeleted;

	bool IsBotDeleted()
	{
		return m_BotDeleted;
	}

	void Bot_StartCombatCommand()
	{
		if (m_BotCombatCmd)
			return;

		Human human = Human.Cast(this);
		if (!human)
		{
			Print("[MyBots][CMD][ERROR] Bot_StartCombatCommand: Human.Cast(this) failed for " + GetType());
			return;
		}

		m_BotCombatCmd = new BotCommandCombat(human);
		human.StartCommand_Script(m_BotCombatCmd);

		HumanInputController hic = GetInputController();
		if (hic)
		{
			hic.OverrideRaise(HumanInputControllerOverrideType.ENABLED, true);
			Print("[MyBots][CMD] OverrideRaise(ENABLED) called for " + GetType());
		}
		else
		{
			Print("[MyBots][CMD][ERROR] GetInputController() returned NULL for " + GetType());
		}

		// Отключаем отдачу — боты не будут задирать ствол вверх
		GetAimingModel().SetProceduralRecoilEnabled(false);
		Print("[MyBots][CMD] ProceduralRecoil disabled for " + GetType());

		// Verify raise state
		bool isRaised = IsRaised();
		bool isRaiseCompleted = IsWeaponRaiseCompleted();
		Print("[MyBots][CMD] CombatCommand started: isRaised=" + isRaised + " isRaiseCompleted=" + isRaiseCompleted + " weapon=" + GetItemInHands().GetType());

		m_BotStunTimer = 0;
	}

	void Bot_StopCombatCommand()
	{
		if (!m_BotCombatCmd)
			return;

		Print("[MyBots][CMD] Bot_StopCombatCommand called for " + GetType());

		m_BotCombatCmd.Invalidate();

		Human human = Human.Cast(this);
		if (human)
		{
			HumanCommandScript cur = human.GetCommand_Script();
			if (cur == m_BotCombatCmd)
				m_BotCombatCmd.SetFlagFinished(true);
		}
		m_BotCombatCmd = null;

		HumanInputController hic = GetInputController();
		if (hic)
		{
			hic.OverrideRaise(HumanInputControllerOverrideType.DISABLED, false);
			Print("[MyBots][CMD] OverrideRaise(DISABLED) called");
		}

		m_BotStunTimer = 0;
	}

	void Bot_RequestFire()
	{
		m_BotWantFire = true;
	}

	void Bot_RequestReload()
	{
		m_BotWantReload = true;
	}

	void Bot_ClearFireRequest()
	{
		m_BotWantFire   = false;
		m_BotWantReload = false;
	}

	private void Bot_RotateToward(vector targetPos, float dt)
	{
		vector curPos = GetPosition();
		vector toTarget = targetPos - curPos;
		toTarget[1] = 0;
		if (toTarget.Length() < 0.1)
			return;

		toTarget = toTarget.Normalized();
		float desiredYaw = Math.Atan2(toTarget[0], toTarget[2]) * Math.RAD2DEG;

		vector orient = GetOrientation();
		float currentYaw = orient[0];
		float yawDiff = desiredYaw - currentYaw;
		while (yawDiff >  180) yawDiff -= 360;
		while (yawDiff < -180) yawDiff += 360;

		float maxTurn = 300.0 * dt;
		orient[0] = currentYaw + Math.Clamp(yawDiff, -maxTurn, maxTurn);
		orient[1] = 0;
		SetOrientation(orient);
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		if (!IsAlive() || IsDamageDestroyed())
		{
			super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
			return;
		}

		BotUnit botUnit = BotManager.Get().FindBotByPlayer(this);

		if (!botUnit || !botUnit.m_IsActive)
		{
			super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
			return;
		}

		bool inCombatOrAlert = (botUnit.m_CurrentState == EBotState.COMBAT || botUnit.m_CurrentState == EBotState.ALERT);

		if (inCombatOrAlert)
		{
			if (m_BotCombatCmd)
			{
				HumanCommandScript activeCmd = GetCommand_Script();
				if (activeCmd != m_BotCombatCmd)
				{
					Print("[MyBots][CMD] Active command mismatch, recreating BotCommandCombat");
					m_BotCombatCmd = null;
				}
			}

			if (!m_BotCombatCmd)
			{
				Print("[MyBots][CMD] Starting combat command from CommandHandler");
				Bot_StartCombatCommand();
			}

			if (!m_BotCombatCmd)
			{
				super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
				return;
			}

			if (m_BotStunTimer > 0)
				m_BotStunTimer -= pDt;

			if (m_BotStunTimer > 0)
			{
				m_BotCombatCmd.SetStun(m_BotStunTimer);
				super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
				return;
			}

			vector targetPos = "0 0 0";
			bool hasTarget   = false;
			EntityAI safeEnemy = NULL;

			if (botUnit.m_CurrentEnemy)
			{
				if (botUnit.m_CurrentEnemy.IsAlive() && !botUnit.m_CurrentEnemy.IsDamageDestroyed())
				{
					safeEnemy = botUnit.m_CurrentEnemy;
					targetPos = safeEnemy.GetPosition();
					hasTarget  = true;
				}
				else
				{
					botUnit.m_CurrentEnemy = null;
				}
			}
			else if (botUnit.m_LastKnownEnemyPos != "0 0 0")
			{
				targetPos = botUnit.m_LastKnownEnemyPos;
				hasTarget  = true;
			}

		if (hasTarget)
		{
			Bot_RotateToward(targetPos, pDt);

			// Расчёт вертикального угла прицеливания (AimY)
			vector curPos2 = GetPosition();
			vector delta = targetPos - curPos2;
			float hDist = Math.Sqrt(delta[0] * delta[0] + delta[2] * delta[2]);
			float aimYDeg = 0;
			if (hDist > 0.1)
			{
				aimYDeg = Math.Atan2(delta[1], hDist) * Math.RAD2DEG;
				aimYDeg = Math.Clamp(aimYDeg, -60, 60);
			}
			if (m_BotCombatCmd)
				m_BotCombatCmd.SetAim(0, aimYDeg);
		}
		else
		{
			// Нет цели — сбрасываем Aim в 0
			if (m_BotCombatCmd)
				m_BotCombatCmd.SetAim(0, 0);
		}

			m_BotDebugTimer -= pDt;
			if (m_BotDebugTimer <= 0)
			{
				m_BotDebugTimer = 2.0;
				ref HumanMovementState ms = new HumanMovementState();
				GetMovementState(ms);
				string enemyInfo = "none";
				if (safeEnemy)
					enemyInfo = safeEnemy.GetType() + " dist=" + vector.Distance(GetPosition(), safeEnemy.GetPosition());
				EntityAI hands = GetItemInHands();
				string weaponInfo = "none";
				if (hands)
					weaponInfo = hands.GetType();
				Print("[MyBots][DEBUG] state=" + botUnit.m_CurrentState + " cmdID=" + pCurrentCommandID + " stance=" + ms.m_iStanceIdx + " raised=" + ms.IsRaised() + " weapon=" + weaponInfo + " enemy=" + enemyInfo);
			}

			if (botUnit.m_CurrentState == EBotState.COMBAT && m_BotWantFire)
			{
				// Стрельба обрабатывается в BotWeaponHandler.TryFire() из BotState_Combat.Update
				// Здесь только сбрасываем флаг
				Bot_ClearFireRequest();
			}

			if (m_BotReloadCooldown > 0)
				m_BotReloadCooldown -= pDt;

			if (botUnit.m_CurrentState == EBotState.COMBAT && m_BotWantReload && m_BotReloadCooldown <= 0)
			{
				Weapon_Base weapon = Weapon_Base.Cast(GetItemInHands());
				if (weapon)
				{
					Magazine mag = Magazine.Cast(weapon.GetAttachmentByType(Magazine));
					if (mag)
					{
						int ammoBefore = mag.GetAmmoCount();
						mag.ServerSetAmmoCount(mag.GetAmmoMax());
						Print("[MyBots][RELOAD] Ammo: " + ammoBefore + " -> " + mag.GetAmmoCount());

						int muzzle = weapon.GetCurrentMuzzle();
						if (weapon.IsChamberEmpty(muzzle) || weapon.IsChamberFiredOut(muzzle))
						{
							float damage;
							string type;
							if (mag.LocalAcquireCartridge(damage, type))
							{
								weapon.PushCartridgeToChamber(muzzle, damage, type);
								weapon.SelectionBulletShow();
								Print("[MyBots][RELOAD] Патрон дослан в патронник damage=" + damage + " type=" + type);
							}
							else
							{
								Print("[MyBots][RELOAD][ERROR] LocalAcquireCartridge failed");
							}
						}
						else
						{
							Print("[MyBots][RELOAD] Патрон уже в патроннике");
						}

						m_BotReloadCooldown = 3.0;
					}
					else
					{
						Print("[MyBots][RELOAD][ERROR] No magazine attached to " + weapon.GetType());
					}
				}
				else
				{
					Print("[MyBots][RELOAD][ERROR] No weapon in hands");
				}
				m_BotWantReload = false;
			}
			else if (m_BotWantReload)
			{
				m_BotWantReload = false;
			}
		}
		else
		{
			if (m_BotCombatCmd)
				Bot_StopCombatCommand();

			Bot_ClearFireRequest();

			super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		}

		if (botUnit && !IsDamageDestroyed())
			botUnit.OnCommandHandlerUpdate(pDt);
	}

	override void EEDelete(EntityAI parent)
	{
		m_BotDeleted = true;
		if (m_BotCombatCmd)
		{
			m_BotCombatCmd.Invalidate();
			m_BotCombatCmd = null;
		}
		super.EEDelete(parent);
		BotManager.Get().ClearEnemyRef(this);
		Print("[MyBots][DELETE] EEDelete: очищены ссылки на " + GetType());
	}

	override void EEKilled(Object killer)
	{
		if (m_BotCombatCmd)
		{
			m_BotCombatCmd.Invalidate();
			m_BotCombatCmd = null;
		}
		super.EEKilled(killer);
		BotManager.Get().ClearEnemyRef(this);
		Print("[MyBots][DELETE] EEKilled: очищены ссылки на " + GetType());
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (!IsAlive() || IsDamageDestroyed())
			return;

		m_BotStunTimer = 0.8;
		if (m_BotCombatCmd)
			m_BotCombatCmd.SetStun(0.8);

		BotUnit botUnit = BotManager.Get().FindBotByPlayer(this);
		if (!botUnit || !botUnit.m_IsActive)
			return;

		if (!IsAlive() || IsDamageDestroyed())
			return;

		PlayerBase attacker = PlayerBase.Cast(source);
		if (!attacker || !attacker.IsAlive())
			return;

		if (BotManager.Get().FindBotByPlayer(attacker))
			return;

		Print("[MyBots][HIT] Удар от " + attacker.GetType() + " state=" + botUnit.m_CurrentState);

		botUnit.m_CurrentEnemy      = attacker;
		botUnit.m_LastKnownEnemyPos = attacker.GetPosition();

		if (botUnit.m_CurrentState != EBotState.COMBAT)
			botUnit.ProcessEvent(new BotEvent(EBotEventType.ENEMY_DETECTED, attacker, attacker.GetPosition()));
	}
};
