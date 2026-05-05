class BQBotSpawnConfig
{
	string m_Classname = "bqBots_Boris_AI";
	string m_Position = "6108.52 298.921 7634.82";
	string m_Orientation = "0 0 0";
	int m_AllowDamage = 1;
	string m_ZoneId = "";
	string m_GroupId = "";
	int m_GroupSlot = -1;
	string m_Formation = "vee"; // vee | invvee | column | invcolumn | file | invfile | wall | line | loose | circle | circledot | star | stardot
	float m_FormationScale = 1.0;
	float m_FormationLooseness = 0.6;
	int m_GroupPatrolEnabled = 1;
	string m_GroupWaypointBehaviour = "loop"; // loop | alternate | halt | roaming | roaming_local
	float m_GroupRegroupDistanceM = 8.0;
	float m_GroupBreakFormationDistanceM = 18.0;
}

class BQBotWeaponConfig
{
	string m_Weapon = "AKM";
	string m_Magazine = "Mag_AKM_30Rnd";
	int m_MagAmmo = 30;
	int m_ReserveMagAmmo = 30;
	string m_FireMode = "auto"; // auto | burst | single
}

class BQBotClothingConfig
{
	string m_Head = "";
	string m_Top = "";
	string m_Pants = "";
	string m_Feet = "";
	string m_Vest = "PressVest_Blue";
	string m_Backpack = "TaloonBag_Blue";
	string m_Gloves = "";
	string m_Mask = "";
}

class BQBotRuntimeConfig
{
	float m_AggroRadius = 300.0;
	float m_DetectRadius = 500.0;
	float m_FireRadius = 250.0;
	float m_ThreatCombatEnter = 0.40;
	int m_LostTargetMemoryMs = 9000;
	int m_TargetSearchTimeoutMs = 15000;
	int m_TargetSearchUpdateIntervalMs = 1800;
	float m_TargetSearchReachM = 1.2;
	float m_ChaseStopDistanceM = 3.0;
	float m_ChaseJogDistanceM = 6.0;
	float m_ChaseSprintDistanceM = 20.0;
	int m_ChaseProgressCheckMs = 250;
	int m_ChaseStuckRepathMs = 800;
	int m_ChaseStrafeDurationMs = 350;
	bool m_CoverEnabled = true;
	float m_CoverSearchRadius = 50.0;
	float m_CoverMinDistance = 4.0;
	float m_CoverMaxDistance = 28.0;
	float m_CoverThreatThreshold = 0.60;
	int m_CoverRecheckIntervalMs = 2500;
	bool m_FlankingEnabled = true;
	bool m_EnableFlankingOutsideCombat = false;
	float m_MaxFlankingDistance = 200.0;
	float m_FlankThreatMin = 0.152;
	float m_FlankThreatMax = 0.80;
	int m_FlankTimeoutMs = 9000;
	int m_MovementSpeedLimit = 3;
	int m_MovementSpeedLimitUnderThreat = 3;
	int m_RoamingMovementSpeedLimit = 2;
	int m_SpeedLimitPreference = 3;
	float m_FOVNearDistThreshold = 0.0;
	float m_FOVFarDistThreshold = 125.0;
	float m_FOVNearHalfAngleH = 120.0;
	float m_FOVFarHalfAngleH = 45.0;
	float m_AccuracyBase = 0.85;
	float m_AccuracyNearDistance = 20.0;
	float m_AccuracyFarDistance = 300.0;
	float m_AccuracyNearFactor = 1.0;
	float m_AccuracyFarFactor = 0.25;
	float m_AccuracyMaxSpreadDeg = 6.0;
	int m_UpdateIntervalMs = 80;
	int m_MaintenanceIntervalMs = 800;
	int m_AimUpdateIntervalMs = 60;
	int m_TargetScanIntervalMs = 150;
	int m_TargetScanNoTargetIntervalMs = 500;
	int m_TargetScanBudgetPerWindow = 40;
	int m_PlayerSnapshotIntervalMs = 200;
	int m_LosCheckIntervalMs = 80;
	int m_LosBudgetPerWindow = 150;
	int m_BudgetWindowMs = 80;
	int m_FireStartStaggerMs = 20;
	int m_NearestDebugIntervalMs = 2000;
}

class BQBotSettings
{
	ref BQBotSpawnConfig m_Spawn;
	ref BQBotWeaponConfig m_Weapon;
	ref BQBotClothingConfig m_Clothing;
	ref BQBotRuntimeConfig m_Runtime;
	float m_DamageMultiplier = 1.0;

	void BQBotSettings()
	{
		m_Spawn = new BQBotSpawnConfig();
		m_Weapon = new BQBotWeaponConfig();
		m_Clothing = new BQBotClothingConfig();
		m_Runtime = new BQBotRuntimeConfig();
	}
}
