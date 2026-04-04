class BotState_Search
{
	static const float SEARCH_TIMEOUT = 30.0;

	static void Enter(BotUnit bot)
	{
		bot.m_CurrentState = EBotState.SEARCH;
		bot.m_SearchTimer = 0;
		bot.SetMovementSpeed(2.0);
		bot.SetStance(DayZPlayerConstants.STANCEMASK_ERECT);
		bot.m_WeaponHandler.StopFire();
		bot.m_Navigation.ClearCache();

		bot.m_Player.Bot_StopCombatCommand();

		if (bot.m_LastKnownEnemyPos != "0 0 0")
		{
			vector buildingPos = bot.m_Navigation.FindNearestBuilding(50.0);
			if (buildingPos != "0 0 0" && vector.Distance(bot.m_Player.GetPosition(), buildingPos) < 30.0)
			{
				bot.MoveToIndoor(buildingPos);
				Print("[MyBots] Бот ищет врага в здании");
			}
			else
			{
				bot.MoveTo(bot.m_LastKnownEnemyPos);
			}
		}

		Print("[MyBots] Бот перешёл в SEARCH");
	}

	static void Update(BotUnit bot, float dt)
	{
		bot.m_SearchTimer += dt;

		if (bot.m_LastKnownEnemyPos != "0 0 0")
		{
			vector botPos = bot.m_Player.GetPosition();
			float dist = vector.Distance(botPos, bot.m_LastKnownEnemyPos);

			if (dist > 5.0)
			{
				TVectorArray path;
				if (bot.m_IsMovingToBuilding)
				{
					path = bot.m_Navigation.FindPathToIndoor(bot.m_LastKnownEnemyPos);
					if (path.Count() > 1)
					{
						bot.MoveTo(path[1]);
						bot.SetMovementSpeed(1.0);
						bot.SetStance(DayZPlayerConstants.STANCEMASK_CROUCH);
					}
					else
					{
						bot.MoveTo(bot.m_LastKnownEnemyPos);
					}
				}
				else
				{
					path = bot.m_Navigation.FindPathTo(bot.m_LastKnownEnemyPos);
					if (path.Count() > 1)
					{
						bot.MoveTo(path[1]);
					}
					else
					{
						bot.MoveTo(bot.m_LastKnownEnemyPos);
					}
				}
			}
			else
			{
				bot.StopMovement();
				bot.m_IsMovingToBuilding = false;
			}
		}

		EntityAI detected = bot.m_Vision.ScanForEnemies(bot.m_DetectionCfg);
		if (detected)
		{
			bot.m_CurrentEnemy = detected;
			bot.m_LastKnownEnemyPos = detected.GetPosition();
			bot.ProcessEvent(new BotEvent(EBotEventType.ENEMY_DETECTED, detected, detected.GetPosition()));
			return;
		}

		if (bot.m_SearchTimer >= SEARCH_TIMEOUT)
		{
			bot.m_IsMovingToBuilding = false;
			bot.ProcessEvent(new BotEvent(EBotEventType.TIMEOUT));
		}
	}

	static EBotState CheckTransitions(BotUnit bot, BotEvent ev)
	{
		if (ev.m_Type == EBotEventType.ENEMY_DETECTED)
			return EBotState.ALERT;
		if (ev.m_Type == EBotEventType.TIMEOUT)
			return EBotState.PATROL;
		if (ev.m_Type == EBotEventType.STOP)
			return EBotState.IDLE;
		return EBotState.SEARCH;
	}
};