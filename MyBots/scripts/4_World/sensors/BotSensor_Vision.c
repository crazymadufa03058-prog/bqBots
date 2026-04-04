class BotSensor_Vision
{
	protected PlayerBase m_Bot;
	protected float m_LastCheckTime;
	protected EntityAI m_LastDetected;
	static const float CHECK_INTERVAL = 0.3;

	void BotSensor_Vision(PlayerBase bot)
	{
		m_Bot = bot;
		m_LastCheckTime = 0;
		m_LastDetected = NULL;
	}

	void ResetCache()
	{
		m_LastCheckTime = 0;
		m_LastDetected = NULL;
	}

	EntityAI GetLastDetected()
	{
		return m_LastDetected;
	}

	EntityAI ScanForEnemies(BotDetectionConfig cfg)
	{
		float now = GetGame().GetTickTime();
		if (now - m_LastCheckTime < CHECK_INTERVAL)
			return m_LastDetected;

		m_LastCheckTime = now;
		m_LastDetected = NULL;

		vector botPos = m_Bot.GetPosition();
		float visionRange = cfg.m_VisionRange;

		if (GetGame().GetWorld().IsNight())
			visionRange *= cfg.m_NightVisionMultiplier;

		float botYaw = m_Bot.GetOrientation()[0] * Math.DEG2RAD;
		vector botDir = Vector(Math.Sin(botYaw), 0, Math.Cos(botYaw));
		float halfAngle = cfg.m_VisionAngle / 2.0;
		vector eyePos = botPos + "0 1.7 0";

		float closestDist = visionRange + 1;

		// Игроки
		array<Man> players = new array<Man>();
		GetGame().GetWorld().GetPlayerList(players);
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase player = PlayerBase.Cast(players[i]);
			if (!player || player == m_Bot || !player.GetIdentity())
				continue;
			if (player.IsDamageDestroyed() || player.IsUnconscious())
				continue;

			vector playerPos = player.GetPosition();
			float dist = vector.Distance(botPos, playerPos);
			if (dist > visionRange)
				continue;

			if (!CheckAngle(botDir, botPos, playerPos, halfAngle))
				continue;

			if (!CheckLineOfSight(eyePos, playerPos + "0 1.0 0"))
				continue;

			if (dist < closestDist)
			{
				closestDist = dist;
				m_LastDetected = EntityAI.Cast(player);
			}
		}

		// Зомби и животные — ищем ближайших в радиусе
		array<Object> nearObjects = new array<Object>();
		GetGame().GetObjectsAtPosition(botPos, visionRange, nearObjects, NULL);
		for (int j = 0; j < nearObjects.Count(); j++)
		{
			EntityAI entity = EntityAI.Cast(nearObjects[j]);
			if (!entity || entity == m_Bot)
				continue;

			// Только зомби и животные
			if (!entity.IsInherited(DayZInfected) && !entity.IsInherited(AnimalBase))
				continue;
			if (entity.IsDamageDestroyed())
				continue;

			vector entityPos = entity.GetPosition();
			float dist2 = vector.Distance(botPos, entityPos);
			if (dist2 > visionRange)
				continue;

			if (!CheckAngle(botDir, botPos, entityPos, halfAngle))
				continue;

			if (!CheckLineOfSight(eyePos, entityPos + "0 1.0 0"))
				continue;

			if (dist2 < closestDist)
			{
				closestDist = dist2;
				m_LastDetected = entity;
			}
		}

		return m_LastDetected;
	}

	EntityAI ScanForEnemiesNearPos(vector pos, float radius)
	{
		// Игроки
		array<Man> players = new array<Man>();
		GetGame().GetWorld().GetPlayerList(players);
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase player = PlayerBase.Cast(players[i]);
			if (!player || player == m_Bot || !player.GetIdentity())
				continue;
			if (player.IsDamageDestroyed() || player.IsUnconscious())
				continue;
			if (vector.Distance(player.GetPosition(), pos) < radius)
				return EntityAI.Cast(player);
		}

		// Зомби
		array<Object> nearObjects = new array<Object>();
		GetGame().GetObjectsAtPosition(pos, radius, nearObjects, NULL);
		for (int j = 0; j < nearObjects.Count(); j++)
		{
			EntityAI entity = EntityAI.Cast(nearObjects[j]);
			if (!entity || entity == m_Bot)
				continue;
			if (!entity.IsInherited(DayZInfected) && !entity.IsInherited(AnimalBase))
				continue;
			if (entity.IsDamageDestroyed())
				continue;
			return entity;
		}

		return NULL;
	}

	protected bool CheckAngle(vector botDir, vector botPos, vector targetPos, float halfAngle)
	{
		vector toTarget = targetPos - botPos;
		toTarget[1] = 0;
		toTarget = toTarget.Normalized();
		float dot = vector.Dot(botDir, toTarget);
		float angle = Math.Acos(Math.Clamp(dot, -1, 1)) * Math.RAD2DEG;
		return angle <= halfAngle;
	}

	protected bool CheckLineOfSight(vector from, vector to)
	{
		vector contactPos, contactDir;
		int contactComponent;
		return !DayZPhysics.RaycastRV(from, to, contactPos, contactDir, contactComponent, NULL, NULL, m_Bot);
	}
};
