class BotState_Patrol
{
	static const float WAYPOINT_PAUSE = 2.0; // пауза между точками в секундах
	static float s_WaypointPauseTimer = 0;
	static bool s_IsWaiting = false;

	static void Enter(BotUnit bot)
	{
		bot.m_CurrentState = EBotState.PATROL;
		bot.m_PatrolTimer = 0;
		bot.m_WaypointIndex = 0;
		bot.SetMovementSpeed(1.0);
		s_WaypointPauseTimer = 0;
		s_IsWaiting = false;
		Print("[MyBots] Бот перешёл в PATROL");
	}

	static void Update(BotUnit bot, float dt)
	{
		bot.m_PatrolTimer += dt;

		// Ждём паузу между вейпоинтами
		if (s_IsWaiting)
		{
			s_WaypointPauseTimer -= dt;
			if (s_WaypointPauseTimer <= 0)
				s_IsWaiting = false;

			// Во время паузы сканируем врагов
			CheckForEnemies(bot);
			return;
		}

		vector targetPos = bot.GetCurrentWaypoint();
		vector botPos = bot.m_Player.GetPosition();
		float dist = vector.Distance(botPos, targetPos);

		if (dist < 3.0)
		{
			// Достигли точки — делаем паузу
			bot.AdvanceWaypoint();
			bot.StopMovement();
			s_IsWaiting = true;
			s_WaypointPauseTimer = WAYPOINT_PAUSE;
			CheckForEnemies(bot);
			return;
		}

		// Ищем путь
		TVectorArray path = bot.m_Navigation.FindPathTo(targetPos);
		if (path.Count() > 1)
			bot.MoveTo(path[1]);
		else
			bot.MoveTo(targetPos);

		CheckForEnemies(bot);
	}

	static void CheckForEnemies(BotUnit bot)
	{
		// Слух
		float soundPriority;
		vector soundPos = bot.m_Hearing.DetectSound(bot.m_DetectionCfg, soundPriority);
		if (soundPriority > 5)
		{
			bot.m_LastKnownEnemyPos = soundPos;
			bot.ProcessEvent(new BotEvent(EBotEventType.HEARD_SOUND, NULL, soundPos, soundPriority));
			return;
		}

		// Зрение
		EntityAI detected = bot.m_Vision.ScanForEnemies(bot.m_DetectionCfg);
		if (detected)
		{
			bot.m_CurrentEnemy = detected;
			bot.m_LastKnownEnemyPos = detected.GetPosition();
			bot.ProcessEvent(new BotEvent(EBotEventType.ENEMY_DETECTED, detected, detected.GetPosition()));
		}
	}

	static EBotState CheckTransitions(BotUnit bot, BotEvent ev)
	{
		if (ev.m_Type == EBotEventType.ENEMY_DETECTED) return EBotState.ALERT;
		if (ev.m_Type == EBotEventType.HEARD_SOUND)    return EBotState.ALERT;
		if (ev.m_Type == EBotEventType.STOP)           return EBotState.IDLE;
		return EBotState.PATROL;
	}
};