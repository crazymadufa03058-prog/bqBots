class BotWeaponConfig
{
	string m_Type;
	string m_Magazine;
	int    m_AmmoCount;
	ref array<string> m_Attachments;

	void BotWeaponConfig()
	{
		m_Type = "";
		m_Magazine = "";
		m_AmmoCount = 0;
		m_Attachments = new array<string>();
	}
};

class BotClothingConfig
{
	ref array<string> m_Head;
	ref array<string> m_Top;
	ref array<string> m_Pants;
	ref array<string> m_Feet;
	ref array<string> m_Vest;
	ref array<string> m_Backpack;

	void BotClothingConfig()
	{
		m_Head = new array<string>();
		m_Top = new array<string>();
		m_Pants = new array<string>();
		m_Feet = new array<string>();
		m_Vest = new array<string>();
		m_Backpack = new array<string>();
	}
};

class BotLoadoutConfig
{
	string m_Name;
	int    m_Weight;
	ref array<ref BotWeaponConfig> m_Weapons;
	ref BotClothingConfig m_Clothing;

	void BotLoadoutConfig()
	{
		m_Name = "";
		m_Weight = 50;
		m_Weapons = new array<ref BotWeaponConfig>();
		m_Clothing = new BotClothingConfig();
	}
};

class BotZoneConfig
{
	string m_Name;
	vector m_Center;
	float  m_Radius;
	int    m_BotCountMin;
	int    m_BotCountMax;
	ref array<string> m_Loadouts;
	string m_Behaviour;
	ref array<vector> m_Waypoints;
	int    m_RespawnTime;

	void BotZoneConfig()
	{
		m_Name = "";
		m_Center = "0 0 0";
		m_Radius = 200;
		m_BotCountMin = 2;
		m_BotCountMax = 4;
		m_Loadouts = new array<string>();
		m_Behaviour = "patrol";
		m_Waypoints = new array<vector>();
		m_RespawnTime = 600;
	}
};

class BotAccuracyConfig
{
	float m_BaseAccuracy;
	float m_AccuracyDropPerMeter;
	float m_MovingPenalty;
	float m_CrouchBonus;
	float m_ProneBonus;
	float m_FirstShotDelayMin;
	float m_FirstShotDelayMax;
	float m_BurstPauseMin;
	float m_BurstPauseMax;

	void BotAccuracyConfig()
	{
		m_BaseAccuracy = 0.6;
		m_AccuracyDropPerMeter = 0.003;
		m_MovingPenalty = 0.3;
		m_CrouchBonus = 0.15;
		m_ProneBonus = 0.25;
		m_FirstShotDelayMin = 0.5;
		m_FirstShotDelayMax = 1.5;
		m_BurstPauseMin = 0.8;
		m_BurstPauseMax = 2.0;
	}
};

class BotDamageConfig
{
	float m_DamageMultiplierToPlayers;
	float m_DamageMultiplierFromPlayers;
	float m_HeadDamageMultiplier;
	bool  m_FriendlyFire;

	void BotDamageConfig()
	{
		m_DamageMultiplierToPlayers = 1.0;
		m_DamageMultiplierFromPlayers = 1.0;
		m_HeadDamageMultiplier = 4.0;
		m_FriendlyFire = false;
	}
};

class BotDetectionConfig
{
	float m_VisionRange;
	float m_VisionAngle;
	float m_HearingRange;
	float m_NightVisionMultiplier;
	float m_BushCoverMultiplier;

	void BotDetectionConfig()
	{
		m_VisionRange = 150;
		m_VisionAngle = 120;
		m_HearingRange = 50;
		m_NightVisionMultiplier = 0.4;
		m_BushCoverMultiplier = 0.5;
	}
};

class BotSettings
{
	int m_MaxBots;
	int m_MaxBotsPerZone;
	float m_DespawnDistance;
	float m_ActivationDistance;
	int m_TickBudget;
	ref BotDamageConfig m_Damage;
	ref BotAccuracyConfig m_Accuracy;
	ref BotDetectionConfig m_Detection;

	void BotSettings()
	{
		m_MaxBots = 30;
		m_MaxBotsPerZone = 8;
		m_DespawnDistance = 800;
		m_ActivationDistance = 500;
		m_TickBudget = 5;
		m_Damage = new BotDamageConfig();
		m_Accuracy = new BotAccuracyConfig();
		m_Detection = new BotDetectionConfig();
	}
};
