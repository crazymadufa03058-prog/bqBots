class BotManager
{
	static ref BotManager s_Instance;

	ref BotSettings m_Settings;
	ref array<ref BotLoadoutConfig> m_Loadouts;
	ref array<ref BotZoneConfig> m_Zones;
	ref array<ref BotUnit> m_AllBots;
	ref array<ref BotUnit> m_ActiveBots;
	ref array<ref BotSquad> m_Squads;

	int m_TickCounter;
	int m_BotsPerTick;
	float m_LastUpdateTime;
	ref map<string, float> m_ZoneRespawnTimers;

	void BotManager()
	{
		m_AllBots = new array<ref BotUnit>();
		m_ActiveBots = new array<ref BotUnit>();
		m_Squads = new array<ref BotSquad>();
		m_TickCounter = 0;
		m_BotsPerTick = 5;
		m_LastUpdateTime = 0;
		m_ZoneRespawnTimers = new map<string, float>();

		Print("[MyBots] BotManager создан");
	}

	static BotManager Get()
	{
		if (!s_Instance)
			s_Instance = new BotManager();
		return s_Instance;
	}

	void Init()
	{
		m_Settings = BotConfigLoader.LoadSettings();
		m_Loadouts = BotConfigLoader.LoadLoadouts();
		m_Zones = BotConfigLoader.LoadZones();

		m_BotsPerTick = m_Settings.m_TickBudget;

		Print("[MyBots] Инициализация: " + m_Zones.Count() + " зон, " + m_Loadouts.Count() + " наборов");

		SpawnAllZones();
	}

	void SpawnAllZones()
	{
		for (int i = 0; i < m_Zones.Count(); i++)
		{
			SpawnZone(m_Zones[i]);
		}
	}

	void SpawnZone(BotZoneConfig zone)
	{
		int count = Math.RandomInt(zone.m_BotCountMin, zone.m_BotCountMax + 1);
		count = Math.Min(count, m_Settings.m_MaxBotsPerZone);

		Print("[MyBots] Зона '" + zone.m_Name + "': спавн " + count + " ботов");

		ref BotSquad squad = new BotSquad(zone.m_Name);
		m_Squads.Insert(squad);

		for (int i = 0; i < count; i++)
		{
			if (m_AllBots.Count() >= m_Settings.m_MaxBots)
			{
				Print("[MyBots] Достигнут лимит ботов: " + m_Settings.m_MaxBots);
				return;
			}

			BotLoadoutConfig loadout = BotConfigLoader.GetRandomLoadout(m_Loadouts, zone.m_Loadouts);
			vector spawnPos = BotSpawner.GetSpawnPosition(zone);

			PlayerBase player = BotSpawner.SpawnBot(spawnPos, loadout);
			if (!player)
				continue;

			BotUnit bot = new BotUnit(player, zone, m_Settings, loadout);
			squad.AddMember(bot);
			m_AllBots.Insert(bot);
			m_ActiveBots.Insert(bot);
			bot.Start();
		}

		Print("[MyBots] Отряд '" + zone.m_Name + "' создан: " + squad.m_Members.Count() + " ботов");
	}

	BotSquad FindSquadForBot(BotUnit bot)
	{
		for (int i = 0; i < m_Squads.Count(); i++)
		{
			if (m_Squads[i].m_Members.Find(bot) >= 0)
				return m_Squads[i];
		}
		return NULL;
	}

	// Быстрый поиск бота по PlayerBase — вызывается из CommandHandler каждый тик
	// Используем простой перебор — список ботов обычно небольшой
	BotUnit FindBotByPlayer(PlayerBase player)
	{
		if (!player)
			return NULL;

		for (int i = 0; i < m_ActiveBots.Count(); i++)
		{
			if (m_ActiveBots[i].m_Player == player)
				return m_ActiveBots[i];
		}
		return NULL;
	}

	// Вызывается из EEDelete/EEKilled PlayerBase — очищает dangling pointer
	// у всех ботов ПЕРЕД тем как движок удалит объект игрока из памяти.
	void ClearEnemyRef(PlayerBase player)
	{
		if (!player)
			return;
		for (int i = 0; i < m_AllBots.Count(); i++)
		{
			BotUnit bot = m_AllBots[i];
			if (bot && bot.m_CurrentEnemy == player)
			{
				Print("[MyBots][MGR] ClearEnemyRef: сброс m_CurrentEnemy у бота " + i);
				bot.m_CurrentEnemy = null;
				if (bot.m_Player)
					bot.m_LastKnownEnemyPos = bot.m_Player.GetPosition();
				else
					bot.m_LastKnownEnemyPos = "0 0 0";
			}
		}
	}

	void OnUpdate(float dt)
	{
		float now = GetGame().GetTickTime();

		if (m_ActiveBots.Count() == 0)
			return;

		int startIdx = m_TickCounter % m_ActiveBots.Count();
		int updates = Math.Min(m_BotsPerTick, m_ActiveBots.Count());

		for (int i = 0; i < updates; i++)
		{
			int idx = (startIdx + i) % m_ActiveBots.Count();
			BotUnit bot = m_ActiveBots[idx];

			if (!bot.IsAlive())
			{
				RemoveBot(bot);
				i--;
				updates--;
				continue;
			}

			float dist = GetNearestPlayerDistance(bot.m_Player);
			bot.m_DistanceToNearestPlayer = dist;

			if (dist > m_Settings.m_DespawnDistance)
			{
				bot.StopMovement();
				continue;
			}

			if (dist > 300)
			{
				float slowDt = dt * 4;
				bot.OnUpdate(slowDt);
			}
			else
			{
				bot.OnUpdate(dt);
			}
		}

		m_TickCounter++;

		CleanupEmptySquads();
		CheckRespawns(now);

		m_LastUpdateTime = now;
	}

	float GetNearestPlayerDistance(PlayerBase botPlayer)
	{
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		vector botPos = botPlayer.GetPosition();
		float minDist = 999999;

		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase p = PlayerBase.Cast(players[i]);
			if (!p || p == botPlayer)
				continue;

			float dist = vector.Distance(botPos, p.GetPosition());
			if (dist < minDist)
				minDist = dist;
		}

		return minDist;
	}

	void RemoveBot(BotUnit bot)
	{
		bot.m_IsActive = false;

		// Очищаем ссылки на этого бота у других ботов (предотвращение dangling pointer)
		PlayerBase botPlayer = bot.m_Player;
		if (botPlayer)
		{
			for (int ci = 0; ci < m_AllBots.Count(); ci++)
			{
				BotUnit other = m_AllBots[ci];
				if (other && other != bot && other.m_CurrentEnemy == botPlayer)
				{
					other.m_CurrentEnemy = null;
					if (other.m_Player)
						other.m_LastKnownEnemyPos = other.m_Player.GetPosition();
				}
			}
		}

		if (bot.m_Squad)
			bot.m_Squad.RemoveMember(bot);

		int idx = m_ActiveBots.Find(bot);
		if (idx >= 0)
			m_ActiveBots.Remove(idx);

		int allIdx = m_AllBots.Find(bot);
		if (allIdx >= 0)
			m_AllBots.Remove(allIdx);

		BotSpawner.DespawnBot(bot.m_Player);

		Print("[MyBots] Бот удалён. Осталось: " + m_ActiveBots.Count());
	}

	void CleanupEmptySquads()
	{
		for (int i = m_Squads.Count() - 1; i >= 0; i--)
		{
			if (m_Squads[i].GetAliveCount() == 0)
			{
				Print("[MyBots] Отряд '" + m_Squads[i].m_ZoneName + "' уничтожен");
				m_Squads.Remove(i);
			}
		}
	}

	void CheckRespawns(float now)
	{
		for (int i = 0; i < m_Zones.Count(); i++)
		{
			BotZoneConfig zone = m_Zones[i];
			int zoneBots = CountBotsInZone(zone);

			if (zoneBots < zone.m_BotCountMin)
			{
				float lastSpawn = 0;
				if (m_ZoneRespawnTimers.Contains(zone.m_Name))
					lastSpawn = m_ZoneRespawnTimers.Get(zone.m_Name);

				if (now - lastSpawn > zone.m_RespawnTime)
				{
					BotLoadoutConfig loadout = BotConfigLoader.GetRandomLoadout(m_Loadouts, zone.m_Loadouts);
					vector spawnPos = BotSpawner.GetSpawnPosition(zone);

					PlayerBase player = BotSpawner.SpawnBot(spawnPos, loadout);
					if (player)
					{
						BotUnit bot = new BotUnit(player, zone, m_Settings, loadout);

						BotSquad squad = FindOrCreateSquadForZone(zone.m_Name);
						squad.AddMember(bot);

						m_AllBots.Insert(bot);
						m_ActiveBots.Insert(bot);
						bot.Start();

						m_ZoneRespawnTimers.Set(zone.m_Name, now);
						Print("[MyBots] Респавн в зоне '" + zone.m_Name + "'");
					}
				}
			}
		}
	}

	BotSquad FindOrCreateSquadForZone(string zoneName)
	{
		for (int i = 0; i < m_Squads.Count(); i++)
		{
			if (m_Squads[i].m_ZoneName == zoneName && m_Squads[i].GetAliveCount() > 0)
				return m_Squads[i];
		}

		ref BotSquad newSquad = new BotSquad(zoneName);
		m_Squads.Insert(newSquad);
		return newSquad;
	}

	int CountBotsInZone(BotZoneConfig zone)
	{
		int count = 0;
		for (int i = 0; i < m_ActiveBots.Count(); i++)
		{
			if (m_ActiveBots[i].m_ZoneConfig.m_Name == zone.m_Name)
				count++;
		}
		return count;
	}

	void ReloadConfigs()
	{
		m_Settings = BotConfigLoader.LoadSettings();
		m_Loadouts = BotConfigLoader.LoadLoadouts();
		m_Zones = BotConfigLoader.LoadZones();
		m_BotsPerTick = m_Settings.m_TickBudget;
		Print("[MyBots] Конфиги перезагружены");
	}

	void DespawnAll()
	{
		for (int i = m_AllBots.Count() - 1; i >= 0; i--)
		{
			BotSpawner.DespawnBot(m_AllBots[i].m_Player);
		}
		m_AllBots.Clear();
		m_ActiveBots.Clear();
		m_Squads.Clear();
		Print("[MyBots] Все боты деспавнены");
	}

	string GetStatus()
	{
		string status = "[MyBots] Статус:\n";
		status += "  Всего ботов: " + m_ActiveBots.Count() + "/" + m_Settings.m_MaxBots + "\n";
		status += "  Отрядов: " + m_Squads.Count() + "\n";
		status += "  Зон: " + m_Zones.Count() + "\n";
		status += "  Наборов: " + m_Loadouts.Count() + "\n";

		for (int i = 0; i < m_Squads.Count(); i++)
		{
			BotSquad sq = m_Squads[i];
			status += "  Отряд '" + sq.m_ZoneName + "': " + sq.GetAliveCount() + " ботов";
			if (sq.m_InCombat)
				status += " [В БОЮ]";
			status += "\n";
		}

		return status;
	}
};