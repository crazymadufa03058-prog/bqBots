modded class MissionServer
{
	void MissionServer()
	{
	}

	override void OnInit()
	{
		super.OnInit();
		Print("[MyBots] MissionServer.OnInit — запуск BotManager");

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DelayedBotInit, 5000, false);
	}

	void DelayedBotInit()
	{
		BotManager.Get().Init();
		Print("[MyBots] BotManager инициализирован");
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		BotManager bm = BotManager.Get();
		if (bm && bm.m_ActiveBots && bm.m_ActiveBots.Count() > 0)
		{
			bm.OnUpdate(timeslice);
		}
	}
};
