class BotConfigLoader
{
	static const string PROFILE_DIR = "$profile:MyBots/";
	static const string SETTINGS_FILE = PROFILE_DIR + "settings.json";
	static const string LOADOUTS_FILE = PROFILE_DIR + "loadouts.json";
	static const string ZONES_FILE = PROFILE_DIR + "zones.json";

	static ref BotSettings LoadSettings()
	{
		ref BotSettings settings = new BotSettings();

		// Создаём папку если не существует
		if (!FileExist(PROFILE_DIR))
		{
			MakeDirectory(PROFILE_DIR);
			Print("[MyBots] Создана папка: " + PROFILE_DIR);
		}

		if (!FileExist(SETTINGS_FILE))
		{
			BotConfigLoader.SaveSettings(settings);
			Print("[MyBots] settings.json не найден, создан по умолчанию");
			return settings;
		}

		JsonFileLoader<BotSettings>.JsonLoadFile(SETTINGS_FILE, settings);
		Print("[MyBots] settings.json загружен");
		return settings;
	}

	static void SaveSettings(BotSettings settings)
	{
		JsonFileLoader<BotSettings>.JsonSaveFile(SETTINGS_FILE, settings);
	}

	static ref array<ref BotLoadoutConfig> LoadLoadouts()
	{
		ref array<ref BotLoadoutConfig> loadouts = new array<ref BotLoadoutConfig>();

		if (!FileExist(LOADOUTS_FILE))
		{
			BotConfigLoader.CreateDefaultLoadouts(loadouts);
			BotConfigLoader.SaveLoadouts(loadouts);
			Print("[MyBots] loadouts.json не найден, создан по умолчанию");
			return loadouts;
		}

		JsonFileLoader<array<ref BotLoadoutConfig>>.JsonLoadFile(LOADOUTS_FILE, loadouts);
		Print("[MyBots] loadouts.json загружен, наборов: " + loadouts.Count());
		return loadouts;
	}

	static void SaveLoadouts(array<ref BotLoadoutConfig> loadouts)
	{
		JsonFileLoader<array<ref BotLoadoutConfig>>.JsonSaveFile(LOADOUTS_FILE, loadouts);
	}

	static ref array<ref BotZoneConfig> LoadZones()
	{
		ref array<ref BotZoneConfig> zones = new array<ref BotZoneConfig>();

		if (!FileExist(ZONES_FILE))
		{
			BotConfigLoader.CreateDefaultZones(zones);
			BotConfigLoader.SaveZones(zones);
			Print("[MyBots] zones.json не найден, создан по умолчанию");
			return zones;
		}

		JsonFileLoader<array<ref BotZoneConfig>>.JsonLoadFile(ZONES_FILE, zones);
		Print("[MyBots] zones.json загружен, зон: " + zones.Count());
		return zones;
	}

	static void SaveZones(array<ref BotZoneConfig> zones)
	{
		JsonFileLoader<array<ref BotZoneConfig>>.JsonSaveFile(ZONES_FILE, zones);
	}

	static BotLoadoutConfig FindLoadout(array<ref BotLoadoutConfig> loadouts, string name)
	{
		for (int i = 0; i < loadouts.Count(); i++)
		{
			if (loadouts[i].m_Name == name)
				return loadouts[i];
		}
		return NULL;
	}

	static BotLoadoutConfig GetRandomLoadout(array<ref BotLoadoutConfig> loadouts, array<string> allowedNames)
	{
		if (allowedNames.Count() == 0 || loadouts.Count() == 0)
			return NULL;

		ref array<BotLoadoutConfig> candidates = new array<BotLoadoutConfig>();
		int totalWeight = 0;

		for (int i = 0; i < allowedNames.Count(); i++)
		{
			BotLoadoutConfig cfg = FindLoadout(loadouts, allowedNames[i]);
			if (cfg)
			{
				candidates.Insert(cfg);
				totalWeight += cfg.m_Weight;
			}
		}

		if (candidates.Count() == 0)
			return NULL;

		int roll = Math.RandomInt(0, totalWeight);
		int cumulative = 0;
		for (int j = 0; j < candidates.Count(); j++)
		{
			cumulative += candidates[j].m_Weight;
			if (roll < cumulative)
				return candidates[j];
		}

		return candidates[candidates.Count() - 1];
	}

	private static void CreateDefaultLoadouts(array<ref BotLoadoutConfig> loadouts)
	{
		ref BotLoadoutConfig mil = new BotLoadoutConfig();
		mil.m_Name = "military_basic";
		mil.m_Weight = 40;
		ref BotWeaponConfig mw = new BotWeaponConfig();
		mw.m_Type = "M4A1";
		mw.m_Magazine = "Mag_STANAG_30Rnd";
		mw.m_AmmoCount = 3;
		mw.m_Attachments.Insert("M4_RISHndgrd");
		mil.m_Weapons.Insert(mw);
		mil.m_Clothing.m_Head.Insert("MilitaryBeret_Red");
		mil.m_Clothing.m_Top.Insert("M65Jacket_Olive");
		mil.m_Clothing.m_Pants.Insert("CargoPants_Green");
		mil.m_Clothing.m_Feet.Insert("MilitaryBoots_Black");
		mil.m_Clothing.m_Vest.Insert("PlateCarrierVest");
		mil.m_Clothing.m_Backpack.Insert("AssaultBag_Green");
		loadouts.Insert(mil);

		ref BotLoadoutConfig bandit = new BotLoadoutConfig();
		bandit.m_Name = "bandit_light";
		bandit.m_Weight = 30;
		ref BotWeaponConfig bw = new BotWeaponConfig();
		bw.m_Type = "SKS";
		bw.m_Magazine = "Ammo_762x39";
		bw.m_AmmoCount = 40;
		bandit.m_Weapons.Insert(bw);
		bandit.m_Clothing.m_Head.Insert("BaseballCap_Black");
		bandit.m_Clothing.m_Top.Insert("BomberJacket_Black");
		bandit.m_Clothing.m_Pants.Insert("Jeans_Black");
		bandit.m_Clothing.m_Feet.Insert("HikingBootsLow_Black");
		bandit.m_Clothing.m_Backpack.Insert("DryBag_Black");
		loadouts.Insert(bandit);
	}

	private static void CreateDefaultZones(array<ref BotZoneConfig> zones)
	{
		ref BotZoneConfig nwaf = new BotZoneConfig();
		nwaf.m_Name = "NWAF_Patrol";
		nwaf.m_Center = "4500 0 10200";
		nwaf.m_Radius = 300;
		nwaf.m_BotCountMin = 3;
		nwaf.m_BotCountMax = 6;
		nwaf.m_Loadouts.Insert("military_basic");
		nwaf.m_Behaviour = "patrol";
		nwaf.m_Waypoints.Insert("4500 0 10200");
		nwaf.m_Waypoints.Insert("4600 0 10300");
		nwaf.m_Waypoints.Insert("4550 0 10100");
		nwaf.m_RespawnTime = 600;
		zones.Insert(nwaf);

		ref BotZoneConfig stary = new BotZoneConfig();
		stary.m_Name = "Stary_Bandits";
		stary.m_Center = "6100 0 7700";
		stary.m_Radius = 150;
		stary.m_BotCountMin = 2;
		stary.m_BotCountMax = 4;
		stary.m_Loadouts.Insert("bandit_light");
		stary.m_Behaviour = "guard";
		stary.m_RespawnTime = 900;
		zones.Insert(stary);
	}
};