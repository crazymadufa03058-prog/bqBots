class BotState_Alert
{
	static const float ALERT_TIMEOUT = 10.0;

	static void Enter(BotUnit bot)
	{
		bot.m_CurrentState = EBotState.ALERT;
		bot.m_AlertTimer = 0;
		bot.StopMovement();

		bot.m_Player.Bot_StartCombatCommand();

		bot.m_Vision.ResetCache();
		Print("[MyBots] Бот перешёл в ALERT");
	}

	static void Update(BotUnit bot, float dt)
	{
		bot.m_AlertTimer += dt;

		EntityAI detected = bot.m_Vision.ScanForEnemies(bot.m_DetectionCfg);

		if (!detected && bot.m_LastKnownEnemyPos != "0 0 0")
			detected = bot.m_Vision.ScanForEnemiesNearPos(bot.m_LastKnownEnemyPos, 50.0);

		if (detected)
		{
			bot.m_CurrentEnemy = detected;
			bot.m_LastKnownEnemyPos = detected.GetPosition();
			bot.ProcessEvent(new BotEvent(EBotEventType.ENEMY_CONFIRMED, detected, detected.GetPosition()));
			return;
		}

		if (bot.m_LastKnownEnemyPos != "0 0 0")
			bot.m_AimController.UpdateAim(bot.m_LastKnownEnemyPos, 0.5, dt);

		if (bot.m_AlertTimer > ALERT_TIMEOUT)
			bot.ProcessEvent(new BotEvent(EBotEventType.TIMEOUT));
	}

	static EBotState CheckTransitions(BotUnit bot, BotEvent ev)
	{
		if (ev.m_Type == EBotEventType.ENEMY_CONFIRMED)
			return EBotState.COMBAT;
		if (ev.m_Type == EBotEventType.TIMEOUT)
			return EBotState.SEARCH;
		if (ev.m_Type == EBotEventType.STOP)
			return EBotState.IDLE;
		return EBotState.ALERT;
	}
};
