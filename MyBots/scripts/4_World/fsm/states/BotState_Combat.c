class BotState_Combat
{
	static const float RETREAT_HP_THRESHOLD = 30.0;

	static void Enter(BotUnit bot)
	{
		bot.m_CurrentState = EBotState.COMBAT;
		bot.m_CombatTimer = 0;
		bot.m_IsRetreating = false;
		bot.m_IsMovingToBuilding = false;

		bot.m_Player.Bot_StartCombatCommand();

		bot.StopMovement();

		if (bot.m_Squad && bot.m_CurrentEnemy)
			bot.m_Squad.OnEngage(bot.m_CurrentEnemy);

		Print("[MyBots] Бот перешёл в COMBAT");
	}

	static void Update(BotUnit bot, float dt)
	{
		bot.m_CombatTimer += dt;

		if (!bot.m_CurrentEnemy || bot.m_CurrentEnemy.IsDamageDestroyed() || !bot.m_CurrentEnemy.IsAlive())
		{
			if (bot.m_Squad)
				bot.m_Squad.OnDisengage();
			bot.ProcessEvent(new BotEvent(EBotEventType.ENEMY_KILLED));
			return;
		}

		vector botPos = bot.m_Player.GetPosition();
		vector enemyPos = bot.m_CurrentEnemy.GetPosition();
		bot.m_LastKnownEnemyPos = enemyPos;
		float dist = vector.Distance(botPos, enemyPos);

		if (bot.m_Squad)
			bot.m_Squad.UpdateRoles(GetGame().GetTickTime());

		bot.StopMovement();
		bot.m_AimController.UpdateAim(enemyPos, 0.8, dt);

		float accuracy = BotWeaponHandler.CalculateAccuracy(bot.m_AccuracyCfg, dist, false, false, false);
		bot.m_WeaponHandler.TryFire(enemyPos, accuracy, bot.m_DamageCfg, dt);

		EntityAI detected = bot.m_Vision.ScanForEnemies(bot.m_DetectionCfg);
		if (!detected)
		{
			float lostDist = vector.Distance(botPos, enemyPos);
			if (lostDist > bot.m_DetectionCfg.m_VisionRange * 1.5)
			{
				bot.m_AimController.ClearOverrides();
				bot.ProcessEvent(new BotEvent(EBotEventType.ENEMY_LOST));
			}
		}
	}

	static EBotState CheckTransitions(BotUnit bot, BotEvent ev)
	{
		if (ev.m_Type == EBotEventType.ENEMY_KILLED)
		{
			if (bot.m_Squad)
				bot.m_Squad.OnDisengage();
			return EBotState.SEARCH;
		}
		if (ev.m_Type == EBotEventType.ENEMY_LOST)
		{
			if (bot.m_Squad)
				bot.m_Squad.OnDisengage();
			return EBotState.SEARCH;
		}
		if (ev.m_Type == EBotEventType.STOP)
			return EBotState.IDLE;
		return EBotState.COMBAT;
	}
};
