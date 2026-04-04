class BotSensor_Hearing
{
	protected PlayerBase m_Bot;
	protected float m_LastCheckTime;
	static const float CHECK_INTERVAL = 3.0;

	void BotSensor_Hearing(PlayerBase bot)
	{
		m_Bot = bot;
		m_LastCheckTime = 0;
	}

	vector DetectSound(BotDetectionConfig cfg, out float outPriority)
	{
		float now = GetGame().GetTickTime();
		if (now - m_LastCheckTime < CHECK_INTERVAL)
			return "0 0 0";
		m_LastCheckTime = now;

		vector botPos = m_Bot.GetPosition();
		float hearRange = cfg.m_HearingRange;

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		vector bestSoundPos = "0 0 0";
		float bestPriority = 0;

		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase player = PlayerBase.Cast(players[i]);
			if (!player || player == m_Bot)
				continue;
			if (player.IsDamageDestroyed())
				continue;

			vector playerPos = player.GetPosition();
			float dist = vector.Distance(botPos, playerPos);

			float priority = 0;

			Weapon_Base weapon = Weapon_Base.Cast(player.GetItemInHands());
			if (weapon)
			{
				float shootRange = 200;
				if (dist < shootRange)
				{
					priority = (1.0 - dist / shootRange) * 10;
				}
			}

			float sprintRange = 30;
			if (dist < sprintRange && player.IsSprinting())
			{
				float sprintPriority = (1.0 - dist / sprintRange) * 5;
				if (sprintPriority > priority)
					priority = sprintPriority;
			}

			if (priority > bestPriority)
			{
				bestPriority = priority;
				bestSoundPos = playerPos;
			}
		}

		outPriority = bestPriority;
		return bestSoundPos;
	}
};
