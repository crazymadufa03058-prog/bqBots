class BotUnit
{
	PlayerBase m_Player;
	EBotState m_CurrentState;

	ref BotNavigation m_Navigation;
	ref BotCoverFinder m_CoverFinder;
	ref BotSensor_Vision m_Vision;
	ref BotSensor_Hearing m_Hearing;
	ref BotAimController m_AimController;
	ref BotWeaponHandler m_WeaponHandler;

	ref BotZoneConfig m_ZoneConfig;
	ref BotLoadoutConfig m_LoadoutConfig;
	ref BotSettings m_Settings;
	ref BotDetectionConfig m_DetectionCfg;
	ref BotAccuracyConfig m_AccuracyCfg;
	ref BotDamageConfig m_DamageCfg;

	EntityAI m_CurrentEnemy;
	vector m_LastKnownEnemyPos;

	ref array<vector> m_Waypoints;
	int m_WaypointIndex;
	bool m_UseRoam;

	float m_PatrolTimer;
	float m_AlertTimer;
	float m_CombatTimer;
	float m_FlankTimer;
	float m_NextFlankCheck;
	float m_SearchTimer;
	float m_SpawnTime;

	bool m_IsActive;
	float m_DistanceToNearestPlayer;

	// Движение
	float m_MoveSpeed;
	bool m_IsMoving;
	vector m_MoveTarget;
	float m_CurrentYaw;
	float m_TargetAimYaw;

	ref BotSquad m_Squad;
	bool m_IsRetreating;
	float m_RetreatStartTime;
	float m_RetreatCooldown;
	vector m_RetreatTarget;
	bool m_IsMovingToBuilding;

	void BotUnit(PlayerBase player, BotZoneConfig zoneConfig, BotSettings settings, BotLoadoutConfig loadoutConfig)
	{
		m_Player = player;
		m_CurrentState = EBotState.IDLE;
		m_ZoneConfig = zoneConfig;
		m_Settings = settings;
		m_LoadoutConfig = loadoutConfig;

		m_DetectionCfg = settings.m_Detection;
		m_AccuracyCfg = settings.m_Accuracy;
		m_DamageCfg = settings.m_Damage;

		m_Navigation = new BotNavigation(player);
		m_CoverFinder = new BotCoverFinder(player, m_Navigation);
		m_Vision = new BotSensor_Vision(player);
		m_Hearing = new BotSensor_Hearing(player);
		m_AimController = new BotAimController(player);
		m_WeaponHandler = new BotWeaponHandler(player, m_AimController);

		m_Waypoints = new array<vector>();
		m_WaypointIndex = 0;
		m_UseRoam = false;

		if (zoneConfig.m_Waypoints.Count() > 0)
		{
			for (int i = 0; i < zoneConfig.m_Waypoints.Count(); i++)
				m_Waypoints.Insert(zoneConfig.m_Waypoints[i]);
		}
		else
		{
			m_UseRoam = true;
		}

		m_CurrentEnemy = NULL;
		m_LastKnownEnemyPos = "0 0 0";
		m_PatrolTimer = 0;
		m_AlertTimer = 0;
		m_CombatTimer = 0;
		m_FlankTimer = 0;
		m_NextFlankCheck = 0;
		m_SearchTimer = 0;
		m_IsActive = true;
		m_DistanceToNearestPlayer = 0;
		m_MoveSpeed = 1.0;
		m_IsMoving = false;
		m_MoveTarget = "0 0 0";
		m_CurrentYaw = player.GetOrientation()[0];
		m_SpawnTime = GetGame().GetTickTime();

		m_Squad = NULL;
		m_IsRetreating = false;
		m_RetreatStartTime = 0;
		m_RetreatCooldown = 0;
		m_RetreatTarget = "0 0 0";
		m_IsMovingToBuilding = false;
	}

	void Start()
	{
		ProcessEvent(new BotEvent(EBotEventType.START));
	}

	void Stop()
	{
		ProcessEvent(new BotEvent(EBotEventType.STOP));
		m_IsActive = false;
	}

	void ProcessEvent(BotEvent ev)
	{
		EBotState newState = m_CurrentState;

		switch (m_CurrentState)
		{
			case EBotState.IDLE:   newState = BotState_Idle.CheckTransitions(this, ev);   break;
			case EBotState.PATROL: newState = BotState_Patrol.CheckTransitions(this, ev); break;
			case EBotState.ALERT:  newState = BotState_Alert.CheckTransitions(this, ev);  break;
			case EBotState.COMBAT: newState = BotState_Combat.CheckTransitions(this, ev); break;
			case EBotState.SEARCH: newState = BotState_Search.CheckTransitions(this, ev); break;
		}

		if (newState != m_CurrentState)
		{
			ExitState(m_CurrentState);
			EnterState(newState);
		}
	}

	// Вызывается из CommandHandler — изнутри физического цикла движка
	void OnCommandHandlerUpdate(float dt)
	{
		if (!m_IsActive)
			return;

		if (m_RetreatCooldown > 0)
			m_RetreatCooldown -= dt;

		// В бою: НЕ ДВИГАЕМСЯ. Всё управление через BotSurvivorBase.CommandHandler:
		// - OverrideMovementSpeed(true, 0) — стоим
		// - SetOrientation — поворот к врагу
		// - AnimSetFloat AimX/AimY — наклон ствола
		if (m_CurrentState == EBotState.COMBAT)
		{
			// Движение полностью блокировано в BotSurvivorBase
			return;
		}

		// В Alert: тоже стоим, ждём подтверждения врага
		if (m_CurrentState == EBotState.ALERT)
		{
			m_Player.GetInputController().OverrideMovementSpeed(true, 0);
			m_Player.GetInputController().OverrideMovementAngle(true, 0);
			return;
		}

		// Patrol / Search / Idle: нормальное движение
		ApplyMovementFromCommandHandler(dt);
	}

	protected void ApplyMovementFromCommandHandler(float dt)
	{
		if (!m_IsMoving || m_MoveSpeed <= 0)
		{
			m_IsMoving = false;
			m_Player.GetInputController().OverrideMovementSpeed(true, 0);
			m_Player.GetInputController().OverrideMovementAngle(true, 0);
			return;
		}

		vector curPos = m_Player.GetPosition();
		vector toTarget = m_MoveTarget - curPos;
		toTarget[1] = 0;
		float dist = toTarget.Length();

		if (dist < 0.5)
		{
			m_IsMoving = false;
			m_Player.GetInputController().OverrideMovementSpeed(true, 0);
			m_Player.GetInputController().OverrideMovementAngle(true, 0);
			return;
		}

		toTarget = toTarget.Normalized();

		// Угол к цели в мировых координатах
		float targetYaw = Math.Atan2(toTarget[0], toTarget[2]) * Math.RAD2DEG;

		// Вне боя: плавный поворот тела к цели, идём вперёд (moveAngle=0)
		float yawDiff = targetYaw - m_CurrentYaw;
		while (yawDiff > 180)  yawDiff -= 360;
		while (yawDiff < -180) yawDiff += 360;
		m_CurrentYaw += Math.Clamp(yawDiff, -180.0 * dt, 180.0 * dt);

		vector orient = m_Player.GetOrientation();
		orient[0] = m_CurrentYaw;
		orient[1] = 0;
		m_Player.SetOrientation(orient);

		float speed;
		if (m_MoveSpeed >= 2.0)
			speed = 2.0;
		else
			speed = 1.0;

		m_Player.GetInputController().OverrideMovementSpeed(true, speed);
		m_Player.GetInputController().OverrideMovementAngle(true, 0);
	}

	void OnUpdate(float dt)
	{
		if (!m_IsActive)
			return;

		if (m_Player.IsDamageDestroyed())
		{
			m_IsActive = false;
			if (m_Squad)
				m_Squad.RemoveMember(this);
			return;
		}

		switch (m_CurrentState)
		{
			case EBotState.IDLE:   BotState_Idle.Update(this, dt);   break;
			case EBotState.PATROL: BotState_Patrol.Update(this, dt); break;
			case EBotState.ALERT:  BotState_Alert.Update(this, dt);  break;
			case EBotState.COMBAT: BotState_Combat.Update(this, dt); break;
			case EBotState.SEARCH: BotState_Search.Update(this, dt); break;
		}
	}

	void ExitState(EBotState state)
	{
		switch (state)
		{
			case EBotState.COMBAT:
				m_WeaponHandler.StopFire();
				m_AimController.ClearOverrides();
				m_Player.Bot_ClearFireRequest();
				break;
			case EBotState.ALERT:
				m_AimController.ClearOverrides();
				m_Player.Bot_ClearFireRequest();
				break;
		}
	}

	void EnterState(EBotState state)
	{
		m_CurrentState = state;
		switch (state)
		{
			case EBotState.IDLE:   BotState_Idle.Enter(this);   break;
			case EBotState.PATROL: BotState_Patrol.Enter(this); break;
			case EBotState.ALERT:  BotState_Alert.Enter(this);  break;
			case EBotState.COMBAT: BotState_Combat.Enter(this); break;
			case EBotState.SEARCH: BotState_Search.Enter(this); break;
		}
	}

	vector GetCurrentWaypoint()
	{
		if (m_UseRoam)
			return BotNavigation.GetRandomPointInRadius(m_ZoneConfig.m_Center, m_ZoneConfig.m_Radius);

		if (m_Waypoints.Count() == 0)
			return m_ZoneConfig.m_Center;

		return m_Waypoints[m_WaypointIndex % m_Waypoints.Count()];
	}

	void AdvanceWaypoint()
	{
		if (!m_UseRoam)
			m_WaypointIndex++;
	}

	void MoveTo(vector pos)
	{
		if (!m_Player)
			return;

		float dist = vector.Distance(m_Player.GetPosition(), pos);
		if (dist < 0.5)
			return;

		m_MoveTarget = pos;
		m_IsMoving = true;
	}

	void MoveToIndoor(vector pos)
	{
		m_IsMovingToBuilding = true;
		m_Navigation.ClearCache();
		TVectorArray path = m_Navigation.FindPathToIndoor(pos);

		if (path.Count() > 1)
		{
			MoveTo(path[1]);
			SetMovementSpeed(1.0);
		}
		else
		{
			MoveTo(pos);
		}
	}

	void StartRetreat(vector retreatTarget)
	{
		m_IsRetreating = true;
		m_RetreatStartTime = GetGame().GetTickTime();
		m_RetreatTarget = retreatTarget;
		m_RetreatCooldown = 30.0;
		m_WeaponHandler.StopFire();
		m_AimController.ClearOverrides();
		SetMovementSpeed(2.0);

		vector buildingPos = m_Navigation.FindNearestBuilding(80.0);
		if (buildingPos != "0 0 0")
		{
			float buildingDist = vector.Distance(m_Player.GetPosition(), buildingPos);
			float retreatDist = vector.Distance(m_Player.GetPosition(), retreatTarget);

			if (buildingDist < retreatDist * 1.5)
			{
				m_RetreatTarget = m_Navigation.GetNearestNavmeshPos(buildingPos, 10);
				m_IsMovingToBuilding = true;
				Print("[MyBots] Бот отступает в здание");
				return;
			}
		}

		Print("[MyBots] Бот отступает к укрытию");
	}

	void EndRetreat()
	{
		m_IsRetreating = false;
		m_IsMovingToBuilding = false;
		m_RetreatTarget = "0 0 0";
		Print("[MyBots] Бот завершил отступление");
	}

	void StopMovement()
	{
		m_IsMoving = false;
		m_Player.GetInputController().OverrideMovementSpeed(true, 0);
		m_Player.GetInputController().OverrideMovementAngle(true, 0);
	}

	void SetMovementSpeed(float speed)
	{
		m_MoveSpeed = speed;
	}

	void SetStance(int stanceMask)
	{
		if (stanceMask & DayZPlayerConstants.STANCEMASK_PRONE)
			m_MoveSpeed = 0.3;
		else if (stanceMask & DayZPlayerConstants.STANCEMASK_CROUCH)
			m_MoveSpeed = 0.8;
	}

	bool HasWeapon()
	{
		return m_Player.GetItemInHands() != NULL;
	}

	bool IsAlive()
	{
		return m_Player && !m_Player.IsDamageDestroyed() && m_IsActive;
	}

	float GetHP()
	{
		return m_Player.GetHealth("", "Health");
	}

	bool CanRetreat()
	{
		return !m_IsRetreating && m_RetreatCooldown <= 0;
	}
};