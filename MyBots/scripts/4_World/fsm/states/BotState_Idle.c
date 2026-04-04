class BotState_Idle
{
	static void Enter(BotUnit bot)
	{
		bot.m_CurrentState = EBotState.IDLE;
		bot.StopMovement();
		bot.m_AimController.ClearOverrides();
		Print("[MyBots] Бот перешёл в IDLE");
	}

	static void Update(BotUnit bot, float dt)
	{
	}

	static EBotState CheckTransitions(BotUnit bot, BotEvent ev)
	{
		if (ev.m_Type == EBotEventType.START)
			return EBotState.PATROL;
		return EBotState.IDLE;
	}
};
