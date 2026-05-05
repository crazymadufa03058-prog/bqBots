class BQBotAccuracyOverrideConfig
{
	float m_Base = -1.0;
	float m_NearDistance = -1.0;
	float m_FarDistance = -1.0;
	float m_NearFactor = -1.0;
	float m_FarFactor = -1.0;
	float m_MaxSpreadDeg = -1.0;
}

class BQBotSpawnZoneConfig
{
	string m_Id = "zone_001";
	string m_LoadoutId = "default";
	string m_Classname = "bqBots_Boris_AI";
	string m_Position = "6108.52 0 7634.82";
	string m_Orientation = "0 0 0";
	string m_GroupId = "";
	int m_GroupSlot = -1;
	string m_Formation = "vee"; // vee | invvee | column | invcolumn | file | invfile | wall | line | loose | circle | circledot | star | stardot
	float m_FormationScale = 1.0;
	float m_FormationLooseness = 0.6;
	int m_GroupPatrolEnabled = 1;
	string m_GroupWaypointBehaviour = "loop"; // loop | alternate | halt | roaming | roaming_local
	float m_GroupRegroupDistanceM = 8.0;
	float m_GroupBreakFormationDistanceM = 18.0;
	ref BQBotAccuracyOverrideConfig m_Accuracy;
}

class BQBotLoadoutConfig
{
	string m_Id = "default";
	int m_AllowDamage = 1;
	float m_DamageMultiplier = 1.0;
	ref BQBotWeaponConfig m_Weapon;
	ref BQBotClothingConfig m_Clothing;
	ref BQBotRuntimeConfig m_Runtime;

	void BQBotLoadoutConfig()
	{
		m_Weapon = new BQBotWeaponConfig();
		m_Clothing = new BQBotClothingConfig();
		m_Runtime = new BQBotRuntimeConfig();
	}
}

modded class MissionServer
{
	protected ref array<ref BQBotShooterManager> m_BQBotShooterManagers;
	protected const string BQ_BOTS_DIR = "$profile:bqBots\\";
	protected const string BQ_BOTS_ZONES_PATH = "$profile:bqBots\\zones.json";
	protected const string BQ_BOTS_LOADOUTS_PATH = "$profile:bqBots\\loadouts.json";
	protected const string BQ_BOTS_LEGACY_PATH = "$profile:bqBots\\bots.json";

	override void OnInit()
	{
		super.OnInit();

		if (GetGame().IsServer())
		{
			BQBot_EnsureExpansionWorldScheduler();
			BQBotShooterManager.ResetBotInitStagger();
			SpawnLoadTestBots();
		}
	}

	protected void BQBot_EnsureExpansionWorldScheduler()
	{
		DayZGame game = GetDayZGame();
		if (!game)
			return;

		ExpansionWorld world;
		if (Class.CastTo(world, game.GetExpansionGame()))
			return;

		game.SetExpansionGame(new ExpansionWorld());
	}
	protected void SpawnLoadTestBots()
	{
		m_BQBotShooterManagers = new array<ref BQBotShooterManager>();
		ref array<ref BQBotSettings> bots = LoadBotsList();
		int botCount = 0;
		if (bots)
			botCount = bots.Count();
		if (!bots || bots.Count() == 0)
		{
			return;
		}

		foreach (BQBotSettings botSettings : bots)
		{
			if (!botSettings)
				continue;

			if (!botSettings.m_Spawn)
				botSettings.m_Spawn = new BQBotSpawnConfig();
			if (botSettings.m_Spawn.m_Classname == "")
				botSettings.m_Spawn.m_Classname = "bqBots_Boris_AI";

			if (!botSettings.m_Weapon)
				botSettings.m_Weapon = new BQBotWeaponConfig();
			if (botSettings.m_Weapon.m_Weapon == "")
				botSettings.m_Weapon.m_Weapon = "AKM";
			if (botSettings.m_Weapon.m_Magazine == "")
				botSettings.m_Weapon.m_Magazine = "Mag_AKM_30Rnd";
			if (botSettings.m_Weapon.m_MagAmmo <= 0)
				botSettings.m_Weapon.m_MagAmmo = 30;
			if (botSettings.m_Weapon.m_ReserveMagAmmo <= 0)
				botSettings.m_Weapon.m_ReserveMagAmmo = 30;

			if (!botSettings.m_Clothing)
				botSettings.m_Clothing = new BQBotClothingConfig();
			if (!botSettings.m_Runtime)
				botSettings.m_Runtime = new BQBotRuntimeConfig();

			BQBotShooterManager.QueueSettingsOverride(botSettings);
			m_BQBotShooterManagers.Insert(new BQBotShooterManager());
		}
	}

	protected ref array<ref BQBotSettings> LoadBotsList()
	{
		EnsureBotsDirectory();

		ref array<ref BQBotSettings> bots = new array<ref BQBotSettings>();
		ref array<ref BQBotSpawnZoneConfig> zones = new array<ref BQBotSpawnZoneConfig>();
		ref array<ref BQBotLoadoutConfig> loadouts = new array<ref BQBotLoadoutConfig>();
		if (!FileExist(BQ_BOTS_ZONES_PATH) || !FileExist(BQ_BOTS_LOADOUTS_PATH))
		{
			string saveErr;
			if (FileExist(BQ_BOTS_LEGACY_PATH))
			{
				MigrateLegacyBotsConfig(zones, loadouts);
			}
			else
			{
				ref BQBotSpawnZoneConfig zone = new BQBotSpawnZoneConfig();
				zone.m_Id = "zone_001";
				zone.m_LoadoutId = "default";
				zone.m_Classname = "bqBots_Boris_AI";
				zone.m_Position = "6108.52 0 7634.82";
				zone.m_Orientation = "0 0 0";
				zones.Insert(zone);

				ref BQBotLoadoutConfig loadout = new BQBotLoadoutConfig();
				loadout.m_Id = "default";
				loadout.m_AllowDamage = 1;
				loadout.m_DamageMultiplier = 1.0;
				loadout.m_Weapon.m_Weapon = "AKM";
				loadout.m_Weapon.m_Magazine = "Mag_AKM_30Rnd";
				loadout.m_Weapon.m_MagAmmo = 30;
				loadout.m_Weapon.m_ReserveMagAmmo = 30;
				loadout.m_Weapon.m_FireMode = "auto";
				loadout.m_Clothing.m_Vest = "PressVest_Blue";
				loadout.m_Clothing.m_Backpack = "TaloonBag_Blue";
				loadouts.Insert(loadout);
			}

			JsonFileLoader<ref array<ref BQBotSpawnZoneConfig>>.SaveFile(BQ_BOTS_ZONES_PATH, zones, saveErr);
			JsonFileLoader<ref array<ref BQBotLoadoutConfig>>.SaveFile(BQ_BOTS_LOADOUTS_PATH, loadouts, saveErr);
		}

		string loadErr;
		if (!JsonFileLoader<ref array<ref BQBotSpawnZoneConfig>>.LoadFile(BQ_BOTS_ZONES_PATH, zones, loadErr) || !zones)
		{
			return bots;
		}

		if (!JsonFileLoader<ref array<ref BQBotLoadoutConfig>>.LoadFile(BQ_BOTS_LOADOUTS_PATH, loadouts, loadErr) || !loadouts)
		{
			return bots;
		}

		foreach (BQBotSpawnZoneConfig zoneCfg : zones)
		{
			if (!zoneCfg)
				continue;

			ref BQBotLoadoutConfig loadoutCfg = FindLoadoutById(loadouts, zoneCfg.m_LoadoutId);
			if (!loadoutCfg && loadouts.Count() > 0)
				loadoutCfg = loadouts.Get(0);

			ref BQBotSettings bot = BuildBotSettingsFromZoneAndLoadout(zoneCfg, loadoutCfg);
			if (bot)
				bots.Insert(bot);
		}

		return bots;
	}

	protected ref BQBotLoadoutConfig FindLoadoutById(array<ref BQBotLoadoutConfig> loadouts, string id)
	{
		if (!loadouts || loadouts.Count() == 0)
			return null;

		if (id != "")
		{
			foreach (BQBotLoadoutConfig loadout : loadouts)
			{
				if (loadout && loadout.m_Id == id)
					return loadout;
			}
		}

		foreach (BQBotLoadoutConfig fallback : loadouts)
		{
			if (fallback && fallback.m_Id == "default")
				return fallback;
		}

		return null;
	}

	protected bool ParseGroupFromCompositeZoneId(string zoneId, out string groupId, out int slot)
	{
		groupId = "";
		slot = -1;
		if (zoneId == "")
			return false;

		TStringArray parts = {};
		zoneId.Split(".", parts);
		if (parts.Count() < 2)
			return false;

		groupId = parts.Get(0);
		if (groupId == "")
			return false;

		string slotPart = parts.Get(1);
		int parsedSlot = slotPart.ToInt();
		if (parsedSlot > 0)
			slot = parsedSlot;

		return true;
	}

	protected void ResolveZoneGroupSettings(BQBotSpawnZoneConfig zoneCfg, out string groupId, out int groupSlot)
	{
		groupId = "";
		groupSlot = -1;
		if (!zoneCfg)
			return;

		if (zoneCfg.m_GroupId != "")
		{
			groupId = zoneCfg.m_GroupId;
		}

		if (zoneCfg.m_GroupSlot > 0)
			groupSlot = zoneCfg.m_GroupSlot;

		string parsedGroupId;
		int parsedSlot;
		if (ParseGroupFromCompositeZoneId(zoneCfg.m_Id, parsedGroupId, parsedSlot))
		{
			if (groupId == "")
				groupId = parsedGroupId;
			if (groupSlot < 1 && parsedSlot > 0)
				groupSlot = parsedSlot;
		}
	}

	protected ref BQBotSettings BuildBotSettingsFromZoneAndLoadout(BQBotSpawnZoneConfig zoneCfg, BQBotLoadoutConfig loadoutCfg)
	{
		if (!zoneCfg)
			return null;

		ref BQBotLoadoutConfig resolvedLoadoutCfg = loadoutCfg;
		if (!resolvedLoadoutCfg)
			resolvedLoadoutCfg = new BQBotLoadoutConfig();

		ref BQBotSettings bot = new BQBotSettings();
		bot.m_Spawn = new BQBotSpawnConfig();
		bot.m_Spawn.m_Classname = zoneCfg.m_Classname;
		bot.m_Spawn.m_Position = zoneCfg.m_Position;
		bot.m_Spawn.m_Orientation = zoneCfg.m_Orientation;
		bot.m_Spawn.m_ZoneId = zoneCfg.m_Id;
		string resolvedGroupId;
		int resolvedGroupSlot;
		ResolveZoneGroupSettings(zoneCfg, resolvedGroupId, resolvedGroupSlot);
		bot.m_Spawn.m_GroupId = resolvedGroupId;
		bot.m_Spawn.m_GroupSlot = resolvedGroupSlot;
		bot.m_Spawn.m_Formation = zoneCfg.m_Formation;
		if (bot.m_Spawn.m_Formation == "")
			bot.m_Spawn.m_Formation = "vee";
		bot.m_Spawn.m_FormationScale = zoneCfg.m_FormationScale;
		if (bot.m_Spawn.m_FormationScale <= 0.0)
			bot.m_Spawn.m_FormationScale = 1.0;
		bot.m_Spawn.m_FormationLooseness = zoneCfg.m_FormationLooseness;
		if (bot.m_Spawn.m_FormationLooseness < 0.0)
			bot.m_Spawn.m_FormationLooseness = 0.0;
		bot.m_Spawn.m_GroupPatrolEnabled = zoneCfg.m_GroupPatrolEnabled;
		bot.m_Spawn.m_GroupWaypointBehaviour = zoneCfg.m_GroupWaypointBehaviour;
		if (bot.m_Spawn.m_GroupWaypointBehaviour == "")
			bot.m_Spawn.m_GroupWaypointBehaviour = "loop";
		bot.m_Spawn.m_GroupRegroupDistanceM = zoneCfg.m_GroupRegroupDistanceM;
		if (bot.m_Spawn.m_GroupRegroupDistanceM <= 0.0)
			bot.m_Spawn.m_GroupRegroupDistanceM = 8.0;
		bot.m_Spawn.m_GroupBreakFormationDistanceM = zoneCfg.m_GroupBreakFormationDistanceM;
		if (bot.m_Spawn.m_GroupBreakFormationDistanceM <= 0.0)
			bot.m_Spawn.m_GroupBreakFormationDistanceM = 18.0;
		bot.m_Spawn.m_AllowDamage = resolvedLoadoutCfg.m_AllowDamage;
		if (bot.m_Spawn.m_AllowDamage != 0)
			bot.m_Spawn.m_AllowDamage = 1;
		bot.m_DamageMultiplier = Math.Clamp(resolvedLoadoutCfg.m_DamageMultiplier, 0.0, 10.0);
		bot.m_Weapon = CloneWeaponConfig(resolvedLoadoutCfg.m_Weapon);
		bot.m_Clothing = CloneClothingConfig(resolvedLoadoutCfg.m_Clothing);
		bot.m_Runtime = CloneRuntimeConfig(resolvedLoadoutCfg.m_Runtime);
		ApplyZoneAccuracyOverride(bot.m_Runtime, zoneCfg.m_Accuracy);
		return bot;
	}

	protected ref BQBotWeaponConfig CloneWeaponConfig(BQBotWeaponConfig src)
	{
		ref BQBotWeaponConfig dst = new BQBotWeaponConfig();
		if (!src)
			return dst;

		dst.m_Weapon = src.m_Weapon;
		dst.m_Magazine = src.m_Magazine;
		dst.m_MagAmmo = src.m_MagAmmo;
		dst.m_ReserveMagAmmo = src.m_ReserveMagAmmo;
		dst.m_FireMode = src.m_FireMode;
		return dst;
	}

	protected ref BQBotClothingConfig CloneClothingConfig(BQBotClothingConfig src)
	{
		ref BQBotClothingConfig dst = new BQBotClothingConfig();
		if (!src)
			return dst;

		dst.m_Head = src.m_Head;
		dst.m_Top = src.m_Top;
		dst.m_Pants = src.m_Pants;
		dst.m_Feet = src.m_Feet;
		dst.m_Vest = src.m_Vest;
		dst.m_Backpack = src.m_Backpack;
		dst.m_Gloves = src.m_Gloves;
		dst.m_Mask = src.m_Mask;
		return dst;
	}

	protected ref BQBotRuntimeConfig CloneRuntimeConfig(BQBotRuntimeConfig src)
	{
		ref BQBotRuntimeConfig dst = new BQBotRuntimeConfig();
		if (!src)
			return dst;

		dst.m_AggroRadius = src.m_AggroRadius;
		dst.m_DetectRadius = src.m_DetectRadius;
		dst.m_FireRadius = src.m_FireRadius;
		dst.m_ThreatCombatEnter = src.m_ThreatCombatEnter;
		dst.m_LostTargetMemoryMs = src.m_LostTargetMemoryMs;
		dst.m_TargetSearchTimeoutMs = src.m_TargetSearchTimeoutMs;
		dst.m_TargetSearchUpdateIntervalMs = src.m_TargetSearchUpdateIntervalMs;
		dst.m_TargetSearchReachM = src.m_TargetSearchReachM;
		dst.m_ChaseStopDistanceM = src.m_ChaseStopDistanceM;
		dst.m_ChaseJogDistanceM = src.m_ChaseJogDistanceM;
		dst.m_ChaseSprintDistanceM = src.m_ChaseSprintDistanceM;
		dst.m_ChaseProgressCheckMs = src.m_ChaseProgressCheckMs;
		dst.m_ChaseStuckRepathMs = src.m_ChaseStuckRepathMs;
		dst.m_ChaseStrafeDurationMs = src.m_ChaseStrafeDurationMs;
		dst.m_CoverEnabled = src.m_CoverEnabled;
		dst.m_CoverSearchRadius = src.m_CoverSearchRadius;
		dst.m_CoverMinDistance = src.m_CoverMinDistance;
		dst.m_CoverMaxDistance = src.m_CoverMaxDistance;
		dst.m_CoverThreatThreshold = src.m_CoverThreatThreshold;
		dst.m_CoverRecheckIntervalMs = src.m_CoverRecheckIntervalMs;
		dst.m_FlankingEnabled = src.m_FlankingEnabled;
		dst.m_EnableFlankingOutsideCombat = src.m_EnableFlankingOutsideCombat;
		dst.m_MaxFlankingDistance = src.m_MaxFlankingDistance;
		dst.m_FlankThreatMin = src.m_FlankThreatMin;
		dst.m_FlankThreatMax = src.m_FlankThreatMax;
		dst.m_FlankTimeoutMs = src.m_FlankTimeoutMs;
		dst.m_MovementSpeedLimit = src.m_MovementSpeedLimit;
		dst.m_MovementSpeedLimitUnderThreat = src.m_MovementSpeedLimitUnderThreat;
		dst.m_RoamingMovementSpeedLimit = src.m_RoamingMovementSpeedLimit;
		dst.m_SpeedLimitPreference = src.m_SpeedLimitPreference;
		dst.m_FOVNearDistThreshold = src.m_FOVNearDistThreshold;
		dst.m_FOVFarDistThreshold = src.m_FOVFarDistThreshold;
		dst.m_FOVNearHalfAngleH = src.m_FOVNearHalfAngleH;
		dst.m_FOVFarHalfAngleH = src.m_FOVFarHalfAngleH;
		dst.m_AccuracyBase = src.m_AccuracyBase;
		dst.m_AccuracyNearDistance = src.m_AccuracyNearDistance;
		dst.m_AccuracyFarDistance = src.m_AccuracyFarDistance;
		dst.m_AccuracyNearFactor = src.m_AccuracyNearFactor;
		dst.m_AccuracyFarFactor = src.m_AccuracyFarFactor;
		dst.m_AccuracyMaxSpreadDeg = src.m_AccuracyMaxSpreadDeg;
		dst.m_UpdateIntervalMs = src.m_UpdateIntervalMs;
		dst.m_MaintenanceIntervalMs = src.m_MaintenanceIntervalMs;
		dst.m_AimUpdateIntervalMs = src.m_AimUpdateIntervalMs;
		dst.m_TargetScanIntervalMs = src.m_TargetScanIntervalMs;
		dst.m_TargetScanNoTargetIntervalMs = src.m_TargetScanNoTargetIntervalMs;
		dst.m_TargetScanBudgetPerWindow = src.m_TargetScanBudgetPerWindow;
		dst.m_PlayerSnapshotIntervalMs = src.m_PlayerSnapshotIntervalMs;
		dst.m_LosCheckIntervalMs = src.m_LosCheckIntervalMs;
		dst.m_LosBudgetPerWindow = src.m_LosBudgetPerWindow;
		dst.m_BudgetWindowMs = src.m_BudgetWindowMs;
		dst.m_FireStartStaggerMs = src.m_FireStartStaggerMs;
		dst.m_NearestDebugIntervalMs = src.m_NearestDebugIntervalMs;
		return dst;
	}

	protected void ApplyZoneAccuracyOverride(BQBotRuntimeConfig runtimeCfg, BQBotAccuracyOverrideConfig overrideCfg)
	{
		if (!runtimeCfg || !overrideCfg)
			return;

		if (overrideCfg.m_Base >= 0.0)
			runtimeCfg.m_AccuracyBase = overrideCfg.m_Base;
		if (overrideCfg.m_NearDistance >= 0.0)
			runtimeCfg.m_AccuracyNearDistance = overrideCfg.m_NearDistance;
		if (overrideCfg.m_FarDistance >= 0.0)
			runtimeCfg.m_AccuracyFarDistance = overrideCfg.m_FarDistance;
		if (overrideCfg.m_NearFactor >= 0.0)
			runtimeCfg.m_AccuracyNearFactor = overrideCfg.m_NearFactor;
		if (overrideCfg.m_FarFactor >= 0.0)
			runtimeCfg.m_AccuracyFarFactor = overrideCfg.m_FarFactor;
		if (overrideCfg.m_MaxSpreadDeg >= 0.0)
			runtimeCfg.m_AccuracyMaxSpreadDeg = overrideCfg.m_MaxSpreadDeg;
	}

	protected void MigrateLegacyBotsConfig(array<ref BQBotSpawnZoneConfig> outZones, array<ref BQBotLoadoutConfig> outLoadouts)
	{
		if (!outZones || !outLoadouts || !FileExist(BQ_BOTS_LEGACY_PATH))
			return;

		ref array<ref BQBotSettings> legacy = new array<ref BQBotSettings>();
		string loadErr;
		if (!JsonFileLoader<ref array<ref BQBotSettings>>.LoadFile(BQ_BOTS_LEGACY_PATH, legacy, loadErr) || !legacy)
			return;

		int idx = 1;
		foreach (BQBotSettings bot : legacy)
		{
			if (!bot)
				continue;

			string idNum = idx.ToString();
			if (idx < 10)
				idNum = "00" + idNum;
			else if (idx < 100)
				idNum = "0" + idNum;

			ref BQBotSpawnZoneConfig zone = new BQBotSpawnZoneConfig();
			zone.m_Id = "zone_" + idNum;
			zone.m_LoadoutId = "loadout_" + idNum;
			if (bot.m_Spawn)
			{
				zone.m_Classname = bot.m_Spawn.m_Classname;
				zone.m_Position = bot.m_Spawn.m_Position;
				zone.m_Orientation = bot.m_Spawn.m_Orientation;
			}
			outZones.Insert(zone);

			ref BQBotLoadoutConfig loadout = new BQBotLoadoutConfig();
			loadout.m_Id = zone.m_LoadoutId;
			if (bot.m_Spawn)
				loadout.m_AllowDamage = bot.m_Spawn.m_AllowDamage;
			loadout.m_DamageMultiplier = Math.Clamp(bot.m_DamageMultiplier, 0.0, 10.0);
			loadout.m_Weapon = CloneWeaponConfig(bot.m_Weapon);
			loadout.m_Clothing = CloneClothingConfig(bot.m_Clothing);
			loadout.m_Runtime = CloneRuntimeConfig(bot.m_Runtime);
			outLoadouts.Insert(loadout);

			idx++;
		}

		if (outLoadouts.Count() == 0)
		{
			ref BQBotLoadoutConfig fallback = new BQBotLoadoutConfig();
			fallback.m_Id = "default";
			outLoadouts.Insert(fallback);
		}
	}

	protected void EnsureBotsDirectory()
	{
		if (!FileExist(BQ_BOTS_DIR))
			MakeDirectory(BQ_BOTS_DIR);
	}

	override void UpdatePlayersStats()
	{
		PluginLifespan moduleLifespan;
		Class.CastTo(moduleLifespan, GetPlugin(PluginLifespan));

		array<Man> players = new array<Man>();
		g_Game.GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase player;
			if (!Class.CastTo(player, man))
				continue;

			// Bots are static-combat entities: skip vanilla stats/lifespan workload.
			if (player.BQBot_IsBot())
				continue;

			player.StatUpdateByTime(AnalyticsManagerServer.STAT_PLAYTIME);
			player.StatUpdateByPosition(AnalyticsManagerServer.STAT_DISTANCE);

			if (moduleLifespan)
				moduleLifespan.UpdateLifespan(player);
		}

		UpdateCorpseStatesServer();
	}
}
