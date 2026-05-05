
typedef map<BuildingBase, ref map<int, ref ExpansionLadder>> eAILadders;
typedef map<EntityAI, bool> eAIThreatOverride;
typedef map<string, float> eAIRecentlyDroppedItems;
typedef map<eAITargetInformation, ref eAITarget> eAITargetInformationStates;

class eAIBase: PlayerBase
{
	ref ExpansionHumanST m_ExpansionST;
	ref map<int, Object> m_Expansion_DebugObjects = new map<int, Object>;
	const int EAI_COMMANDID_MOVE = 1;
	int m_BQNextEAIWeaponTraceTime;

	Object Expansion_DebugObject(int id, vector position, string type = "ExpansionDebugBox", vector direction = "0 0 0", vector origin = "0 0 0", float lifetime = 300.0, int flags = 0)
	{
		return null;
	}

	Object Expansion_DebugObject_Deferred(int id, vector position, string type = "ExpansionDebugBox", vector direction = "0 0 0", vector origin = "0 0 0", float lifetime = 300.0, int flags = 0)
	{
		return null;
	}

	static float LOS_CHECK_INTERVAL = 0.15;

	static bool AI_HANDLEDOORS = true;
	static bool AI_HANDLEVAULTING = true;

	static int s_eAI_UnlimitedReload;
	static int s_eAI_UnlimitedReloadAll;
	static int s_eAI_FTO = 2;

	private static autoptr array<eAIBase> s_AllAI = new array<eAIBase>();
	static ref CF_DoublyLinkedNodes_WeakRef<eAIBase> s_eAI_Alive = new CF_DoublyLinkedNodes_WeakRef<eAIBase>;
	ref CF_DoublyLinkedNode_WeakRef<eAIBase> m_eAI_Alive_Node = s_eAI_Alive.Add(this);

	static float s_eAI_LastCEUpdateTime;
	static ref set<Object> s_eAI_TakenCoverObjects = new set<Object>;

	//! model name w/o .p3d ext, can be partial
	static ref TStringArray s_eAI_PreventClimb = {
		"pole",
		"sign",
		"busstop",
		"stairs",
		"farm_strawstack",
		"misc_walkover",
		"rubble_dirtpile_large",
		"stockyard_oremound",
		"wall_canal_10"
	};

	//! model name w/o .p3d ext, can be partial
	static ref TStringArray s_eAI_PreventClimbOver = {
		"container",
		"tank_small",
		"misc_haybale",
		"misc_woodreserve",
		"misc_advertcolumn",
		"misc_coil",
		"ice_sea"
	};

	ref SHumanCommandClimbResult m_eAI_LastSuccessfulClimbTestResult;

	static string s_Expansion_SurvivorDisplayName = g_Game.ConfigGetTextOut(CFG_VEHICLESPATH + " SurvivorBase displayName");

	private ref eAICallbacks m_eAI_Callbacks = new eAICallbacks(this);

	protected autoptr eAIFSM m_FSM;
	bool m_eAI_IsFightingFSM;
	bool m_eAI_ShouldTakeCover;
	bool m_eAI_UpdatePotentialCoverObjects;

	//! Fighting FSM variables
	int m_eAI_LastFireTime;
	int m_eAI_TimeBetweenFiring = 10000;
	int m_eAI_TimeBetweenFiringAndGettingUp = 15000;
	int m_eAI_LastEvadeTime;
	float m_eAI_DistanceToTargetSq;

	// Targeting data
	private autoptr array<ref eAITarget> m_eAI_Targets;
	bool m_eAI_TargetChanged;
	ref eAITarget m_eAI_ItemTargetHistory[3];
	typename m_eAI_LastEngagedTargetType;
	int m_eAI_AcuteDangerTargetCount;
	int m_eAI_AcuteDangerPlayerTargetCount;
	float m_eAI_SilentAttackViabilityTime;
#ifdef DIAG_DEVELOPER
	bool m_eAI_PrintCurrentTarget;
#endif
	ref eAITargetInformationStates m_eAI_TargetInformationStates = new eAITargetInformationStates;
	float m_ThreatClearedTimeout;
	float m_eAI_CurrentThreatToSelf;
	float m_eAI_PreviousThreatToSelf;
	float m_eAI_CurrentThreatToSelfActive;
	float m_eAI_PreviousThreatToSelfActive;

	//[eAIAttribute<float>.Register("m_eAI_AccuracyMin")]
	float m_eAI_AccuracyMin;

	//[eAIAttribute<float>.Register("m_eAI_AccuracyMax")]
	float m_eAI_AccuracyMax;

	//[eAIAttribute<float>.Register("m_eAI_ThreatDistanceLimit")]
	float m_eAI_ThreatDistanceLimit;

	//[eAIAttribute<float>.Register("m_eAI_NoiseInvestigationDistanceLimit")]
	float m_eAI_NoiseInvestigationDistanceLimit;

	//[eAIAttribute<float>.Register("m_eAI_DamageMultiplier")]
	float m_eAI_DamageMultiplier;

	float m_eAI_DamageReceivedMultiplier;
	bool m_eAI_SyncCurrentTarget;
	int m_eAI_CurrentTarget_NetIDLow;
	int m_eAI_CurrentTarget_NetIDHigh;
	int m_eAI_CurrentTarget_NetIDLowSync;
	int m_eAI_CurrentTarget_NetIDHighSync;
	ref eAINoiseTargetInformation m_eAI_NoiseTargetInfo = new eAINoiseTargetInformation();
	int m_eAI_NoiseTarget;
	private bool m_eAI_HasLOS;
	Object m_eAI_HitObject;
	float m_eAI_DbgThreshAngleH;
	float m_eAI_DbgLookAngleH;
	int m_eAI_DbgLOSAngles = -1;

	// Command handling
	ref eAICommandMove m_eAI_CommandMove;
	protected int m_eAI_CurrentCommandID;
	float m_eAI_LOSCheckDT;
	float m_eAI_CommandTime;
	int m_eAI_LastUpdateTime;
	bool m_eAI_DeathHandled;
	bool m_eAI_SkipScript;
	bool m_eAI_JumpClimb;
	bool m_eAI_IsOnLadder;
	float m_eAI_LadderTime;
	int m_eAI_LadderClimbDirection;  //! 1 = up, -1 = down
	ExpansionLadder m_eAI_Ladder;
	vector m_eAI_LadderEntryPoint;
	vector m_eAI_LadderDirPoint;

	//[eAIAttribute<BuildingBase>.Register("m_eAI_BuildingWithLadder")]
	BuildingBase m_eAI_BuildingWithLadder;
	bool m_eAI_FloorIsBuildingWithLadder;
	bool m_eAI_TargetIsInBuildingWithLadderRadius;
	
	//[eAIAttribute<eAILadders>.Register("m_eAI_Ladders")]
	ref eAILadders m_eAI_Ladders = new eAILadders;

	//[eAIAttribute<int>.Register("m_eAI_LadderLoops")]
	int m_eAI_LadderLoops;

	//[eAIAttribute<BuildingBase>.Register("m_eAI_LastClimbedBuildingWithLadder")]
	BuildingBase m_eAI_LastClimbedBuildingWithLadder;

	//[eAIAttribute<ExpansionLadder>.Register("m_eAI_LastClimbedLadder")]
	ref ExpansionLadder m_eAI_LastClimbedLadder;

	bool m_eAI_PreferLadder;
	bool m_eAI_Recreate;
	bool m_eAI_Recreating;

	private Transport m_eAI_Transport;
	private int m_eAI_Transport_SeatIndex;

	bool m_eAI_IsPreparingMelee;
	int m_eAI_MeleeTime;

	private ref eAIAimingProfile m_AimingProfile;

	private ref eAIActionManager m_eActionManager;
	ref eAIMeleeCombat m_eMeleeCombat;
	ref eAIMeleeFightLogic_LightHeavy m_eAI_MeleeFightLogic;

	bool m_eAI_FallHasLanded;
	float m_eAI_FallYVelZeroTime;

	private bool m_eAI_IsChangingStance;
	private bool m_eAI_ShouldGetUp = true;
	eAIStance m_eAI_StancePreference = -1;
	eAIStance m_eAI_DefaultStance = eAIStance.STANDING;
	float m_eAI_DefaultLookAngle;

	bool m_eAI_IsRestrained;
	bool m_eAI_IsInventoryVisible;

	// Position for aiming/looking in the world
	private vector m_eAI_LookPosition_WorldSpace;
	private vector m_eAI_AimPosition_WorldSpace;

	// A direction vector (not YPR) in Model Space, not World Space
	private vector m_eAI_LookRelAngles;
	private vector m_eAI_LookDirectionTarget_ModelSpace;
	private bool m_eAI_LookDirection_Recalculate;
	float m_eAI_LookVelLR[1];
	float m_eAI_LookVelUD[1];

	private vector m_eAI_AimRelAngles;
	protected float m_eAI_AimRelAngleLR;
	protected float m_eAI_AimRelAngleUD;
	private vector m_eAI_AimDirectionTarget_ModelSpace;
	private bool m_eAI_AimDirection_Recalculate;
	private vector m_eAI_AimDirectionPrev;
	float m_eAI_AimVelLR[1];
	float m_eAI_AimVelUD[1];

	//! Dynamic FOV (detection angle) depending on distance to target
	static float m_eAI_FOVNear_DistThreshold = 0;
	static float m_eAI_FOVFar_DistThreshold = 125;
	static float m_eAI_FOVNear_HalfAngleH = 120;  //! Half angle! FOV = angle * 2
	static float m_eAI_FOVFar_HalfAngleH = 45;  //! Half angle! FOV = angle * 2
	static float m_eAI_FOVTransitionExponent = 2.0;  //! 1.0 = linear

	private bool m_MovementSpeedActive;
	private int m_MovementSpeed;

	//[eAIAttribute<int>.Register("m_MovementSpeedLimit")]
	private int m_MovementSpeedLimit = 3;

	//[eAIAttribute<int>.Register("m_MovementSpeedLimitUnderThreat")]
	private int m_MovementSpeedLimitUnderThreat = 3;

	//[eAIAttribute<int>.Register("m_eAI_RoamingMovementSpeedLimit")]
	int m_eAI_RoamingMovementSpeedLimit;

	//[eAIAttribute<int>.Register("m_eAI_SpeedLimitPreference")]
	int m_eAI_SpeedLimitPreference = 3;

	bool m_eAI_IsAttachedToMovingParent;

	private bool m_MovementDirectionActive;
	protected bool m_eAI_ResetMovementDirectionActive;
	private float m_MovementDirection;
	private float m_SideStepAngle;
	private bool m_eAI_TurnTargetActive;
	private float m_eAI_TurnTarget;
	bool m_eAI_Halt;
	bool m_eAI_ResetInteractionLayer;
	float m_eAI_PositionTime;
	float m_eAI_BlockedTime;
	float m_eAI_LastDoorInteractionTime;

	float m_eAI_FormationPositionUpdateTime;
	vector m_eAI_FormationPosition;
	vector m_eAI_FormationDirection;

	float m_eAI_FormationDirectionUpdateTime;
	float m_eAI_FormationDirectionNextUpdateTime;
	float m_eAI_MovementSpeedPrev;

	bool m_eAI_HasProjectileWeaponInHands;
	private bool m_WeaponRaised;
	private bool m_WeaponRaisedPrev;

	private float m_WeaponRaisedTimer;
	private float m_WeaponLowerTimeout;


	ref array<Weapon_Base> m_eAI_Firearms = {};
	ref array<Weapon_Base> m_eAI_Handguns = {};
	ref array<Weapon_Base> m_eAI_Launchers = {};
	ref array<ItemBase> m_eAI_MeleeWeapons = {};
	ref array<ItemBase> m_eAI_RepairKits = {};
	ref array<Magazine> m_eAI_Magazines = {};
	ref array<ItemBase> m_eAI_Food = {};
	ref array<ItemBase> m_eAI_ItemsToDrop = {};
	int m_eAI_MaxFoodCount;

	ref map<typename, Magazine> m_eAI_EvaluatedFirearmTypes = new map<typename, Magazine>;

	int m_eAI_AccessibleCargoSpaceTaken;
	int m_eAI_AccessibleCargoSpaceTotal;

	ref set<Clothing> m_eAI_CargosToTidy = new set<Clothing>;

#ifdef DIAG_DEVELOPER
	EntityAI m_eAI_CompareWeapon;
	EntityAI m_eAI_CurrentWeapon;
#endif

	Object m_eAI_SideStepObject;
	private float m_eAI_SideStepTimeout;
	private bool m_eAI_SideStepCancelOnLOS;
	private float m_eAI_PositionOverrideTimeout;

	//[eAIAttribute<int>.Register("m_eAI_UnlimitedReload")]
	private int m_eAI_UnlimitedReload;

	bool m_eAI_IsUnlimitedReloadAll;

	//[eAIAttribute<float>.Register("m_eAI_SniperProneDistanceThreshold")]
	private float m_eAI_SniperProneDistanceThreshold;

	//[eAIAttribute<int>.Register("m_eAI_LootingBehavior")]
	int m_eAI_LootingBehavior = eAILootingBehavior.DEFAULT;

	// Path Finding
#ifndef EAI_USE_LEGACY_PATHFINDING
	/*private*/ ref ExpansionPathHandler m_PathFinding;
#else
	/*private*/ ref eAIPathFinding m_PathFinding;
#endif
	vector m_eAI_TargetPosition;
	bool m_eAI_TargetPositionIsFinal = true;
	bool m_eAI_PositionIsFinal = true;
	int m_eAI_PrevHCCState = -1;
	float m_eAI_SurfaceY;
	//! @note we track the last entered area and NOT the cluster directly because it is a mutable property of the area
	//! and WILL be recreated whenever dynamic areas get spawned/despawned by CE!
	EffectArea m_eAI_EffectArea;
	int m_eAI_DangerousAreaCount;
	vector m_eAI_DangerousAreaAvoidancePosition;
	ref map<int, float> m_eAI_ProtectionLevels;
	bool m_eAI_IsInDangerByArea;

	private Apple m_DebugTargetApple;
	private vector m_DebugTargetOrientation;

	float m_eAI_UpdateTargetsTick;
	autoptr array<EntityAI> m_eAI_PotentialTargetEntities = new array<EntityAI>();
	ref array<EntityAI> m_eAI_TrackedBodies = {};
	int m_eAI_CurrentPotentialTargetIndex;
	CF_DoublyLinkedNode_WeakRef<PlayerBase> m_eAI_PotentialTargetPlayer;
	ref set<CF_DoublyLinkedNode_WeakRef<PlayerBase>> m_eAI_PlayersWithinVisibilityDistanceLimit = new set<CF_DoublyLinkedNode_WeakRef<PlayerBase>>;
	CF_DoublyLinkedNode_WeakRef<eAICustomCreatureTargetInformation> m_eAI_PotentialTargetCustomCreature;
	float m_eAI_UpdateNearTargetsTime;
	int m_eAI_UpdateNearTargetsCount;
	ref set<Object> m_eAI_PotentialCoverObjects = new set<Object>;
	Object m_eAI_CurrentCoverObject;
	vector m_eAI_CurrentCoverPosition;
	bool m_eAI_IsInCover;
	float m_eAI_MaxFlankingDistance = 200;
	bool m_eAI_EnableFlankingOutsideCombat;
	float m_eAI_FlankAngle;
	float m_eAI_FlankTime;
	float m_eAI_FlankTimeMax;

	int m_Expansion_EmoteID;
	bool m_Expansion_EmoteAutoCancel;
	int m_Expansion_EmoteAutoCancelDelay; //! ms

	int m_eAI_MinTimeTillNextFire;
	int m_eAI_QueuedShots;

	private float m_eAI_UpdateVisibilityTime = 5.0;  //! Force 1st update
	private float m_eAI_UpdateVisibilityTimeThreshold = 5.0;

	float m_Expansion_DaylightVisibility;
	float m_Expansion_Visibility = 0.1;
	float m_Expansion_VisibilityDistThreshold = 90.0;
	float m_eAI_DayNightThreshold = 0.4;  //! below threshold = night, above = day
	int m_eAI_IsDay = -1;  //! -1 = not set (implicit daytime), 0 = night, 1 = day
	float m_eAI_MinVisibility;
	float m_eAI_NightVisibility;
	float m_eAI_FogVisibility;
	float m_eAI_OvercastVisibility;
	float m_eAI_RainVisibility;
	float m_eAI_SnowVisibility;
	float m_eAI_DynVolFogVisibility;
	float m_eAI_ContaminatedAreaVisibility;
	float m_eAI_BaseVisibility;
	float m_eAI_VisibilityLimit;
	ref array<ItemBase> m_Expansion_ActiveVisibilityEnhancers = {};
	bool m_Expansion_TriedTurningOnVisibilityEnhancers;
	bool m_eAI_IsActiveVisibilityEnhancerTemporary;

	string m_eAI_TypeSwitchedOnDuringCombat;

	ref set<Man> m_eAI_InteractingPlayers = new set<Man>;

	//[eAIAttribute<bool>.Register("m_eAI_DespawnOnLoosingAggro")]
	bool m_eAI_DespawnOnLoosingAggro;

	//ref IsObjectObstructedCache m_eAI_IsItemObstructedCache = new IsObjectObstructedCache(vector.Zero, 1);
	//[eAIAttribute<eAIThreatOverride>.Register("m_eAI_ThreatOverride")]
	ref eAIThreatOverride m_eAI_ThreatOverride = new eAIThreatOverride;

	//[eAIAttribute<ItemBase>.Register("m_eAI_LastDroppedItem")]
	ItemBase m_eAI_LastDroppedItem;

	//[eAIAttribute<eAIRecentlyDroppedItems>.Register("m_eAI_RecentlyDroppedItems")]
	ref eAIRecentlyDroppedItems m_eAI_RecentlyDroppedItems = new eAIRecentlyDroppedItems;

	ref Timer m_eAI_ClientUpdateTimer;

	static ref ExpansionSoundSet s_eAI_LoveSound01_SoundSet = ExpansionSoundSet.Register("Expansion_AI_The_Sound_Of_Love_01_SoundSet");
	static ref ExpansionSoundSet s_eAI_LoveSound02_SoundSet = ExpansionSoundSet.Register("Expansion_AI_The_Sound_Of_Love_02_SoundSet");

	int m_eAI_Meme;

	//[eAIAttribute<int>.Register("m_eAI_MemeLevel")]
	int m_eAI_MemeLevel;

	float m_eAI_Lean;
	float m_eAI_LeanTarget;

	bool m_eAI_Unbug;
	
#ifndef DAYZ_1_25	
	vector m_ExTransformPlayer[4];
#endif
	
	ref set<PlayerBase> m_eAI_Spectators = new set<PlayerBase>;

	ref array<ref ExpansionPrefab> m_eAI_LootDropOnDeath;

	void eAIBase()
	{
#ifdef EAI_TRACE
#endif

#ifdef CF_DEBUG
		CF_Debug.Create(this);
#endif

		s_AllAI.Insert(this);

		if (g_Game.IsServer())
		{
			ExpansionAISettings settings = GetExpansionSettings().GetAI();
			AI_HANDLEVAULTING = settings.Vaulting;
			m_eAI_MemeLevel = settings.MemeLevel;
		}

		SetEventMask(EntityEvent.INIT);

	#ifdef AdmiralsDivingMod
		m_Swimming = new eAIImplementSwimming(this);
	#endif

/*
		if (g_Game.IsClient())
		{
			m_eAI_ClientUpdateTimer = new Timer(CALL_CATEGORY_SYSTEM);
			m_eAI_ClientUpdateTimer.Run(1.0 / 30.0, this, "eAI_ClientUpdate", NULL, true);
		}
*/
	}

	static eAIBase Get(int index)
	{
#ifdef EAI_TRACE
#endif
		
		if (index >= s_AllAI.Count())
		{
			return null;
		}

		return s_AllAI[index];
	}

	static array<eAIBase> eAI_GetAll()
	{
		return s_AllAI;
	}

	static void eAI_ToggleFTO()
	{
		if (s_eAI_FTO == 2)
			s_eAI_FTO = 0;
		else
			++s_eAI_FTO;

		ExpansionWorld world;
		Class.CastTo(world, GetDayZGame().GetExpansionGame());

		if (s_eAI_FTO > 0)
		{
			eAICommandMove.OBSTACLE_AVOIDANCE_INTERVAL = 0.1;

			world.m_LastAIUpdateTime = 0;

			auto node = s_eAI_Alive.m_Head;
			while (node)
			{
				node.m_Value.m_eAI_LastUpdateTime = 0;
				node = node.m_Next;
			}
		}
		else
		{
			eAICommandMove.OBSTACLE_AVOIDANCE_INTERVAL = 0.12;

			world.DbgAIUpdate();
		}
	}

	//! @note Init is called from vanilla PlayerBase ctor, so runs before eAIBase ctor!
	override void Init()
	{
#ifdef EAI_TRACE
#endif

		super.Init();

		m_eAI_Targets = {};

		m_eAI_ProtectionLevels = new map<int, float>;

		m_AimingProfile = new eAIAimingProfile(this);

		m_eMeleeCombat = new eAIMeleeCombat(this);
		m_MeleeCombat = m_eMeleeCombat;

		m_eAI_MeleeFightLogic = new eAIMeleeFightLogic_LightHeavy(this);
		m_MeleeFightLogic = m_eAI_MeleeFightLogic;

		// bqBots keeps its own vanilla fire bridge; do not replace the weapon manager here.
		if (g_Game.IsServer())
		{
			eAI_SetAccuracy(-1, -1);
			eAI_SetThreatDistanceLimit(-1);
			eAI_SetNoiseInvestigationDistanceLimit(-1);
			eAI_SetMaxFlankingDistance(-1);
			eAI_SetEnableFlankingOutsideCombat(-1);
			eAI_SetDamageMultiplier(-1);
			eAI_SetDamageReceivedMultiplier(-1);
			m_eAI_SniperProneDistanceThreshold = GetExpansionSettings().GetAI().SniperProneDistanceThreshold;

#ifndef EAI_USE_LEGACY_PATHFINDING
			m_PathFinding = new ExpansionPathHandler(this);
#else
			m_PathFinding = new eAIPathFinding(this);
#endif

			LoadFSM();

			s_Expansion_AllPlayers.m_OnRemove.Insert(eAI_OnRemovePlayer);
			eAICustomCreatureTargetInformation.s_AllCustomCreatures.m_OnRemove.Insert(eAI_OnRemoveCustomCreature);

			GetStatWater().Set(GetStatWater().GetMax());
			GetStatEnergy().Set(GetStatEnergy().GetMax());
		}

		LookAtDirection("0 0 1");
		AimAtDirection("0 0 1");

		//! Vehicles mod will set this in PlayerBase::Init if loaded
		if (!m_ExpansionST)
			m_ExpansionST = new ExpansionHumanST(this);

		if (g_Game.IsServer())
		{
			m_eAI_CommandMove = new eAICommandMove(this, m_ExpansionST, DayZPlayerConstants.STANCEIDX_ERECT);

			eAINoiseSystem.SI_OnNoiseAdded.Insert(eAI_OnNoiseEvent);
		}

		m_Expansion_EnableBonePositionUpdate = true;

	}

	override void Expansion_Init()
	{
		if (g_Game.IsServer() && !m_eAI_FactionType)
		{
			m_eAI_FactionType = eAIFactionRaiders;
		}

		super.Expansion_Init();

		RegisterNetSyncVariableBool("m_Expansion_CanBeLooted");
		//RegisterNetSyncVariableFloat("m_eAI_AccuracyMin");
		//RegisterNetSyncVariableFloat("m_eAI_AccuracyMax");
		//RegisterNetSyncVariableInt("m_eAI_CurrentTarget_NetIDLow");
		//RegisterNetSyncVariableInt("m_eAI_CurrentTarget_NetIDHigh");
	//#ifdef DIAG_DEVELOPER
		//RegisterNetSyncVariableInt("m_eAI_DbgLOSAngles");
	//#endif
		RegisterNetSyncVariableBool("m_eAI_IsInventoryVisible");

		m_Expansion_NetsyncData = new ExpansionNetsyncData(this);
	}

	override void OnPlayerLoaded()
	{
		if (m_Environment)
			m_Environment.Init();
		
		if (!g_Game.IsDedicatedServer())
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateCorpseStateVisual, 2000, false);
			m_PlayerSoundEventHandler = new PlayerSoundEventHandler(this);
			m_ReplaceSoundEventHandler = new ReplaceSoundEventHandler(this);
		}

		int slotId = InventorySlots.GetSlotIdFromString("Head");
		m_CharactersHead = Head_Default.Cast(GetInventory().FindPlaceholderForSlot(slotId));
		CheckHairClippingOnCharacterLoad();
		UpdateHairSelectionVisibility();
		PreloadDecayTexture();
		
		Weapon_Base wpn = Weapon_Base.Cast(GetItemInHands());
		if (wpn)
		{
			if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_AI_REMOTE)
			{
				wpn.DelayedValidateAndRepair();
			}
			else
			{
				wpn.ValidateAndRepair();
			}
		}

		m_PlayerLoaded = true;
	}

	static void ReloadAllFSM()
	{
		array<eAIBase> ai();
		foreach (auto ai0 : s_AllAI)
		{
			if (!ai0.GetFSM())
			{
				continue;
			}

			ai0.m_FSM = null;

			ai.Insert(ai0);
		}

		ExpansionFSMType.UnloadAll();

		foreach (auto ai1 : ai)
		{
			ai1.LoadFSM();
		}
	}

	void eAI_ResetPathfinding()
	{
		m_PathFinding.ResetUnreachable();
		m_eAI_LadderLoops = 0;
	}

	void LoadFSM()
	{
		ExpansionFSMType type = ExpansionFSMType.LoadXML("DayZExpansion/AI/scripts/FSM", "Master");
		ExpansionFSMOwnerType owner = this;
		if (type && Class.CastTo(m_FSM, type.Spawn(owner, null)))
		{
			m_FSM.StartDefault();
		}
		else
		{
			CF_Log.Error("Invalid FSM");
			Delete();
		}
	}

	void ~eAIBase()
	{
#ifdef EAI_TRACE
#endif

	#ifndef DIAG_DEVELOPER
		if (!g_Game)
			return;
	#endif

#ifdef CF_DEBUG
		CF_Debug.Destroy(this);
#endif

		s_AllAI.RemoveItem(this);

		if (s_eAI_Alive)
			s_eAI_Alive.Remove(m_eAI_Alive_Node);
	}

	protected override void EOnInit(IEntity other, int extra)
	{
		EXTrace.Print(EXTrace.AI, this, "EOnInit");

		OnSelectPlayer();
	}

	override void OnSelectPlayer()
	{
		EXTrace.Print(EXTrace.AI, this, "OnSelectPlayer");

		m_QuickBarBase.updateSlotsCount();

		m_PlayerSelected = true;

		m_WeaponManager.SortMagazineAfterLoad();

		if (g_Game.IsServer())
		{
			//! add callbacks for ai target system
			SetAITargetCallbacks(new AITargetCallbacksPlayer(this));

			GetSoftSkillsManager().InitSpecialty(GetStatSpecialty().Get());
			GetModifiersManager().SetModifiers(true);

			SetSynchDirty();
		}

		CheckForGag();

		m_eActionManager = new eAIActionManager(this);
		m_ActionManager = m_eActionManager;
	}

	override void DeferredInit()
	{
		super.DeferredInit();

		if (g_Game.IsServer() && !m_eMeleeCombat.eAI_GetWeapon())
		{
			m_eMeleeCombat.Reset(null, EMeleeHitType.NONE, false);

		#ifdef EXPANSION_AI_MELEEDBG_CHATTY
			ExpansionStatic.MessageNearPlayers(GetPosition(), 100, ToString() + " DeferredInit - melee weapon (in hands) null range " + m_eMeleeCombat.eAI_GetRange());
		#endif
		}
	}

	override void EEOnCECreate()
	{
	#ifdef EXTRACE_DIAG
		auto trace = EXTrace.Start(EXTrace.AI, this);
	#endif

		super.EEOnCECreate();

		ExpansionHumanLoadout.Apply(this, "FreshSpawnLoadout");
		SetMovementSpeedLimits(2, 3);
		GetGroup().SetWaypointBehaviour(eAIWaypointBehavior.ROAMING);
	}

	override void OnDebugSpawn()
	{
	#ifdef EXTRACE_DIAG
		auto trace = EXTrace.Start(EXTrace.AI, this);
	#endif

		ExpansionHumanLoadout.Apply(this, "FreshSpawnLoadout");
		SetMovementSpeedLimits(2, 3);
		GetGroup().AddWaypoint(ExpansionMath.GetRandomPointInCircle(GetPosition(), 5.0));
	}

	override void OnCEUpdate()
	{
		super.OnCEUpdate();

		int persistentCount = eAIGroup.s_PersistentGroups.Count();

		if (persistentCount > 0)
		{
			int time = g_Game.GetTickTime();

			if (time - s_eAI_LastCEUpdateTime > 3.0)
			{
				int lastPersistIndex = persistentCount - 1;
	
				if (eAIGroup.s_eAI_CurrentPersistIndex > lastPersistIndex)
					eAIGroup.s_eAI_CurrentPersistIndex = lastPersistIndex;
				else if (eAIGroup.s_eAI_CurrentPersistIndex == lastPersistIndex)
					eAIGroup.s_eAI_CurrentPersistIndex = 0;
				else
					eAIGroup.s_eAI_CurrentPersistIndex += 1;

				s_eAI_LastCEUpdateTime = time;
			}
		}

		if (!IsAlive())
			return;

		eAIGroup group = GetGroup();
		if (group && group.m_Persist && group.m_BaseName && group.GetFormationLeader() == this)
		{
			int idx = eAIGroup.s_PersistentGroups.Find(group);

#ifdef EXTRACE
			auto trace = EXTrace.Start(EXTrace.AI, this, "persistent group '" + group.GetName() + "' index " + idx + " current " + eAIGroup.s_eAI_CurrentPersistIndex);
#endif 
			
			if (idx != eAIGroup.s_eAI_CurrentPersistIndex)
				return;

#ifdef EXTRACE
#endif 
			
			group.Save(true);

		//#ifdef DIAG_DEVELOPER
			//ExpansionNotification("EXPANSION AI", "Saved group " + group.GetName()).Info();
		//#endif

		/*
			if (IsAlive())
			{
				string basePath = group.GetStorageDirectory();
				if (FileExist(basePath) || ExpansionStatic.MakeDirectoryRecursive(basePath))
				{
					string path = basePath + m_eAI_GroupMemberID.ToString() + ".bin";
					ExpansionEntityStorageModule.SaveToFile(this, path);
				}
			}
		*/
		}
	}

	override void InsertAgent(int agent, float count = 1)
	{
		switch (agent)
		{
			case eAgents.WOUND_AGENT:
				return;
		}

		super.InsertAgent(agent, count);
	}

	void eAI_Recreate()
	{
		InventoryLocation loc = new InventoryLocation();

		if (GetInventory().GetCurrentInventoryLocation(loc))
		{
			SetPosition(vector.Zero);

			EntityAI clone = ExpansionItemSpawnHelper.Clone(this, true, loc);

			if (clone)
			{
				eAIBase ai;
				if (Class.CastTo(ai, clone))
				{
					eAIGroup group = GetGroup();

					ai.SetGroup(group, true, group.GetIndex(this));

				/*
					foreach (string name, eAIAttributeBase attribute: eAIAttributeBase.s_Attributes)
					{
						attribute.Sync(this, ai);  //! Crashes for some people?!?
					}
				*/

					ai.m_eAI_BuildingWithLadder = m_eAI_BuildingWithLadder;
					ai.m_eAI_LadderLoops = m_eAI_LadderLoops;
					ai.m_eAI_Ladders = m_eAI_Ladders;
					ai.m_eAI_LastClimbedBuildingWithLadder = m_eAI_LastClimbedBuildingWithLadder;
					ai.m_eAI_LastClimbedLadder = m_eAI_LastClimbedLadder;
					ai.eAI_SetSniperProneDistanceThreshold(m_eAI_SniperProneDistanceThreshold);
					ai.Expansion_SetCanBeLooted(m_Expansion_CanBeLooted);
					ai.Expansion_SetFormerGroup(Expansion_GetFormerGroup());
					ai.m_eAI_DespawnOnLoosingAggro = m_eAI_DespawnOnLoosingAggro;
					ai.SetMovementSpeedLimits(m_eAI_SpeedLimitPreference, GetMovementSpeedLimitUnderThreat());
					ai.m_eAI_RoamingMovementSpeedLimit = m_eAI_RoamingMovementSpeedLimit;
					ai.m_eAI_RecentlyDroppedItems = m_eAI_RecentlyDroppedItems;
					ai.m_eAI_LastDroppedItem = m_eAI_LastDroppedItem;
					ai.m_eAI_DamageMultiplier = m_eAI_DamageMultiplier;
					ai.m_eAI_DamageReceivedMultiplier = m_eAI_DamageReceivedMultiplier;
					ai.m_eAI_AccuracyMin = m_eAI_AccuracyMin;
					ai.m_eAI_AccuracyMax = m_eAI_AccuracyMax;
					ai.m_eAI_ThreatOverride = m_eAI_ThreatOverride;
					ai.m_eAI_MemeLevel = m_eAI_MemeLevel;
					ai.m_eAI_NoiseInvestigationDistanceLimit = m_eAI_NoiseInvestigationDistanceLimit;
					ai.eAI_SetUnlimitedReload(m_eAI_UnlimitedReload);
					ai.m_eAI_GroupMemberID = m_eAI_GroupMemberID;
					ai.m_eAI_LootingBehavior = m_eAI_LootingBehavior;
					ai.m_eAI_ThreatDistanceLimit = m_eAI_ThreatDistanceLimit;

				#ifdef JM_COT
					foreach (PlayerBase spectator: m_eAI_Spectators)
					{
						GetDayZGame().GetExpansionGame().SpectateAI(spectator.GetIdentity(), ai, null);
					}
				#endif

					//! Since threat overrides point to same instance on original and cloned AI,
					//! we have to sever this connection before original AI deletion because
					//! threat overrides are cleared on AI destruction
					m_eAI_ThreatOverride = new eAIThreatOverride;

					g_Game.ObjectDelete(this);

					return;
				}
				else
				{
					clone.SetPosition(vector.Zero);
					g_Game.ObjectDelete(clone);
				}
			}

			SetPosition(loc.GetPos());
		}
	}

#ifdef DIAG_DEVELOPER
	ref map<EntityAI, bool> m_eAI_PlayerIsEnemyStatus = new map<EntityAI, bool>;
	void eAI_UpdatePlayerIsEnemyStatus(EntityAI player, bool state, string reason)
	{
		bool currentState;
		if (!m_eAI_PlayerIsEnemyStatus.Find(player, currentState) || state != currentState)
		{
			m_eAI_PlayerIsEnemyStatus[player] = state;
		}
	}
#endif

	/**
	 * @brief check if other player is currently considered an enemy
	 * 
	 * @param other Other player entity
	 * @param track Track movement (will return true also for friendly targets)
	 * @param[out] isPlayerMoving Is other player moving?
	 * @param[out] friendly Are we normally friendly towards other player?
	 * @param[out] targeted Are we currently targeting/targeted by other player? (our threat to them or theirs to us above 0.2)
	 */
	bool PlayerIsEnemy(EntityAI other, bool track = false, out bool isPlayerMoving = false, out bool friendly = false, out bool targeted = false)
	{
#ifdef EAI_TRACE
#endif

		PlayerBase player;
		if (!Class.CastTo(player, other))
		{
#ifdef DIAG_DEVELOPER
			eAI_UpdatePlayerIsEnemyStatus(player, true, "target not a player");
#endif
			return true;
		}

		if (player.Expansion_GetActualVelocity().LengthSq() > 0 || player.IsClimbing() || player.IsFalling() || player.IsFighting() || player.IsLeaning())
			isPlayerMoving = true;

		if (GetGroup().GetFaction().IsObserver())
		{
			friendly = true;

			//! Interacting players are always "enemies" to observers (will be looked at)
			if (m_eAI_InteractingPlayers.Find(player) > -1)
				return true;

			//! Look at others if they move
			return isPlayerMoving;
		}

		//! Are we targeting them and aggro?
		if (eAI_GetCachedThreat(player.GetTargetInformation()) > 0.2)
			targeted = true;

		if (player.GetGroup())
		{
			if (player.GetGroup() == GetGroup())
			{
#ifdef DIAG_DEVELOPER
				eAI_UpdatePlayerIsEnemyStatus(player, false, "target has same group");
#endif
				friendly = true;

				if (track && GetExpansionSettings().GetAI().MemeLevel > 0 && isPlayerMoving)
					return true;

				return false;
			}

			//! If we are not targeting them, are they targeting us?
			eAIBase ai;
			float ourThreatToThem;
			if (Class.CastTo(ai, player))
				ourThreatToThem = ai.eAI_GetCachedThreat(GetTargetInformation());
			if (!targeted && !player.GetGroup().GetFaction().IsObserver() && ourThreatToThem > 0.2)
				targeted = true;

			//! Other faction friendly to our faction or this specific AI?
			if (player.GetGroup().GetFaction().IsFriendly(GetGroup().GetFaction()) || player.GetGroup().GetFaction().IsFriendlyEntity(this, player))
			{
#ifdef DIAG_DEVELOPER
				if (eAI_IsPassive())
					eAI_UpdatePlayerIsEnemyStatus(player, targeted, "passive | target is friendly " + player.GetGroup().GetFaction());
				else
					eAI_UpdatePlayerIsEnemyStatus(player, targeted, "target is friendly " + player.GetGroup().GetFaction());
#endif
				friendly = true;

				if (track && GetExpansionSettings().GetAI().MemeLevel > 0 && isPlayerMoving)
					return true;

				return targeted;
			}

			//! If the other player is a guard and we are not, consider them an enemy if they raised their weapon and
			//! either aren't AI or consider us a threat
			if (player.GetGroup().GetFaction().IsGuard())
			{
				bool hostile;
				//! https://feedback.bistudio.com/T173348
				if ((player.IsRaised() || player.IsFighting()) && (!ai || ourThreatToThem >= 0.4))
					hostile = true;
#ifdef DIAG_DEVELOPER
				eAI_UpdatePlayerIsEnemyStatus(player, hostile, "target is guard");
#endif
				return hostile;
			}
		}
		else
		{
			if (!player.GetIdentity() && !player.CanBeTargetedByAI(this))
			{
				if (track && GetExpansionSettings().GetAI().MemeLevel > 0 && isPlayerMoving)
					return true;

				return targeted;
			}
		}

		//! Our faction fiendly to specific player?
		if (GetGroup().GetFaction().IsFriendlyEntity(other, this))
		{
#ifdef DIAG_DEVELOPER
			if (eAI_IsPassive())
				eAI_UpdatePlayerIsEnemyStatus(player, targeted, "passive | friendly | target has no group");
			else
				eAI_UpdatePlayerIsEnemyStatus(player, targeted, "friendly");
#endif
			friendly = true;

			if (track && GetExpansionSettings().GetAI().MemeLevel > 0 && isPlayerMoving)
				return true;

			return targeted;
		}

		// at this point we know both we and they have groups, and the groups aren't friendly towards each other
#ifdef DIAG_DEVELOPER
		eAI_UpdatePlayerIsEnemyStatus(player, true, "not friendly");
#endif
		return true;
	}



	void BQ_EAIWeaponTrace(string tag, string details = "", bool force = false)
	{
		return;
	}


	bool BQ_EAIWeaponIsChambered(Weapon_Base weapon)
	{
		if (!weapon)
			return false;

		int muzzle = weapon.GetCurrentMuzzle();
		if (weapon.IsChamberEmpty(muzzle))
			return false;
		if (weapon.IsChamberFiredOut(muzzle))
			return false;

		return true;
	}

	bool BQ_EAIWeaponNeedsReload(Weapon_Base weapon, inout Magazine magazine)
	{
		magazine = null;
		if (!weapon)
			return false;

		int muzzle = weapon.GetCurrentMuzzle();

		if (!BQ_EAIWeaponIsChambered(weapon))
			return true;

		Magazine currentMag = Magazine.Cast(weapon.GetMagazine(muzzle));
		if (currentMag && currentMag.GetAmmoCount() <= 0)
			return true;

		if (!currentMag && !weapon.HasInternalMagazine(muzzle))
			return true;

		if (weapon.HasInternalMagazine(muzzle))
		{
			if (weapon.GetInternalMagazineCartridgeCount(muzzle) <= 0)
				return true;
		}

		return false;
	}

	void BQ_EAIRequestReload(Weapon_Base weapon, Magazine magazine = null)
	{
		BQExpansionDiag.Log("RELOAD_REQ", "bot=" + ToString() + " weapon=" + BQExpansionDiag.Obj(weapon) + " mag=" + BQExpansionDiag.Obj(magazine));
		BQBot_SetVanillaTriggerHoldWanted(false);
		BQBot_SetCombatPoseWanted(false);
		BQBot_MarkPostReloadSyncNeeded("eAI_reload_state");
	}

	void BQ_EAIRequestUnjam(Weapon_Base weapon)
	{
		BQExpansionDiag.Log("UNJAM_REQ", "bot=" + ToString() + " weapon=" + BQExpansionDiag.Obj(weapon));
		BQBot_SetVanillaTriggerHoldWanted(false);
		BQBot_SetCombatPoseWanted(false);

		if (weapon && weapon.IsJammed())
			weapon.SetJammed(false);

		BQBot_MarkPostReloadSyncNeeded("eAI_unjam_state");
	}

	bool BQ_EAIItemIsDanger(ItemBase item)
	{
		if (!item)
			return false;

		string type = item.GetType();
		if (type.Contains("Grenade"))
			return true;
		if (type.Contains("Mine"))
			return true;
		if (type.Contains("Explosive"))
			return true;
		if (type.Contains("Trap"))
			return true;
		if (type.Contains("Claymore"))
			return true;

		return false;
	}

	float BQ_EAIGetMinSafeFiringDistance(Weapon_Base weapon)
	{
		if (!weapon)
			return 0.0;

		if (weapon.ShootsExplosiveAmmo())
			return 12.0;

		return 0.0;
	}

	bool BQ_EAIIsExplosiveTarget(eAITarget target)
	{
		if (!target)
			return false;

		EntityAI entity = target.GetEntity();
		ItemBase item;
		if (Class.CastTo(item, entity))
			return BQ_EAIItemIsDanger(item);

		return false;
	}

	bool BQ_EAIHasFriendlyInBlastRadius(eAITarget target, float radius)
	{
		if (!target || radius <= 0.0)
			return false;

		vector center = target.GetPosition() + target.GetAimOffset();
		float radiusSq = radius * radius;

		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			EntityAI entity = EntityAI.Cast(man);
			if (!entity || entity == this)
				continue;

			if (vector.DistanceSq(entity.GetPosition(), center) > radiusSq)
				continue;

			if (!PlayerIsEnemy(entity))
				return true;
		}

		return false;
	}

	void TryFireWeapon()
	{
		if (!GetGame() || !GetGame().IsServer())
		{
			BQExpansionDiag.Log("TRY_FIRE", "FAIL reason=no_game_or_not_server bot=" + ToString());
			return;
		}

		Weapon_Base weapon = Weapon_Base.Cast(GetHumanInventory().GetEntityInHands());
		if (!weapon)
		{
			BQExpansionDiag.Log("TRY_FIRE", "FAIL reason=no_weapon bot=" + ToString());
			return;
		}

		if (g_Game.GetTime() < m_eAI_MinTimeTillNextFire)
		{
			BQExpansionDiag.Log("TRY_FIRE", "FAIL reason=cooldown bot=" + ToString() + " now=" + g_Game.GetTime().ToString() + " minNext=" + m_eAI_MinTimeTillNextFire.ToString());
			return;
		}

		if (!eAI_CanFire(weapon))
		{
			BQExpansionDiag.Log("TRY_FIRE", "FAIL reason=eAI_CanFire_false bot=" + ToString() + " weapon=" + weapon.ToString());
			return;
		}

		eAITarget target = GetTarget();
		if (!target || !target.GetEntity())
		{
			BQExpansionDiag.Log("TRY_FIRE", "FAIL reason=no_target bot=" + ToString());
			return;
		}

		if (!target.m_LOS && !eAI_HasLOS())
		{
			string noLosLog = "FAIL reason=no_los bot=" + ToString();
			noLosLog = noLosLog + " target=" + target.GetEntity().ToString();
			noLosLog = noLosLog + " targetLOS=" + BQExpansionDiag.B(target.m_LOS);
			noLosLog = noLosLog + " aiLOS=" + BQExpansionDiag.B(eAI_HasLOS());
			BQExpansionDiag.Log("TRY_FIRE", noLosLog);
			return;
		}

		vector aimPosition = target.GetPosition(false) + target.GetAimOffset();
		string tryFireOk = "SUCCESS bot=" + ToString();
		tryFireOk = tryFireOk + " weapon=" + weapon.ToString();
		tryFireOk = tryFireOk + " target=" + target.GetEntity().ToString();
		tryFireOk = tryFireOk + " aim=" + aimPosition.ToString();
		tryFireOk = tryFireOk + " distSq=" + m_eAI_DistanceToTargetSq.ToString();
		BQExpansionDiag.Log("TRY_FIRE", tryFireOk);
		BQBot_SetAimTarget(aimPosition);
		BQBot_SetCombatPoseWanted(true);
		BQBot_SetFireIntentMode(2); // single-shot intent; BQ firebridge keeps projectile/sound path
		BQBot_SetVanillaTriggerHoldWanted(false);
		BQBot_RequestVanillaFire();
		BQBot_MarkTrigger();

		m_eAI_LastFireTime = g_Game.GetTime();
		m_eAI_MinTimeTillNextFire = m_eAI_LastFireTime + Math.RandomIntInclusive(200, 300);
	}


	/**
	 * @brief select firemode based on target, distance, and weapon
	 * 
	 * @note currently only deals with double barrel (i.e. Blaze and BK-43) as there is no real need to ever change firemode on any other gun
	 * because when gun enters AI hands, firemode is set to fullauto (e.g. M4A1) or burst (e.g. M16) if supported.
	 * The firing code in TryFireWeapon deals with the amount of, and time between, shots fired in fullauto and burst modes,
	 * which gives the appearance of semi-auto in those modes if only one shot is queued.
	 */
	void eAI_SelectFireMode(eAITarget target, float distSq, Weapon_Base weapon)
	{
		bool changedFireMode;

		if (weapon.m_Expansion_WeaponInfo.m_FireModes.Contains(ExpansionFireMode.Double))
		{
			if (distSq <= 30 && target.IsPlayer())
				weapon.Expansion_SetFireMode(ExpansionFireMode.Double, changedFireMode);
			else
				weapon.Expansion_SetFireMode(ExpansionFireMode.Single, changedFireMode);

			m_eAI_QueuedShots = 1;  //! @note for firemode double, one queued shot is two projectiles
		}

		if (changedFireMode)
			m_eAI_MinTimeTillNextFire = g_Game.GetTime() + Math.RandomIntInclusive(200, 300);
	}

	bool eAI_CanFire(Weapon_Base weapon)
	{
		string bqWeaponType = "none";
		if (weapon)
			bqWeaponType = weapon.GetType();
		BQ_EAIWeaponTrace("eAI_CanFire_CALLED", "weapon=" + bqWeaponType);
		if (IsClimbing() || IsFalling() || IsFighting() || IsSwimming())
			return false;

		if (GetDayZPlayerInventory().IsProcessing())
			return false;

		if (!IsRaised())
			return false;

		if (!weapon.CanFire())
			return false;

		if (GetWeaponManager().IsRunning())
			return false;

		int muzzleIndex = weapon.GetCurrentMuzzle();
		if (!weapon.CanFire(muzzleIndex))
			return false;

		return true;
	}

	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		//! Detect improper 3rd party mod damage handling
		if (!dmgZone && source && source.IsDayZCreature())
		{
			IEntity child = source.GetChildren();
			while (child)
			{
				Weapon weapon;
				if (Class.CastTo(weapon, child))
				{
					//! Set source to weapon to prevent improper 3rd party mod damage handling
					source = weapon;
					break;
				}

				child = child.GetSibling();
			}
		}

		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		m_eAI_FlankTime = 0.0;

		if (source && source.IsDayZCreature() && m_eAI_ThreatOverride.Contains(source))
			eAI_ThreatOverride(source, false);

		//! If we are currently blocking, go out of block so we can start going on offense
		if (m_eAI_MeleeFightLogic.IsInBlock())
			m_eAI_MeleeFightLogic.eAI_EndBlock();
	}

	override void EEHitByRemote(int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
	{
		if (!g_Game.IsDedicatedServer())
			super.EEHitByRemote(damageType, source, component, dmgZone, ammo, modelPos);
	#ifdef DIAG_DEVELOPER
		else if (m_MeleeFightLogic.IsInBlock())
			EXError.Warn(this, "EEHitByRemote called on server while in block");
	#endif
	}

	override void EEKilled(Object killer)
	{
	#ifdef EXTRACE_DIAG
		auto trace = EXTrace.Start(EXTrace.AI, this, "" + killer);
	#endif

		super.EEKilled(killer);

		if (s_eAI_Alive)
			s_eAI_Alive.Remove(m_eAI_Alive_Node);

		if (g_Game.IsServer())
		{
			if (!m_Expansion_EmoteAutoCancel)
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(Expansion_PlayEmote);

			eAIGroup group = GetGroup();
			if (group && group.m_Persist && group.m_BaseName)
				eAI_DeletePersistentFiles();
		}

		if (m_eAI_LootDropOnDeath)
		{
			foreach (ExpansionPrefab lootDropOnDeath: m_eAI_LootDropOnDeath)
			{
				if (lootDropOnDeath.CanSpawn())
					Object obj = lootDropOnDeath.Spawn(ExpansionMath.GetRandomPointInCircle(GetPosition(), 0.5), vector.Zero);
			}
		}
	}

	override protected void eAI_Cleanup(bool autoDeleteGroup = false)
	{
	#ifdef EXTRACE_DIAG
		auto trace = EXTrace.Start(EXTrace.AI, this);
	#endif

		array<ref eAITargetInformation> targets = {};
		foreach (eAITargetInformation info, eAITarget state: m_eAI_TargetInformationStates)
		{
			if (info)
				targets.Insert(info);
		}

		foreach (eAITargetInformation target: targets)
		{
			target.RemoveAI(this);
		}

		if (m_eAI_TargetInformationStates.Count())
		{
			eAI_DebugTargetInformationStates();

			m_eAI_TargetInformationStates.Clear();
		}

		super.eAI_Cleanup(autoDeleteGroup);

		eAIGroup group = GetGroup();
		if (group)
			group.SetIsInCombat(false);

		if (g_Game.IsServer() && ((!IsDamageDestroyed() && autoDeleteGroup) || (IsDamageDestroyed() && !autoDeleteGroup)))
		{
			s_Expansion_AllPlayers.m_OnRemove.Remove(eAI_OnRemovePlayer);
			eAICustomCreatureTargetInformation.s_AllCustomCreatures.m_OnRemove.Remove(eAI_OnRemoveCustomCreature);
		}

		if (m_eAI_ClientUpdateTimer && m_eAI_ClientUpdateTimer.IsRunning())
			m_eAI_ClientUpdateTimer.Stop();

		if (g_Game.IsServer())
			eAINoiseSystem.SI_OnNoiseAdded.Remove(eAI_OnNoiseEvent);

		if (m_eAI_CurrentCoverObject)
			s_eAI_TakenCoverObjects.RemoveItem(m_eAI_CurrentCoverObject);
	}

	void eAI_DebugTargetInformationStates()
	{
	}


	override bool IsAI()
	{
		return true;
	}

	override bool Expansion_IsAI()
	{
		return true;
	}

#ifndef EAI_USE_LEGACY_PATHFINDING
	ExpansionPathHandler GetPathFinding()
#else
	eAIPathFinding GetPathFinding()
#endif
	{
		return m_PathFinding;
	}

	eAIFSM GetFSM()
	{
		return m_FSM;
	}

	bool eAI_IsItemObstructed(ItemBase item)
	{
		vector begPos;
		MiscGameplayFunctions.GetHeadBonePos(this, begPos);
		vector endPos = item.GetCenter();

		//! @note cannot use IsObjectObstructedFilterEx
		//! (uses vanilla ItemBase::CanObstruct which will always return true on server as it is only meant to be called on client because it relies on g_Game.GetPlayer)
		//m_eAI_IsItemObstructedCache.ClearCache();
		//m_eAI_IsItemObstructedCache.RaycastStart = begPos;
		//m_eAI_IsItemObstructedCache.ObjectCenterPos = endPos;

		//if (MiscGameplayFunctions.IsObjectObstructedFilterEx(item, m_eAI_IsItemObstructedCache, this))
			//return true;

		vector contactPos, contactDir;
		int contactComponent;
		set<Object> results = new set<Object>;

		if (DayZPhysics.RaycastRV(begPos, endPos, contactPos, contactDir, contactComponent, results, item, this, false, false, ObjIntersectFire, 0.0, CollisionFlags.ALLOBJECTS))
		{
			if (results.Count() == 0)
				return true;

			EntityAI entity;

			foreach (Object obj: results)
			{
				if (Class.CastTo(entity, obj))
					obj = entity.GetHierarchyRoot();

				if (ExpansionStatic.CanObstruct(obj))
					return true;
			}
		}

		return false;
	}

	bool Expansion_GetUp(bool force = false)
	{
		if (!force)
		{
			int stanceMask;
			eAIStance defaultStance;

			if (m_eAI_DefaultStance > eAIStance.STANDING)
			{
				if (g_Game.GetTickTime() - m_eAI_LastHitTime > 10.0 && !m_eAI_IsPreparingMelee && GetCurrentWaterLevel() < 0.3)
					defaultStance = m_eAI_DefaultStance;
			}

			switch (defaultStance)
			{
				case eAIStance.CROUCHED:
					stanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_RAISEDCROUCH;
					break;

				case eAIStance.PRONE:
					stanceMask = DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_RAISEDPRONE;
					break;

				default:
					stanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_RAISEDERECT;
					break;
			}

			if (IsPlayerInStance(stanceMask))
				return false;
		}

		return OverrideStance(DayZPlayerConstants.STANCEIDX_ERECT, force);
	}

	//! @brief Set emote for playing
	//! Setting autoCancel to true will automatically cancel the emote if it is a static pose
	//! @note Emote will not play instantly! Will play when AI FSM can transition to PlayEmote state
	void Expansion_SetEmote(int emoteID, bool autoCancel = false, int autoCancelDelay = 0)
	{
		m_Expansion_EmoteID = emoteID;
		m_Expansion_EmoteAutoCancel = autoCancel;
		m_Expansion_EmoteAutoCancelDelay = autoCancelDelay;
	}

	void Expansion_PlayEmote()
	{
		if (!m_Expansion_EmoteID)
			return;

		if (!m_EmoteManager.IsEmotePlaying() && m_EmoteManager.CanPlayEmote(m_Expansion_EmoteID))
		{
			m_EmoteManager.PlayEmote(m_Expansion_EmoteID);
			if (m_Expansion_EmoteAutoCancel)
			{
				int delay;
				switch (m_Expansion_EmoteID)
				{
					//! These emotes are temporary poses and don't need to be canceled
					case EmoteConstants.ID_EMOTE_TAUNTKISS:
					case EmoteConstants.ID_EMOTE_TAUNTELBOW:
					case EmoteConstants.ID_EMOTE_THROAT:
					case EmoteConstants.ID_EMOTE_DANCE:
					case EmoteConstants.ID_EMOTE_DABBING:
					case EmoteConstants.ID_EMOTE_CLAP:
					case EmoteConstants.ID_EMOTE_TAUNTTHINK:
					case EmoteConstants.ID_EMOTE_MOVE:
					case EmoteConstants.ID_EMOTE_DOWN:
					case EmoteConstants.ID_EMOTE_COME:
					case EmoteConstants.ID_EMOTE_NOD:
					case EmoteConstants.ID_EMOTE_SHAKE:
					case EmoteConstants.ID_EMOTE_SHRUG:
					case EmoteConstants.ID_EMOTE_VOMIT:
						break;

					//! These emotes are static poses with looping animation
					case EmoteConstants.ID_EMOTE_SOS:
						if (m_Expansion_EmoteAutoCancelDelay)
							delay = m_Expansion_EmoteAutoCancelDelay;
						else
							delay = 4000;
						break;

					//! Assume static pose
					default:
						if (m_Expansion_EmoteAutoCancelDelay)
							delay = m_Expansion_EmoteAutoCancelDelay;
						else
							delay = 2000;
						break;
				}
				if (delay)
					g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(m_EmoteManager.ServerRequestEmoteCancel, delay);
			}
		}

		if (m_Expansion_EmoteAutoCancel)
			m_Expansion_EmoteID = 0;
	}

	//! Radians!
	override float Expansion_GetHeadingAngle()
	{
		//! XXX: Should we sync m_eAI_AimRelAngles[0] to client? Seems overkill though
		//return (m_eAI_AimRelAngles[0] - GetOrientation()[0]) * Math.DEG2RAD;
		return -GetOrientation()[0] * Math.DEG2RAD;
	}

	override vector Expansion_GetHeadingVector()
	{
		vector dir = vector.Zero;
		//! XXX: Should we sync m_eAI_AimRelAngles[0] to client? Seems overkill though
		//float headingAngle = (m_eAI_AimRelAngles[0] - GetOrientation()[0]) * Math.DEG2RAD;
		float headingAngle = -GetOrientation()[0] * Math.DEG2RAD;
		dir[0] = Math.Cos(headingAngle + Math.PI_HALF);
		dir[2] = Math.Sin(headingAngle + Math.PI_HALF);
		return dir.Normalized();
	}

	override float Expansion_GetMovementSpeed()
	{
		auto cmd = GetCommand_Move();
		if (cmd)
		{
			float speed = m_eAI_CommandMove.GetCurrentMovementSpeed();
			if (speed > 2.0 && m_WeaponRaised)
				speed = 2.0;
			return speed;
		}

		return 0.0;
	}

	override float Expansion_GetMovementAngle()
	{
		auto cmd = GetCommand_Move();
		if (cmd)
			return m_eAI_CommandMove.GetCurrentMovementAngle();

		return 0.0;
	}

	float Expansion_GetTargetMovementAngle()
	{
		auto cmd = GetCommand_Move();
		if (cmd)
			return m_eAI_CommandMove.GetTargetDirection();

		return 0.0;
	}

	bool HandleVaulting(eAICommandMove hcm, float pDt)
	{
		//if (!m_PathFinding.IsVault())
		//{
		//	return false;
		//}

		string jumpClimbStr = "false";
		if (m_PathFinding.m_IsJumpClimb)
			jumpClimbStr = "true";
		string blockedStr = "false";
		if (hcm.IsBlocked())
			blockedStr = "true";
		Print(string.Format("[EAI] [vault_attempt] bot=%1 jumpClimb=%2 blocked=%3 blockingObj=%4", ToString(), jumpClimbStr, blockedStr, hcm.GetBlockingObject()));

		if (IsRestrained())
			return false;

		if (m_eAI_PositionIsFinal && Math.Round(Expansion_GetMovementSpeed()) == 0.0)
			return false;

		if (!IsSwimming() && !m_PathFinding.m_AllowJumpClimb && !m_PathFinding.m_IsBlockedPhysically)
			return false;

		if (Math.AbsFloat(ExpansionMath.AngleDiff2(GetOrientation()[0], m_PathFinding.m_PathSegmentDirection.VectorToAngles()[0])) > 45.0)
			return false;

		if (m_WeaponManager.IsRunning())
			return false;

		Object blockingObject = hcm.GetBlockingObject();

		bool isBlockingItem;
		bool climbFloatingItem;
		bool isBlockingVehicle;

		//! Offset is because AI, like players, can just run over small height differences unless walking slowly
		//! 0.49 instead of 0.5 because of platform2_wall.p3d (e.g. at <1057.06, 157.539, 6692.51> in Camp Metalurg on Chernarus)
		float offset = 0.49;

		//! Superjanky but allows climbing floating items with collision, e.g. Expansion basebuilding floors,
		//! while preventing unwanted climbing on other items.
		if (blockingObject)
		{
			if (blockingObject.IsTransport())
			{
				isBlockingVehicle = true;
			}
			else if (blockingObject.IsInventoryItem() && ExpansionStatic.CanObstruct(blockingObject))
			{
				isBlockingItem = true;

				if (GetPosition()[1] + offset < blockingObject.GetPosition()[1] || IsSwimming() || m_MovementState.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_WALK)
					climbFloatingItem = true;
			}
		}

		//! If blocking object is NOT a floating item that should be climbed/jumped and is NOT a vehicle...
		if (!climbFloatingItem && !isBlockingVehicle)
		{
			//! ...don't allow jump/climb if blocking object is an item
			if (isBlockingItem)
				return false;

			//! If pathfinding doesn't indicate jump/climb...
			if (!m_PathFinding.m_IsJumpClimb)
			{
				//! ...don't allow jump/climb if movement NOT blocked
				if (!hcm.IsBlocked())
					return false;

				//! ...don't allow jump/climb if NOT on inverse path and NOT using underwater surface for pathfinding
				//! and NOT blocked by sea ice
				if (m_PathFinding.m_PathGlueIdx == -1 && !eAI_ShouldUseSurfaceUnderWaterForPathFinding())
				{
					//! @note if not using underwater surface for pathfinding, we're also not swimming but might be in shallow water
					if (!eAI_IsSeaIce(blockingObject))
						return false;
				}
			}
		}

#ifdef EXTRACE_DIAG
		auto trace = EXTrace.Profile(EXTrace.AI_PROFILE, this, "CommandHandler(14) -> HandleVaulting");
#endif

		SHumanCommandClimbSettings hcls = GetDayZPlayerType().CommandClimbSettingsW();
		
		if ( m_MovementState.m_iMovement != DayZPlayerConstants.MOVEMENTIDX_IDLE )
			hcls.m_fFwMaxDistance = 2.5;
		else
			hcls.m_fFwMaxDistance = 1.2;
		
		if (m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_SWIM)
			hcls.m_fBackwardsCheckDist = 0.35;
		else
			hcls.m_fBackwardsCheckDist = 0;

		SetOrientation(GetOrientation());

		//! These two NEED to be reset to zero vector else they may carry previous values if new result climb/climbover differs from previous!
		//! Other SHumanCommandClimbResult values are set appropriately by DoClimbTest
		m_ExClimbResult.m_ClimbOverStandPoint = vector.Zero;
		m_ExClimbResult.m_ClimbStandPoint = vector.Zero;

		HumanCommandClimb.DoClimbTest(this, m_ExClimbResult, 0);

		if (m_ExClimbResult.m_bIsClimb || m_ExClimbResult.m_bIsClimbOver)
		{
			m_eAI_LastSuccessfulClimbTestResult = m_ExClimbResult;

			//! AI, like players, can just run over small height differences unless walking slowly (avoids awkwardly climbing stairs)
			//! @note hcls.m_fFwMinHeight cannot be used for this since it is too high (0.7)
			//! @note this check is only needed for vanilla DoClimbTest, ExpansionClimb::DoClimbTest checks height internally
			if ((m_ExClimbResult.m_fClimbHeight < offset && !IsSwimming() && m_MovementState.m_iMovement != DayZPlayerConstants.MOVEMENTIDX_WALK) || m_ExClimbResult.m_fClimbHeight > hcls.m_fFwMaxHeight)
				return false;

			return true;
		}

		//! As we are essentially using Zombie pathfinding, we may encounter situations where the path will go through a fence
		//! that Zs would be able to jump (e.g. wall_indfnc_9.p3d) but player AI would not due to HumanCommandClimb.DoClimbTest not letting us.
		//! Use ExpansionClimb.DoClimbTest with alwaysAllowClimb = true instead.
		if (m_PathFinding.m_DoClimbTestEx)
		{
			ExpansionClimb.DoClimbTest(this, m_ExClimbResult, true);
			if (m_ExClimbResult.m_bIsClimb || m_ExClimbResult.m_bIsClimbOver)
			{
				m_eAI_LastSuccessfulClimbTestResult = m_ExClimbResult;
				return true;
			}
		}

		return false;
	}

	override bool ProcessJumpOrClimb(float pDt, int pCurrentCommandID)
	{
		//if (GetIdentity())
			//return super.ProcessJumpOrClimb(pDt, pCurrentCommandID);

		return false;
	}

	override bool CanJump()
	{
		//! Following is equivalent to vanilla PlayerBase::CanJump

		if (GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS)
		{	
			return false;
		}
		
		if (!CanConsumeStamina(EStaminaConsumers.JUMP))
			return false;

		//! disables jump when player is significantly injured
		if (m_InjuryHandler && m_InjuryHandler.GetInjuryAnimValue() >= InjuryAnimValues.LVL3)
			return false;
		
		if (IsInFBEmoteState() || m_EmoteManager.m_MenuEmote)
		{
			return false;
		}

		//! Following is roughly equivalent to vanilla DayZPlayerImplement::CanJump

		if (IsFBSymptomPlaying() || IsRestrained() || IsUnconscious())
			return false;
		
		if (m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
			return false;
		
		HumanItemBehaviorCfg hibcfg = GetItemAccessor().GetItemInHandsBehaviourCfg();
		if (!hibcfg.m_bJumpAllowed)
			return false;
		
		if (!DayZPlayerUtils.PlayerCanChangeStance(this, DayZPlayerConstants.STANCEIDX_ERECT) || !DayZPlayerUtils.PlayerCanChangeStance(this, DayZPlayerConstants.STANCEIDX_RAISEDERECT))
			return false;
		
		return true;
	}

	override bool CanClimb(int climbType, SHumanCommandClimbResult climbRes)
	{
		//! Following is roughly equivalent to vanilla PlayerBase::CanClimb

		if (GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
			
		if (climbType == 1 && !CanConsumeStamina(EStaminaConsumers.VAULT))
			return false;	
		
		//! vault from water allowed - only check if not swimming
		if (!IsSwimming())
		{
			if (climbType == 2 && (!CanConsumeStamina(EStaminaConsumers.CLIMB) || GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS))
				return false;

			if (climbType > 0 && m_InjuryHandler && m_InjuryHandler.GetInjuryAnimValue() >= InjuryAnimValues.LVL3)
				return false;
		}

		//! Following is roughly equivalent to vanilla DayZPlayerImplement::CanClimb

		if (IsFBSymptomPlaying() || IsRestrained() || IsUnconscious() || IsInFBEmoteState())
			return false;
		
		if (m_MovementState.IsInProne() || m_MovementState.IsInRaisedProne())
			return false;

		HumanItemBehaviorCfg hibcfg = GetItemAccessor().GetItemInHandsBehaviourCfg();
		if (!hibcfg.m_bJumpAllowed)
			return false;

		if (climbRes)
		{
			if (!eAI_CanClimbOn(climbRes.m_GrabPointParent, climbRes))
				return false;
			if (!eAI_CanClimbOn(climbRes.m_ClimbStandPointParent, climbRes))
				return false;
			if (!eAI_CanClimbOn(climbRes.m_ClimbOverStandPointParent, climbRes))
				return false;

			IEntity standPointParent;
			vector standPoint;
			vector checkPosition;
			vector checkDirection;
			vector unitPosition = GetPosition();

			if (climbRes.m_ClimbOverStandPoint != vector.Zero)
			{
				standPointParent = climbRes.m_ClimbOverStandPointParent;
				standPoint = climbRes.m_ClimbOverStandPoint;
			}
			else if (climbRes.m_ClimbStandPoint != vector.Zero)
			{
				standPointParent = climbRes.m_ClimbStandPointParent;
				standPoint = climbRes.m_ClimbStandPoint;
			}
			else
			{
				checkDirection = GetDirection() * 1.5;
			}

			if (standPoint != vector.Zero)
			{
				Object standPointObject;
				if (Class.CastTo(standPointObject, standPointParent))
				{
					//! Standpoint parent is object, local coordinates unequal world coordinates
					vector transform[4];
					standPointParent.GetTransform(transform);
					checkPosition = standPoint.Multiply4(transform);
				}
				else
				{
					//! No standpoint parent or part of world, local coordinates equal world coordinates
					checkPosition = standPoint;
				}
				checkPosition[1] = unitPosition[1];
				checkDirection = vector.Direction(unitPosition, checkPosition);
			}

			vector surfacePosition = ExpansionStatic.GetSurfaceRoadPosition(unitPosition + checkDirection);
			//! Swim start water level = 1.5 m, see DayZPlayerUtils::CheckWaterLevel
			if (!m_PathFinding.m_IsSwimmingEnabled && g_Game.GetWaterDepth(surfacePosition) > 1.5)
			{
			#ifdef DIAG_DEVELOPER
				Expansion_DebugObject(1344, unitPosition + checkDirection, "ExpansionDebugRodBig_Blue", checkDirection, surfacePosition);
			#endif
				return false;
			}
		#ifdef DIAG_DEVELOPER
			else
			{
				Expansion_DebugObject(1344, "0 0 0", "ExpansionDebugRodBig_Blue");
			}
		#endif

			if (!IsSwimming() || climbRes.m_bIsClimbOver)
			{
				float heightThresh;

				if (climbRes.m_bIsClimbOver)
					heightThresh = 1.0;  //! should prevent unwanted vaulting of stair-rails
				else
					heightThresh = DayZPlayerImplementFallDamage.HEALTH_HEIGHT_LOW - climbRes.m_fClimbHeight;

				if (!eAI_IsFallSafe(checkDirection, false, heightThresh, false))
					return false;
			}
		}

		return true;
	}

	bool eAI_CanClimbOn(IEntity parent, SHumanCommandClimbResult climbRes)
	{
		EntityAI entity;
		if (Class.CastTo(entity, parent) && entity.IsHologram())
			return false;

		Object object;
		bool isPathEndPointCollidingObject;
		if (Class.CastTo(object, parent) && ExpansionStatic.IsColliding(object, m_PathFinding.GetEnd()))
			isPathEndPointCollidingObject = true;

		if ((m_WeaponRaised || (m_eAI_IsFightingFSM && !IsSwimming())) && !isPathEndPointCollidingObject && !climbRes.m_bIsClimbOver && !m_PathFinding.m_IsBlockedPhysically)
			return false;

		if (object)
		{ 
			if (object.IsTree() || object.IsBush() || object.IsMan())
				return false;

			BuildingBase building;
			if (Class.CastTo(building, object))
			{
				if (building.m_eAI_PreventClimb || building.IsInherited(ExpansionDebugObject))
					return false;

				if (climbRes.m_bIsClimbOver)
				{
					//! This should prevent AI from vaulting open doors...
					if (climbRes.m_fClimbHeight >= 1.7)
						return false;

					//! Don't allow vaulting unenterable building unless path endpoint is within building or we are within building
					if (!building.Expansion_IsEnterable() && !isPathEndPointCollidingObject && !ExpansionStatic.IsColliding(object, GetPosition()))
					{
						auto move = GetCommand_Move();
						if (!move || !m_eAI_CommandMove.IsBlocked())
						{
							if (EXTrace.AI)
								EXTrace.Print(true, this, "eAI_CanClimbOn false " + Debug.GetDebugName(parent) + " is scenery? " + object.IsScenery() + " is plain? " + object.IsPlainObject());

							return false;
						}
					}
				}
			}
			else if (object.IsTransport() || object.IsInventoryItem())
			{
				if (climbRes.m_bIsClimbOver && !isPathEndPointCollidingObject && !ExpansionStatic.IsColliding(object, GetPosition()))
					return false;
			}
			else if (!object.IsScenery())
			{
				//! This should prevent AI from vaulting open gates unless blocked...
				if (climbRes.m_bIsClimbOver && climbRes.m_fClimbHeight >= 1.7 && (!m_PathFinding.m_IsBlocked || !m_PathFinding.m_IsJumpClimb))
					return false;

				string debugName = object.GetDebugName();
				debugName.ToLower();

				//! Things we should never attempt to vault
				if (climbRes.m_bIsClimbOver && ExpansionString.ContainsAny(debugName, s_eAI_PreventClimbOver))
				{
					if (EXTrace.AI)
						EXTrace.Print(true, this, "eAI_CanClimbOn false " + Debug.GetDebugName(parent) + " is scenery? " + object.IsScenery() + " is plain? " + object.IsPlainObject());

					return false;
				}

				if (ExpansionString.ContainsAny(debugName, s_eAI_PreventClimb))
				{
					if (EXTrace.AI)
						EXTrace.Print(true, this, "eAI_CanClimbOn false " + Debug.GetDebugName(parent) + " is scenery? " + object.IsScenery() + " is plain? " + object.IsPlainObject());

					return false;
				}
			}
		}

		if (EXTrace.AI && parent)
			EXTrace.Print(true, this, "eAI_CanClimbOn " + Debug.GetDebugName(parent) + " is scenery? " + object.IsScenery() + " is plain? " + object.IsPlainObject());

		return true;
	}

	bool eAI_IsFallSafe(vector checkDirection, bool checkBlocking = true, float heightThresh = 0, bool checkHealth = true, int dbgIndex = 1337)
	{
		if (heightThresh == 0)
			heightThresh = DayZPlayerImplementFallDamage.HEALTH_HEIGHT_LOW;

		vector position = GetPosition();
		vector checkPosition = position + checkDirection;
		
		vector hitPosition;
		vector hitNormal;
		int contactComponent;
		set<Object> results = new set<Object>;

		//! Is there a physical object blocking check direction (e.g. a wall)?
		//! @note vertical offset of 0.76 m and radius of 0.26 m is chosen so that it can still hit the railing of various buildings
		//! (e.g. Land_Pier_Crane2_Base and Land_Factory_Small), else AI could get stuck due to unsafe fallheight
		if (checkBlocking && DayZPhysics.RaycastRV(checkPosition + "0 0.76 0", position + "0 0.76 0", hitPosition, hitNormal, contactComponent, results, null, this, false, false, ObjIntersectGeom, 0.26))
		{
			//vector fromHit = hitPosition - checkDirection.Normalized() * 0.2;

			////! Is there a physical object blocking up direction from hit pos (e.g. a ceiling)?
			//if (DayZPhysics.RaycastRV(fromHit, fromHit + "0 3.5 0", hitPosition, hitNormal, contactComponent, results, null, this, false, false, ObjIntersectGeom))
				return true;
		}
	#ifdef DIAG_DEVELOPER
		else if (checkBlocking)
		{
			Expansion_DebugObject(dbgIndex + 3, checkPosition + "0 0.76 0", "ExpansionDebugSphereSmall_Red", vector.Zero, position + "0 0.76 0");
		}
	#endif

		//! @note the 1.5m vertical offset has been added for the top floor of Land_HouseBlock_2F3, else surface won't be detected properly
		//! (doesn't matter if via SurfaceRoad3D or raycast)
		float offsetY = 1.5;

/*
		vector begPos = Vector(checkPosition[0], checkPosition[1] + offsetY, checkPosition[2]);
*/

		checkPosition[1] = ExpansionStatic.GetSurfaceRoadY3D(checkPosition[0], checkPosition[1] + offsetY, checkPosition[2], RoadSurfaceDetection.UNDER);

		float fallHeight = position[1] - checkPosition[1];
/*
		if (fallHeight > DayZPlayerImplementFallDamage.HEALTH_HEIGHT_LOW)
		{
			PhxInteractionLayers collisionLayerMask;
			collisionLayerMask |= PhxInteractionLayers.ROADWAY;
			collisionLayerMask |= PhxInteractionLayers.BUILDING;
			collisionLayerMask |= PhxInteractionLayers.FENCE;
			collisionLayerMask |= PhxInteractionLayers.VEHICLE;
			collisionLayerMask |= PhxInteractionLayers.TERRAIN;

			Object hitObject;
		#ifdef DIAG_DEVELOPER
			string type;
		#endif

			if (DayZPhysics.RayCastBullet(begPos, checkPosition, collisionLayerMask, this, hitObject, hitPosition, null, null))
			{
				checkPosition = hitPosition;

				fallHeight = position[1] - checkPosition[1];

			#ifdef DIAG_DEVELOPER
				if (fallHeight > DayZPlayerImplementFallDamage.HEALTH_HEIGHT_LOW)
				{
					if (hitObject)
						type = hitObject.GetType();
					EXTrace.Print(EXTrace.AI, this, "eAI_IsFallSafe RayCastBullet hitObject " + hitObject + " " + type + " " + hitPosition);
				}
			#endif
			}

			if (fallHeight > DayZPlayerImplementFallDamage.HEALTH_HEIGHT_LOW)
			{
				results = new set<Object>;

				if (DayZPhysics.RaycastRV(begPos, checkPosition, hitPosition, hitNormal, contactComponent, results, null, this, false, false, ObjIntersectFire, 0.0))
				{
					checkPosition = hitPosition;

					fallHeight = position[1] - checkPosition[1];

				#ifdef DIAG_DEVELOPER
					if (fallHeight > DayZPlayerImplementFallDamage.HEALTH_HEIGHT_LOW)
					{
						hitObject = results[0];
						if (hitObject)
							type = hitObject.GetType();
						EXTrace.Print(EXTrace.AI, this, "eAI_IsFallSafe RaycastRV hitObject " + hitObject + " " + type + " " + hitPosition);
					}
				#endif
				}
			}
		}
*/
		bool isFallSafe;

		vector waterCheckPosition = checkPosition;
		float waterDepth = g_Game.GetWaterDepth(checkPosition);
		if (waterDepth > 0.0)
		{
			waterCheckPosition[1] = waterCheckPosition[1] + waterDepth;
			fallHeight -= waterDepth;
		}
		
		if (waterDepth > 1.5 && ExpansionStatic.SurfaceIsWater(waterCheckPosition))
			isFallSafe = m_PathFinding.m_IsSwimmingEnabled;  //! Falling into water that is deep enough for swimming is safe if swimming enabled
		else if (fallHeight <= heightThresh || (checkHealth && GetHealth01() - Math.InverseLerp(heightThresh, DayZPlayerImplementFallDamage.HEALTH_HEIGHT_HIGH, fallHeight) >= 0.90))
			isFallSafe = true;

	#ifdef DIAG_DEVELOPER
		//EXTrace.Print(EXTrace.AI, this, "eAI_IsFallSafe position " + ExpansionStatic.VectorToString(position, ExpansionVectorToString.Plain) + " checkDirection " + ExpansionStatic.VectorToString(checkDirection, ExpansionVectorToString.Plain) + " " + ExpansionStatic.VectorToString(checkDirection.VectorToAngles(), ExpansionVectorToString.Plain) + " checkPosition " + ExpansionStatic.VectorToString(checkPosition, ExpansionVectorToString.Plain) + " fallHeight " + fallHeight + " safe " + isFallSafe);
		vector arrowPos = checkPosition;
		arrowPos[1] = position[1] + 0.3;
		vector arrowOri = checkDirection.VectorToAngles();
		arrowOri[1] = -90.0;
		if (isFallSafe)
		{
			Expansion_DebugObject(dbgIndex, arrowPos, "ExpansionDebugArrow", arrowOri.AnglesToVector());
			Expansion_DebugObject(dbgIndex + 1, "0 0 0", "ExpansionDebugArrow_Red");
			Expansion_DebugObject(dbgIndex + 2, "0 0 0", "ExpansionDebugArrow_Red");
		}
		else
		{
			Expansion_DebugObject(dbgIndex, "0 0 0", "ExpansionDebugArrow");
			Expansion_DebugObject(dbgIndex + 1, arrowPos, "ExpansionDebugArrow_Red", arrowOri.AnglesToVector());
			arrowPos[1] = checkPosition[1] + 0.3;
			Expansion_DebugObject(dbgIndex + 2, arrowPos, "ExpansionDebugArrow_Red", arrowOri.AnglesToVector());
		}
	#endif

		return isFallSafe;
	}

	bool eAI_IsDangerousAltitude()
	{
		if (IsSwimming())
			return false;

		vector position = m_ExTransformPlayer[3];
		float fallHeight = position[1] - m_eAI_SurfaceY;

		if (fallHeight < DayZPlayerImplementFallDamage.HEALTH_HEIGHT_LOW)
			return false;

		if (m_PathFinding.m_IsSwimmingEnabled)
		{
			vector surfacePosition = Vector(position[0], m_eAI_SurfaceY, position[2]);
			float waterDepth = Math.Max(g_Game.GetWaterDepth(surfacePosition), 0.0);
			if (fallHeight - waterDepth < DayZPlayerImplementFallDamage.HEALTH_HEIGHT_LOW)
				return false;
		}

		return true;
	}

	float GetThreatToSelf(bool ignoreLOS = false)
	{
		if (ignoreLOS)
			return m_eAI_CurrentThreatToSelf;

		return m_eAI_CurrentThreatToSelfActive;
	}
	void eAI_SetIsFightingFSM(bool state)
	{
	#ifdef DIAG_DEVELOPER
		EXTrace.Print(EXTrace.AI, this, "SetIsFightingFSM " + state);
	#endif
		m_eAI_IsFightingFSM = state;

		eAITarget target = GetTarget();
		if (!state || !target || !target.IsItem())
		{
			eAIGroup group = GetGroup();
			if (group)
				group.SetIsInCombat(state);

			eAI_SetShouldTakeCover(state);
		}

		if (m_eAI_IsFightingFSM && GetExpansionSettings().GetAI().MemeLevel > 9000)
			eAI_PlayRandomLoveSound();
	}

	void eAI_SetShouldTakeCover(bool state)
	{
		if (state)
		{
			m_eAI_ShouldTakeCover = true;
			m_eAI_UpdatePotentialCoverObjects = true;
		}
		else
		{
			m_eAI_ShouldTakeCover = false;
		}
	}

	void eAI_LeaveCover()
	{
		if (m_eAI_CurrentCoverObject)
		{
			s_eAI_TakenCoverObjects.RemoveItem(m_eAI_CurrentCoverObject);
			m_eAI_CurrentCoverObject = null;
		}
	}

	void eAI_PlayRandomLoveSound()
	{
		if (Math.RandomInt(0, 2))
			s_eAI_LoveSound01_SoundSet.Play(this);
		else
			s_eAI_LoveSound02_SoundSet.Play(this);
	}
	array<ref eAITarget> GetTargets()
	{
		return m_eAI_Targets;
	}

	int TargetCount()
	{
		return m_eAI_Targets.Count();
	}

	eAITarget GetTarget(int index = 0)
	{
		if (!m_eAI_Targets || index < 0 || index >= m_eAI_Targets.Count())
			return null;
		return m_eAI_Targets[index];
	}

	eAITargetInformation eAI_GetTargetInformation(int index = 0)
	{
		eAITarget target = GetTarget(index);
		if (target)
			return target.m_Info;

		return null;
	}

	void eAI_AddTarget(eAITarget target)
	{
		m_eAI_Targets.Insert(target);
		if (m_eAI_Targets.Count() == 1)
			m_eAI_SyncCurrentTarget = true;
	}

	void eAI_RemoveTarget(eAITarget target)
	{
		int removeIndex = m_eAI_Targets.Find(target);
		if (removeIndex >= 0)
		{
			m_eAI_Targets.RemoveOrdered(removeIndex);
			if (removeIndex == 0)
				m_eAI_SyncCurrentTarget = true;
			if (removeIndex < m_eAI_NoiseTarget)
				m_eAI_NoiseTarget--;
		}

		eAI_RemoveTargetInfoState(target.m_Info);
		if (target.IsNoise())
		{
			m_eAI_NoiseTargetInfo.SetNoiseParams(null, vector.Zero, 0.0, 0.0, 0.0);
		}
	}

	void eAI_RemoveTargetInfoState(eAITargetInformation info)
	{
		m_eAI_TargetInformationStates.Remove(info);
		ItemBase item;
		if (Class.CastTo(item, info.GetEntity()))
		{
			eAI_ThreatOverride(item, false);
		}
	}

	float eAI_GetCachedThreat(eAITargetInformation info, bool ignoreLOS = false)
	{
		eAITarget state;
		if (m_eAI_TargetInformationStates.Find(info, state))
		{
			if (ignoreLOS)
				return state.m_ThreatLevel;

			return state.m_ThreatLevelActive;
		}

		return 0.0;
	}

	void DetermineThreatToSelf(float pDt)
	{
		m_eAI_PreviousThreatToSelf = m_eAI_CurrentThreatToSelf;
		m_eAI_PreviousThreatToSelfActive = m_eAI_CurrentThreatToSelfActive;

		if (m_eAI_Targets && m_eAI_Targets.Count() > 0)
		{
			eAITarget target = GetTarget();
			eAITargetInformationState state = target;
			m_eAI_CurrentThreatToSelf = target.GetThreat();

			if (!state)
				m_eAI_CurrentThreatToSelfActive = m_eAI_CurrentThreatToSelf;
			else
				m_eAI_CurrentThreatToSelfActive = state.m_ThreatLevelActive;
		}
		else
		{
			m_eAI_CurrentThreatToSelf = 0.0;
			m_eAI_CurrentThreatToSelfActive = 0.0;
		}

		if (m_eAI_PreviousThreatToSelf != m_eAI_CurrentThreatToSelf)
			m_eAI_Callbacks.OnThreatLevelChanged(m_eAI_PreviousThreatToSelf, m_eAI_CurrentThreatToSelf);

		if (m_eAI_PreviousThreatToSelfActive != m_eAI_CurrentThreatToSelfActive)
			m_eAI_Callbacks.OnActiveThreatLevelChanged(m_eAI_PreviousThreatToSelfActive, m_eAI_CurrentThreatToSelfActive);
	}

	void eAI_SetSniperProneDistanceThreshold(float distance)
	{
		m_eAI_SniperProneDistanceThreshold = distance;
	}

	void eAI_SetUnlimitedReload(int unlimitedReload)
	{
		m_eAI_UnlimitedReload = unlimitedReload;

		if (unlimitedReload == eAITargetType.ALL || unlimitedReload == eAITargetType.ALL_OR)
			m_eAI_IsUnlimitedReloadAll = true;
		else
			m_eAI_IsUnlimitedReloadAll = false;
	}

	void eAI_SetDespawnOnLoosingAggro(bool state)
	{
		m_eAI_DespawnOnLoosingAggro = state;
	}

	void eAI_Despawn()
	{
		Delete();
	}

	void eAI_ForceVisibilityUpdate()
	{
		m_eAI_UpdateVisibilityTime = m_eAI_UpdateVisibilityTimeThreshold;
	}
	void eAI_SetAccuracy(float accuracyMin, float accuracyMax)
	{
		if (accuracyMin <= 0)
			accuracyMin = GetExpansionSettings().GetAI().AccuracyMin;

		m_eAI_AccuracyMin = accuracyMin;

		if (accuracyMax <= 0)
			accuracyMax = GetExpansionSettings().GetAI().AccuracyMax;

		m_eAI_AccuracyMax = accuracyMax;

		SetSynchDirty();
	}

	void eAI_SetThreatDistanceLimit(float distance)
	{
		if (distance <= 0)
		{
			auto settings = GetExpansionSettings().GetAI();
			distance = settings.ThreatDistanceLimit;

			if (distance <= 0)
				distance = 1000;
		}

		m_eAI_ThreatDistanceLimit = distance;
	}

	void eAI_SetNoiseInvestigationDistanceLimit(float distance)
	{
		if (distance <= 0)
			distance = GetExpansionSettings().GetAI().NoiseInvestigationDistanceLimit;

		m_eAI_NoiseInvestigationDistanceLimit = distance;
	}

	void eAI_SetMaxFlankingDistance(float distance)
	{
		if (distance <= 0)
			distance = GetExpansionSettings().GetAI().MaxFlankingDistance;

		if (distance > 0)
			m_eAI_MaxFlankingDistance = distance;
	}

	void eAI_SetEnableFlankingOutsideCombat(int option)
	{
		if (option <= 0)
			option = GetExpansionSettings().GetAI().EnableFlankingOutsideCombat;

		if (option > 0)
			m_eAI_EnableFlankingOutsideCombat = true;
		else
			m_eAI_EnableFlankingOutsideCombat = false;
	}

	void eAI_SetDamageMultiplier(float multiplier)
	{
		if (multiplier <= 0)
			multiplier = GetExpansionSettings().GetAI().DamageMultiplier;

		m_eAI_DamageMultiplier = multiplier;
	}

	void eAI_SetDamageReceivedMultiplier(float multiplier)
	{
		if (multiplier <= 0)
			multiplier = GetExpansionSettings().GetAI().DamageReceivedMultiplier;

		m_eAI_DamageReceivedMultiplier = multiplier;
	}
	bool eAI_CanEnableSwimming()
	{
		if (IsSwimming())
			return false;

		if (IsRestrained())
			return false;

		if (m_PathFinding.m_IsSwimmingEnabled)
			return false;

		if (IsBleeding() && m_eAI_DangerousAreaCount == 0)
			return false;

		if (m_eAI_EffectArea)
			return true;

		if (GetCurrentWaterLevel() >= -0.5 || m_eAI_SurfaceY < g_Game.SurfaceGetSeaLevelMin())
		{
			if (GetThreatToSelf() < 0.2)
				return true;
		}

		return false;
	}

	bool eAI_IsExcludedBuildingWithLadder(Object building)
	{
		string type = building.GetType();

		switch (type)
		{
			case "Land_Pier_Crane2_Base":
				if (m_eAI_LastClimbedLadder && m_eAI_LastClimbedBuildingWithLadder == building && m_eAI_LastClimbedLadder.m_Index == 2)
					return true;
				break;

			case "Land_Pier_Crane2_Top":
				if (m_eAI_LastClimbedLadder && m_eAI_LastClimbedBuildingWithLadder == building && m_eAI_LastClimbedLadder.m_Index == 3)
					return true;
				break;

			case "Land_Pier_Crane_A":
				if (m_eAI_LastClimbedLadder && m_eAI_LastClimbedBuildingWithLadder == building && m_eAI_LastClimbedLadder.m_Index == 2)
					return true;
				break;

			case "Land_Pier_Crane_B":
				if (m_eAI_LastClimbedLadder && m_eAI_LastClimbedBuildingWithLadder == building && m_eAI_LastClimbedLadder.m_Index == 1)
					return true;
				break;

			case "Land_Geoplant_PipeHall":
				return true;

			case "Land_Geoplant_CoolingStack":
				return true;
		}

		return false;
	}

	bool eAI_CanReachLadderEntryPoint()
	{
		vector begPos = m_ExTransformPlayer[3];
		begPos[1] = begPos[1] + 1.1;

		PhxInteractionLayers collisionLayerMask;
		collisionLayerMask |= PhxInteractionLayers.ROADWAY;
		collisionLayerMask |= PhxInteractionLayers.BUILDING;
		collisionLayerMask |= PhxInteractionLayers.DOOR;
		collisionLayerMask |= PhxInteractionLayers.FENCE;
		collisionLayerMask |= PhxInteractionLayers.VEHICLE;
		collisionLayerMask |= PhxInteractionLayers.TERRAIN;
		collisionLayerMask |= PhxInteractionLayers.ITEM_LARGE;

		vector contactPos;
		vector contactDir;
		Object hitObj;
		float hitFraction;

		if (DayZPhysics.SphereCastBullet(begPos, m_eAI_LadderEntryPoint, 0.1, collisionLayerMask, this, hitObj, contactPos, contactDir, hitFraction))
		{
			vector dir = vector.Direction(m_eAI_LadderDirPoint, m_eAI_LadderEntryPoint).Normalized();
			vector perpend = dir.Perpend();
			vector min = m_eAI_LadderEntryPoint - perpend * 0.35 - dir * 0.45;
			vector max = m_eAI_LadderEntryPoint + perpend * 0.35 + dir * 0.15;

			if (ExpansionMath.IsPointInRotatedRectangle(min, max, 0.6, contactPos))
				return true;

			return false;
		}

		return true;
	}

	bool eAI_IsCloseToLadderEntryPoint(float maxDist = 2.282542)
	{
		vector begPos = m_ExTransformPlayer[3];

		if (m_eAI_LadderEntryPoint[1] > begPos[1])
			begPos[1] = begPos[1] + 1.1;

		if (vector.DistanceSq(begPos, m_eAI_LadderEntryPoint) < maxDist * maxDist)
			return true;

		return false;
	}

	bool eAI_ShouldUseSurfaceUnderWaterForPathFinding()
	{
		if (m_eAI_SurfaceY < g_Game.SurfaceGetSeaLevelMin())
		{
			if (IsSwimming())
				return true;

			IEntity floor = PhysicsGetFloorEntity();

			if (eAI_IsSeaIce(floor))
				return true;
		}

		return false;
	}

	bool eAI_IsSeaIce(IEntity entity)
	{
		if (entity)
		{
			string name = entity.GetDebugName();
			name.ToLower();

			if (name.Contains(": ice_sea"))
				return true;
		}

		return false;
	}

	float eAI_GetVisibility(float distance, eAITarget target)
	{
		if (distance > m_Expansion_VisibilityDistThreshold)
		{
			if (m_Expansion_Visibility <= m_eAI_NightVisibility)
			{
				if (target.IsLit())
					return ExpansionMath.PowerConversion(250, 200, distance, 0.0, 1.0, 2.0);
			}

			return ExpansionMath.PowerConversion(1100 * m_Expansion_Visibility, m_Expansion_VisibilityDistThreshold, distance, 0.0, 1.0, 2.0);
		}

		return 1.0;
	}

	bool eAI_HasInteractingPlayers()
	{
		for (int i = m_eAI_InteractingPlayers.Count() - 1; i >= 0; i--)
		{
			Man player = m_eAI_InteractingPlayers[i];
			if (player && player.IsAlive() && !player.IsUnconscious())
				return true;
			else
				m_eAI_InteractingPlayers.Remove(i);
		}

		return false;
	}
	bool eAI_IsLowVitals()
	{
		if (GetHealth01("", "") <= 0.5 || GetHealth01("", "Blood") < 0.7)
			return true;

		return false;
	}

	bool eAI_IsInFlankRange(eAITarget target)
	{
		return eAI_IsInFlankRange(target.GetDistance());
	}

	bool eAI_IsInFlankRange(float dist)
	{
		if (dist > 5.0 && dist <= m_eAI_MaxFlankingDistance)
			return true;

		return false;
	}

	void OverrideTurnTarget(bool pActive, float angle)
	{
		m_eAI_TurnTargetActive = pActive;
		m_eAI_TurnTarget = angle;
	}
	/**
	 * Expansion-compatible path target override. The final goal is stored on the AI,
	 * while ExpansionPathHandler owns the actual path/waypoint chain.
	 */
	
	
	// @param LookWS a position in WorldSpace to look at
	void LookAtPosition(vector pPositionWS, bool recalculate = true)
	{
		m_eAI_LookPosition_WorldSpace = pPositionWS;
		m_eAI_LookDirection_Recalculate = recalculate;
	}

	// @param AimWS a position in WorldSpace to aim at
	void AimAtPosition(vector pPositionWS, bool recalculate = true)
	{
		m_eAI_AimPosition_WorldSpace = pPositionWS;
		m_eAI_AimDirection_Recalculate = recalculate;
	}

	void LookAtDirection(vector pDirectionMS)
	{
		m_eAI_LookDirectionTarget_ModelSpace = pDirectionMS;
		m_eAI_LookDirection_Recalculate = false;
	}

	void AimAtDirection(vector pDirectionMS)
	{
		m_eAI_AimDirectionTarget_ModelSpace = pDirectionMS;
		m_eAI_AimDirection_Recalculate = false;
	}

	vector GetLookRelAngles()
	{
		return m_eAI_LookRelAngles;
	}

	vector GetLookDirection()
	{
		return m_eAI_LookRelAngles.AnglesToVector().Multiply3(m_ExTransformPlayer);
	}

	bool GetLookDirectionRecalculate()
	{
		return m_eAI_LookDirection_Recalculate;
	}

	vector GetAimRelAngles()
	{
		return m_eAI_AimRelAngles;
	}

	vector GetAimDirection()
	{
		return m_eAI_AimRelAngles.AnglesToVector().Multiply3(m_ExTransformPlayer);
	}

	vector GetAimDirectionTarget()
	{
		return m_eAI_AimDirectionTarget_ModelSpace.Multiply3(m_ExTransformPlayer);
	}

	vector GetWeaponAimDirection()
	{
		vector aimRelAngles = Vector(m_eAI_AimRelAngleLR, m_eAI_AimRelAngleUD, 0.0);
		return aimRelAngles.AnglesToVector();
	}

	override vector Expansion_GetAimDirection()
	{
		return GetAimDirection();
	}

	vector Expansion_GetAimDirectionClient()
	{
		return super.Expansion_GetAimDirection();
	}

	override vector GetAimPosition()
	{
		return m_eAI_AimPosition_WorldSpace;
	}

	bool GetAimDirectionRecalculate()
	{
		return m_eAI_AimDirection_Recalculate;
	}
void eAI_OnRemovePlayer(CF_DoublyLinkedNode_WeakRef<PlayerBase> node)
	{
		if (node == m_eAI_PotentialTargetPlayer)
			m_eAI_PotentialTargetPlayer = node.m_Next;

		m_eAI_PlayersWithinVisibilityDistanceLimit.RemoveItem(node);
	}

	void eAI_OnRemoveCustomCreature(CF_DoublyLinkedNode_WeakRef<eAICustomCreatureTargetInformation> node)
	{
		if (node == m_eAI_PotentialTargetCustomCreature)
			m_eAI_PotentialTargetCustomCreature = node.m_Next;
	}

	void eAI_OnNoiseEvent(EntityAI source, vector position, float lifetime, eAINoiseParams params, float strengthMultiplier)
	{
		if (m_eAI_NoiseInvestigationDistanceLimit <= 0)
			return;

		if (IsUnconscious())
			return;

		float strength = params.m_Strength * strengthMultiplier;
		if (strength <= 0)
			return;

		EntityAI root;
		if (source)
		{
			root = source.GetHierarchyRoot();

			if (root == this)
				return;

			if (m_eAI_CurrentThreatToSelfActive >= 0.4 && m_eAI_HasLOS)
			{
				eAITarget currentTarget = GetTarget();
				if (currentTarget && currentTarget.GetEntity() == root)
					return;
			}
		}

		DayZPlayerImplement player;
		eAIGroup ourGroup = GetGroup();
		eAIFaction faction = ourGroup.GetFaction();
		eAIPlayerTargetInformation info;
		if (Class.CastTo(player, root))
		{
			if (params.m_Type != eAINoiseType.BULLETIMPACT)
			{
				eAIGroup theirGroup = player.GetGroup();
				if (theirGroup)
				{
					if (theirGroup == ourGroup || theirGroup.GetFaction().IsFriendly(faction) || theirGroup.GetFaction().IsFriendlyEntity(this))
						return;
				}
			}

			Class.CastTo(info, player.GetTargetInformation());

			if (info && eAI_HasLOS(info))
				return;
		}

		if (source && position == vector.Zero)
			position = source.GetPosition();

		float strengthSq = strength * strength;
		float distSq = vector.DistanceSq(GetPosition(), position);
		if (distSq > strengthSq)
			return;

		if (params.m_Type == eAINoiseType.BULLETIMPACT && info)
		{
			bool created;
			eAITarget target = info.AddAI(this, -1, true, created);

			if (target.m_ThreatLevel > 0.2)
			{
				float threat = 1.0;

				if (threat > target.m_ThreatLevelActive)
				{
					if (!target.m_SearchOnLOSLost)
					{
						target.SetInitial(threat, position);
						target.m_SearchOnLOSLost = true;
					}
					else
					{
						target.SetThreat(threat);
					}
				}

				if (!created)
					target.Update();

				return;
			}
		}

		float distance = Math.Sqrt(distSq);
		float threatLevel;
		if (eAI_IsPassive() || faction.IsObserver() || faction.IsGuard())
		{
			threatLevel = ExpansionMath.PowerConversion(0.5, Math.Max(strength, 30.0), distance, 0.152, 0.1, 0.1);
		}
		else
		{
			float distMin;
			float threatLevelMax;

			if (params.m_Type == eAINoiseType.BULLETIMPACT)
			{
				distMin = 0.5;
				threatLevelMax = 0.2;
			}
			else
			{
				distMin = Math.Min(strength, m_eAI_NoiseInvestigationDistanceLimit);
				threatLevelMax = 0.4;
			}

			threatLevel = ExpansionMath.LinearConversion(distMin, strength * 1.1, distance, threatLevelMax, 0.1024);
		}

		if (threatLevel >= m_eAI_NoiseTargetInfo.GetThreat())
		{
			position = ExpansionMath.GetRandomPointInCircle(position, distance * 0.05);

			if (threatLevel >= 0.4)
				lifetime = ExpansionMath.LinearConversion(0.0, 500.0, distance, 3.0, 240.0);
			else
				lifetime = Math.RandomFloat(2.0, 4.0);

			m_eAI_NoiseTargetInfo.SetNoiseParams(source, position, strength, lifetime, threatLevel);

			int delay = distance * 2.915452 + 170;
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(eAI_AddNoiseTarget, delay, false, threatLevel);
		}
	}

	void eAI_AddNoiseTarget(float threatLevel)
	{
		if (IsDamageDestroyed())
			return;

		if (threatLevel >= m_eAI_NoiseTargetInfo.GetThreat())
		{
			int maxTime = m_eAI_NoiseTargetInfo.GetLifetime() * 1000;
			bool created;

			eAITarget state = m_eAI_NoiseTargetInfo.AddAI(this, maxTime, true, created);

			if (!created)
			{
				state.UpdatePosition(true);
				state.UpdateFoundAtTime();
			}
		}
	}
	void eAI_UpdatePotentialCoverObjects(float pDt)
	{
		eAIGroup group = GetGroup();
		if (!group)
			return;

		bool updateCoverObjects;
		if (m_eAI_ShouldTakeCover)
			updateCoverObjects = true;
		else if (group.GetWaypointBehaviour() == eAIWaypointBehavior.ROAMING && group.GetFormationLeader() == this)
			updateCoverObjects = true;

		if (!updateCoverObjects)
			return;

		m_eAI_UpdateTargetsTick += pDt;
		if (m_eAI_UpdateTargetsTick <= Math.RandomFloat(0.1, 0.2) && !m_eAI_UpdatePotentialCoverObjects)
			return;

		m_eAI_UpdateTargetsTick = 0;

		vector center = GetPosition();
		vector min = Vector(center[0] - 30, center[1] - 30, center[2] - 30);
		vector max = Vector(center[0] + 30, center[1] + 30, center[2] + 30);

		if (m_eAI_UpdateNearTargetsCount % 33 != 0 && !m_eAI_UpdatePotentialCoverObjects)
			return;

		m_eAI_PotentialCoverObjects.Clear();
		DayZPlayerUtils.PhysicsGetEntitiesInBox(min, max, m_eAI_PotentialTargetEntities);
		m_eAI_UpdatePotentialCoverObjects = false;

		for (int i = m_eAI_PotentialTargetEntities.Count() - 1; i >= 0; i--)
		{
			EntityAI potentialCoverObj = m_eAI_PotentialTargetEntities[i];
			if (!potentialCoverObj)
				continue;

			bool isPotentialCoverObject;
			if (potentialCoverObj.IsBuilding())
			{
				string type = potentialCoverObj.GetType();
				if ((type.IndexOf("Land_") == 0 || type.IndexOf("Wreck_") == 0) && type.IndexOf("Gate") == -1)
					isPotentialCoverObject = true;
			}
			else if (potentialCoverObj.IsRock())
			{
				isPotentialCoverObject = true;
			}
			else if (potentialCoverObj.IsTree())
			{
				isPotentialCoverObject = true;
			}
			else if (potentialCoverObj.IsTransport())
			{
				isPotentialCoverObject = true;
			}

			if (isPotentialCoverObject)
			{
				m_eAI_PotentialCoverObjects.Insert(potentialCoverObj);
				m_eAI_PotentialTargetEntities.Remove(i);
			}
		}

		m_eAI_UpdateNearTargetsCount++;
	}
	void OverrideTargetPosition(vector pPosition, bool isFinal = true, float maxDistance = 1.0, bool allowJumpClimb = true)
	{
		// Safety: if Y is zero (common config mistake), project to surface so navmesh sampling works
		if (pPosition[1] == 0.0 && vector.DistanceSq("0 0 0", pPosition) > 0.01)
			pPosition[1] = g_Game.SurfaceY(pPosition[0], pPosition[2]);

		string isFinalStr = "false";
		if (isFinal)
			isFinalStr = "true";
		if (vector.DistanceSq(m_eAI_TargetPosition, pPosition) > 1.0)
			Print(string.Format("[EAI] [move_target] bot=%1 botPos=%2 newTarget=%3 isFinal=%4 maxDist=%5", ToString(), GetPosition().ToString(), pPosition.ToString(), isFinalStr, maxDistance.ToString()));
		else
			Print(string.Format("[EAI] [move_target_same] bot=%1 pos=%2 isFinal=%3 maxDist=%4", ToString(), pPosition.ToString(), isFinalStr, maxDistance.ToString()));

#ifndef EAI_USE_LEGACY_PATHFINDING
		m_PathFinding.SetTarget(pPosition, maxDistance, allowJumpClimb);
#else
		m_PathFinding.OverridePosition(pPosition);
#endif

		m_eAI_TargetPosition = pPosition;
		m_eAI_TargetPositionIsFinal = isFinal;
	}

	void OverrideTargetPosition(eAITarget target, bool isFinal = true)
	{
		if (m_eAI_IsOnLadder)
		{
			Print("[EAI] [otp_return] bot=" + ToString() + " reason=on_ladder");
			return;
		}

		string targetEntStr = "null";
		if (target && target.GetEntity())
			targetEntStr = target.GetEntity().ToString();
		string isFinalStr = "false";
		if (isFinal)
			isFinalStr = "true";
		Print(string.Format("[EAI] [move_target_ent] bot=%1 botPos=%2 targetEntity=%3 targetPos=%4 isFinal=%5", ToString(), GetPosition().ToString(), targetEntStr, target.GetPosition().ToString(), isFinalStr));
		float dbgDirLen = target.GetDirection().Length();
		string otpIn = "bot=" + ToString();
		otpIn = otpIn + " target=" + targetEntStr;
		otpIn = otpIn + " targetPos=" + target.GetPosition().ToString();
		otpIn = otpIn + " dirLen=" + dbgDirLen.ToString();
		otpIn = otpIn + " minDistNow=" + target.GetMinDistance(dbgDirLen).ToString();
		otpIn = otpIn + " targetLOS=" + BQExpansionDiag.B(target.m_LOS);
		otpIn = otpIn + " raised=" + BQExpansionDiag.B(IsRaised());
		otpIn = otpIn + " raiseDone=" + BQExpansionDiag.B(IsWeaponRaiseCompleted());
		otpIn = otpIn + " pfUnreach=" + BQExpansionDiag.B(m_PathFinding.m_IsUnreachable);
		otpIn = otpIn + " pfTargetUnreach=" + BQExpansionDiag.B(m_PathFinding.m_IsTargetUnreachable);
		BQExpansionDiag.Log("OTP_TARGET_IN", otpIn);

		vector pos;
		vector dir = target.GetDirection();

		float dist = dir.Length();
		float minDist = target.GetMinDistance(dist);
		bool allowJumpClimb = true;
		bool keepMinDistToTarget;
		bool cannotMelee;
		bool flank;

		if (minDist)
		{
			//! Return early if we are already overriding position and not yet near endpoint
			if (!eAI_ShouldUpdatePosition())
			{
				if (m_eAI_IsInDangerByArea)
					OverrideTargetPosition(m_eAI_TargetPosition, m_eAI_TargetPositionIsFinal, 1.0, m_PathFinding.m_AllowJumpClimb);
				return;
			}

			keepMinDistToTarget = true;
			//EXTrace.Print(EXTrace.AI, this, "Positioning - min dist " + minDist);
		}
		else
		{
			if (!IsRaised() && target.IsEntity() && !target.CanMeleeIfClose())
				cannotMelee = true;

			//! While weapon is raised and we have LOS or action/weapon manager is running (e.g. reload/unjam), reposition/seek cover
			if ((IsRaised() && target.m_LOS) || GetActionManager().GetRunningAction() || GetWeaponManager().IsRunning() || cannotMelee)
			{
				allowJumpClimb = false;
				//EXTrace.Print(EXTrace.AI, this, "Positioning - flank time reset " + m_eAI_FlankTime);
				m_eAI_FlankTime = 0.0;
				m_eAI_FlankTimeMax = 0.0;
			}
			else if (eAI_IsInFlankRange(dist) && !eAI_IsDangerousAltitude())
			{
				if (m_eAI_FlankTime <= 0.0)
				{
					if (Math.RandomInt(0, 2))
						m_eAI_FlankAngle = Math.RandomFloat(56.25, 67.5);
					else
						m_eAI_FlankAngle = -Math.RandomFloat(56.25, 67.5);

					m_eAI_FlankTimeMax = Math.RandomFloat(dist * 0.3, dist * 0.6);
					//EXTrace.Print(EXTrace.AI, this, "Positioning - flank angle " + m_eAI_FlankAngle + ", time max " + m_eAI_FlankTimeMax);
				}

				m_eAI_FlankTime += m_dT;

				//! Return early if we are already overriding position and not yet near endpoint and not in cover
				if (m_eAI_FlankTimeMax > 0.0 && !eAI_ShouldUpdatePosition() && !m_eAI_IsInCover)
				{
					string inCoverStr = "false";
					if (m_eAI_IsInCover)
						inCoverStr = "true";
					string otpReturnPrint = "[EAI] [otp_return] bot=" + ToString();
					otpReturnPrint = otpReturnPrint + " reason=flank_shouldNotUpdate";
					otpReturnPrint = otpReturnPrint + " flankTime=" + m_eAI_FlankTime.ToString();
					otpReturnPrint = otpReturnPrint + " flankTimeMax=" + m_eAI_FlankTimeMax.ToString();
					otpReturnPrint = otpReturnPrint + " timeout=" + m_eAI_PositionOverrideTimeout.ToString();
					otpReturnPrint = otpReturnPrint + " inCover=" + inCoverStr;
					Print(otpReturnPrint);
					if (m_eAI_IsInDangerByArea)
						OverrideTargetPosition(m_eAI_TargetPosition, m_eAI_TargetPositionIsFinal, 1.0, m_PathFinding.m_AllowJumpClimb);
					return;
				}

				//! Flank time is based on a random speed estimate.
				//! Sprint = 6.66 m/s avg or factor 0.3, half speed = 3.33 m/s avg or factor 0.6 (lower speed = more time)
				if (m_eAI_FlankTime >= m_eAI_FlankTimeMax)
				{
					//EXTrace.Print(EXTrace.AI, this, "Positioning - flank time exceeded " + m_eAI_FlankTime);

					if ((m_eAI_FlankTime >= m_eAI_FlankTimeMax * 2 && Math.RandomInt(0, 3000) < 1) || m_PathFinding.m_IsUnreachable)
					{
						m_eAI_FlankTime = 0.0;
						Expansion_GetUp();
						if (m_eAI_IsInDangerByArea)
							OverrideTargetPosition(m_eAI_TargetPosition, m_eAI_TargetPositionIsFinal, 1.0, m_PathFinding.m_AllowJumpClimb);
						return;
					}

					if (m_eAI_IsInCover)
					{
						if (m_eAI_CurrentCoverObject && m_eAI_CurrentCoverObject.IsRock())
							OverrideStance(DayZPlayerConstants.STANCEIDX_PRONE);
						else if (eAI_GetStance() == DayZPlayerConstants.STANCEIDX_ERECT && m_eAI_StancePreference != DayZPlayerConstants.STANCEIDX_PRONE)
							OverrideStance(DayZPlayerConstants.STANCEIDX_CROUCH);

						EntityAI hands = GetHumanInventory().GetEntityInHands();
						if (hands && hands.IsWeapon())
							eAI_RaiseWeapon(true);

						m_eAI_TargetPositionIsFinal = true;

						return;
					}
				}

				flank = true;
				isFinal = false;
				Expansion_GetUp();
				//EXTrace.Print(EXTrace.AI, this, "Positioning - flanking " + m_eAI_FlankTime);
			}
			else
			{
				//EXTrace.Print(EXTrace.AI, this, "Positioning - dist " + dist);
				m_eAI_FlankTime = 0.0;  //! Allow switch to search if flanking previously
			}
		}

		Weapon_Base weapon;
		if (Class.CastTo(weapon, GetHumanInventory().GetEntityInHands()))
		{
			if (weapon.ShootsExplosiveAmmo())
			{
				minDist = Math.Max(weapon.Expansion_GetMinSafeFiringDistance(), minDist);
				keepMinDistToTarget = true;
			}
		}

		if (!keepMinDistToTarget && !allowJumpClimb)
		{
			if (dist < 3.0 && !cannotMelee)
			{
				//! If the distance to target is already less than 3 m, don't take cover, just try to keep the distance
				minDist = 3.0;  //! Potentially backpedal
			}
			else
			{
				//! Return early if we are already overriding position and not yet near endpoint
				if (!eAI_ShouldUpdatePosition())
				{
					Print(string.Format("[EAI] [otp_return] bot=%1 reason=minDist_shouldNotUpdate timeout=%2", ToString(), m_eAI_PositionOverrideTimeout.ToString()));
					if (m_eAI_IsInDangerByArea)
						OverrideTargetPosition(m_eAI_TargetPosition, m_eAI_TargetPositionIsFinal, 1.0, m_PathFinding.m_AllowJumpClimb);
					return;
				}

				if (weapon)
				{
					//! Determine min distance range based on zeroing range.
					//! Final min distance will be chosen to be lower or equal to this range.

					ItemOptics optics = weapon.GetAttachedOptics();

					if (optics)
						minDist = optics.GetZeroingDistanceZoomMax();
					else
						minDist = weapon.GetZeroingDistanceZoomMax(weapon.GetCurrentMuzzle()) * 0.3;

					if (minDist > 1000)
						minDist = 1000;
				}

				//! Stay at current distance if lower than min distance range
				//! (roughly, may take cover or reposition to random point if no cover available)
				if (dist <= minDist)
					minDist = dist;
				else if (cannotMelee)
					minDist = 30.0;
			}

			//EXTrace.Print(EXTrace.AI, this, "Positioning - no jump/climb, min dist " + minDist);
		}

		Object currentCoverObj = m_eAI_CurrentCoverObject;

		if (minDist || flank)
		{
			pos = GetPosition();

			if (flank)
			{
				vector ori = dir.VectorToAngles();

				ori[0] = ExpansionMath.RelAngle(ori[0] + m_eAI_FlankAngle);

				dir = ori.AnglesToVector();
			}

			if (m_eAI_CurrentCoverObject)
			{
				s_eAI_TakenCoverObjects.RemoveItem(m_eAI_CurrentCoverObject);
				m_eAI_CurrentCoverObject = null;
			}

			if (m_eAI_PotentialCoverObjects.Count() && (!target.IsMechanicalTrap() || target.IsExplosive()))
			{
				vector aimDirectionTarget = GetAimDirectionTarget();
				vector forwardPos = GetPosition();
				
				vector toObjDir;
				vector toObjDirNorm;
				float dot;
				int coverObjKey;
				float distSq = dist * dist;
				float objDistSq;

				vector targetPos;
				float minDistSq;
				float objToTargetDistSq;

				if (keepMinDistToTarget)
				{
					targetPos = target.GetPosition(true);
					minDistSq = minDist * minDist;
				}

				TIntArray distances = {};
				map<int, Object> potentialCoverObjs = new map<int, Object>;

				foreach (Object obj: m_eAI_PotentialCoverObjects)
				{
					if (!obj)
						continue;

					//! Don't try to find cover behind objects with active physics
					if (dBodyIsActive(obj))
						continue;

					if (flank)
					{
						if (obj == currentCoverObj)
							continue;
					}

					if (s_eAI_TakenCoverObjects.Find(obj) > -1)
						continue;

					toObjDir = vector.Direction(forwardPos, obj.GetPosition());
					toObjDirNorm = toObjDir.Normalized();

					objDistSq = toObjDir.LengthSq();

					if (flank)
					{
						dot = vector.Dot(dir, toObjDirNorm);

						//! Select object if in direction of flank angle
						if (dot < 0.75)
							continue;
					}
					else
					{
						dot = vector.Dot(aimDirectionTarget, toObjDirNorm);

						//! Select object if closer than target or if opposite of AI aim direction target
						if ((keepMinDistToTarget || distSq <= objDistSq) && dot >= 0.0)
							continue;
					}

					if (keepMinDistToTarget)
						objToTargetDistSq = vector.DistanceSq(obj.GetPosition(), targetPos);

					if (!keepMinDistToTarget || objToTargetDistSq > minDistSq)
					{
						coverObjKey = Math.Round(objDistSq);
						distances.Insert(coverObjKey);
						potentialCoverObjs[coverObjKey] = obj;
					}
				}

				if (distances.Count())
				{
					//! Select closest potential cover
					distances.Sort();

					coverObjKey = distances[0];

					m_eAI_CurrentCoverObject = potentialCoverObjs[coverObjKey];
					//m_eAI_PotentialCoverObjects.Clear();
					//m_eAI_PotentialCoverObjects.Insert(m_eAI_CurrentCoverObject);
					s_eAI_TakenCoverObjects.Insert(m_eAI_CurrentCoverObject);

				#ifdef DIAG_DEVELOPER
					EXTrace.Print(EXTrace.AI, this, "New cover object selected: " + m_eAI_CurrentCoverObject + " " + m_eAI_CurrentCoverObject.GetPosition());
				#endif

					BuildingBase building;
					bool houseWithDoors;
					float extension;
					vector minMax[2];
					if (m_eAI_CurrentCoverObject.IsTree())
					{
						extension = 2.0;
					}
					else if (Class.CastTo(building, m_eAI_CurrentCoverObject) && building.Expansion_IsEnterable())
					{
						houseWithDoors = true;
					}
					else if (m_eAI_CurrentCoverObject.GetCollisionBox(minMax))
					{
						minMax[0][1] = 0.0;
						minMax[1][1] = 0.0;
						extension = vector.Distance(minMax[0], minMax[1]) * 0.5 + 1.0;
					}
					else
					{
						extension = m_eAI_CurrentCoverObject.ClippingInfo(minMax);
					}

					vector objPos = m_eAI_CurrentCoverObject.GetPosition();

					if (houseWithDoors)
					{
						pos = objPos;
					}
					else
					{
						objPos[1] = m_eAI_AimPosition_WorldSpace[1];
						vector objToTargetDir = vector.Direction(objPos, m_eAI_AimPosition_WorldSpace);
						vector objToTargetDirNorm = objToTargetDir.Normalized();
						pos = objPos - objToTargetDirNorm * extension;
					}

					if (flank)
					{
						m_eAI_FlankTimeMax = Math.RandomFloat(dist * 0.3, dist * 0.6);
						m_eAI_PositionOverrideTimeout = m_eAI_FlankTimeMax;
					}
					else
					{
						m_eAI_PositionOverrideTimeout = 3.0;
					}
				}
			}

			if (!m_eAI_CurrentCoverObject)
			{
				//EXTrace.Print(EXTrace.AI, this, "Positioning - no cover obj");

				if (dist != minDist)
				{
					//EXTrace.Print(EXTrace.AI, this, "Positioning - dist != min dist, AI pos=" + pos);

					pos = pos + dir.Normalized() * (dist - minDist * Math.RandomFloat(1.0, 2.0));
				}

				if (flank)
				{
					m_eAI_FlankTimeMax = Math.RandomFloat(dist * 0.3, dist * 0.6);
					m_eAI_PositionOverrideTimeout = m_eAI_FlankTimeMax;
				}
				else
				{
					if (keepMinDistToTarget)
					{
						m_eAI_PositionOverrideTimeout = 3.0;
					}
					else
					{
						if (minDist > 3.0)
							pos = ExpansionMath.GetRandomPointInCircle(pos, minDist);

						m_eAI_PositionOverrideTimeout = Math.RandomFloat(5.0, 10.0);
					}
				}
			}
		}
		else
		{
			pos = target.GetPosition();
		}

		//EXTrace.Print(EXTrace.AI, this, "Positioning - pos " + pos + " dist " + dist + " minDist " + minDist + " cover obj " + m_eAI_CurrentCoverObject);
			string allowJumpClimbStr = "false";
			if (allowJumpClimb)
				allowJumpClimbStr = "true";
			string flankStr = "false";
			if (flank)
				flankStr = "true";
			Print(string.Format("[EAI] [otp_setpos] bot=%1 pos=%2 isFinal=%3 allowJumpClimb=%4 flank=%5 minDist=%6", ToString(), pos.ToString(), isFinalStr, allowJumpClimbStr, flankStr, minDist.ToString()));
			string otpOut = "bot=" + ToString();
			otpOut = otpOut + " selectedPos=" + pos.ToString();
			otpOut = otpOut + " isFinal=" + isFinalStr;
			otpOut = otpOut + " allowJumpClimb=" + allowJumpClimbStr;
			otpOut = otpOut + " flank=" + flankStr;
			otpOut = otpOut + " minDist=" + minDist.ToString();
			otpOut = otpOut + " cover=" + BQExpansionDiag.Obj(m_eAI_CurrentCoverObject);
			otpOut = otpOut + " timeout=" + m_eAI_PositionOverrideTimeout.ToString();
			BQExpansionDiag.Log("OTP_TARGET_OUT", otpOut);
		OverrideTargetPosition(pos, isFinal, 1.0, allowJumpClimb);

		if ((minDist || flank) && m_eAI_CurrentCoverObject && m_eAI_CurrentCoverObject != currentCoverObj)
		{
			m_PathFinding.OnUpdate(m_dT, -1.0);
			m_eAI_CurrentCoverPosition = m_PathFinding.GetEnd();
		#ifdef DIAG_DEVELOPER
			//EXTrace.Print(EXTrace.AI, this, "Positioning - cover obj " + m_eAI_CurrentCoverObject + " pos " + m_eAI_CurrentCoverObject.GetPosition() + " cover pos " + m_eAI_CurrentCoverPosition + " dist to cover pos " + vector.Distance(m_eAI_CurrentCoverObject.GetPosition(), m_eAI_CurrentCoverPosition));
			Expansion_DebugObject_Deferred(20, m_eAI_CurrentCoverPosition, "ExpansionDebugNoticeMe", GetDirection());
		}
		else if (!m_eAI_CurrentCoverObject)
		{
			Expansion_DebugObject_Deferred(20, "0 0 0", "ExpansionDebugNoticeMe");
		#endif
		}
	}

	bool eAI_ShouldUpdatePosition()
	{
		bool nearEnd = Math.IsPointInCircle(GetPosition(), 0.55, m_PathFinding.GetEnd());
		if (m_eAI_PositionOverrideTimeout > 0 && (!nearEnd || m_eAI_IsInCover))
		{
			string otpUpd0 = "result=0 bot=" + ToString();
			otpUpd0 = otpUpd0 + " timeout=" + m_eAI_PositionOverrideTimeout.ToString();
			otpUpd0 = otpUpd0 + " nearEnd=" + BQExpansionDiag.B(nearEnd);
			otpUpd0 = otpUpd0 + " inCover=" + BQExpansionDiag.B(m_eAI_IsInCover);
			otpUpd0 = otpUpd0 + " botPos=" + GetPosition().ToString();
			otpUpd0 = otpUpd0 + " pathEnd=" + m_PathFinding.GetEnd().ToString();
			BQExpansionDiag.Log("OTP_SHOULD_UPDATE", otpUpd0);
			return false;
		}

		string otpUpd1 = "result=1 bot=" + ToString();
		otpUpd1 = otpUpd1 + " timeout=" + m_eAI_PositionOverrideTimeout.ToString();
		otpUpd1 = otpUpd1 + " nearEnd=" + BQExpansionDiag.B(nearEnd);
		otpUpd1 = otpUpd1 + " inCover=" + BQExpansionDiag.B(m_eAI_IsInCover);
		otpUpd1 = otpUpd1 + " botPos=" + GetPosition().ToString();
		otpUpd1 = otpUpd1 + " pathEnd=" + m_PathFinding.GetEnd().ToString();
		BQExpansionDiag.Log("OTP_SHOULD_UPDATE", otpUpd1);
		return true;
	}
	void eAI_ResetFlankTimers()
	{
		m_eAI_FlankTime = 0.0;
		m_eAI_FlankTimeMax = 0.0;
		m_eAI_PositionOverrideTimeout = 0.0;
	}

	void eAI_SetPositionOverrideTimeoutMS(int timeoutMs)
	{
		if (timeoutMs > 0)
			m_eAI_PositionOverrideTimeout = timeoutMs * 0.001;
		else
			m_eAI_PositionOverrideTimeout = 0.0;
	}

	void eAI_UpdatePositionOverrideTimeout(float pDt)
	{
		if (m_eAI_PositionOverrideTimeout <= 0.0)
			return;

		m_eAI_PositionOverrideTimeout = m_eAI_PositionOverrideTimeout - pDt;
		if (m_eAI_PositionOverrideTimeout < 0.0)
			m_eAI_PositionOverrideTimeout = 0.0;
	}

	bool eAI_ShouldTakeCover()
	{
		return m_eAI_ShouldTakeCover;
	}

	bool eAI_ShouldUpdatePotentialCoverObjects()
	{
		return m_eAI_UpdatePotentialCoverObjects;
	}

	void eAI_ClearPotentialCoverObjectsUpdate()
	{
		m_eAI_UpdatePotentialCoverObjects = false;
	}
	bool OverrideStance(int pStanceIdx, bool force = false)
	{
		if (IsClimbing() || IsFalling() || IsFighting() || IsSwimming() || IsUnconscious())
			return false;

		auto move = GetCommand_Move();

		if (move)
		{
			int stanceIdx = eAI_GetStance();
			int pStanceIdxOrig = pStanceIdx;

			if (pStanceIdx < m_eAI_DefaultStance)
			{
				if (g_Game.GetTickTime() - m_eAI_LastHitTime > 10.0 && !m_eAI_IsPreparingMelee && GetCurrentWaterLevel() < 0.3)
					pStanceIdx = m_eAI_DefaultStance;
			}

			if (m_eAI_CommandMove.OverrideStance(pStanceIdx, force))
			{
				if (pStanceIdxOrig == DayZPlayerConstants.STANCEIDX_ERECT)
					m_eAI_ShouldGetUp = true;
				else
					m_eAI_ShouldGetUp = false;

				return true;
			}
		}

		return false;
	}

	bool eAI_IsChangingStance()
	{
		auto cmd = GetCommand_Move();
		return cmd && m_eAI_CommandMove.IsChangingStance();
	}

	bool eAI_AdjustStance(Weapon_Base gun, float distToTargetSq)
	{
		//! Expansion-compatible overload used by eAIState_Fighting_FireWeapon.
		//! It only adjusts stance; actual firing is still delegated to BQ firebridge via TryFireWeapon().
		float distThreshSq = m_eAI_SniperProneDistanceThreshold * m_eAI_SniperProneDistanceThreshold;
		if (gun && distThreshSq > 0.0 && distToTargetSq > distThreshSq && g_Game.GetTickTime() - m_eAI_LastHitTime > 10.0)
		{
			vector targetDirection = vector.Direction(GetPosition(), m_eAI_AimPosition_WorldSpace).Normalized();
			ItemOptics optics;
			if (vector.Dot(GetDirection(), targetDirection) > 0.9 && Class.CastTo(optics, gun.GetAttachedOptics()) && optics.GetZeroingDistanceZoomMax() >= 800)
				return OverrideStance(DayZPlayerConstants.STANCEIDX_PRONE);
		}

		if (m_eAI_IsInCover)
			return false;

		return OverrideStance(DayZPlayerConstants.STANCEIDX_ERECT);
	}

	bool eAI_AdjustStance(int lastFireTime, int timeSinceLastFire, int timeBetweenFiringAndGettingUp)
	{
		bool getUp;

		if (lastFireTime > 0 && timeSinceLastFire > timeBetweenFiringAndGettingUp && !m_eAI_IsInCover)
		{
			getUp = Expansion_GetUp();
		}

		return getUp;
	}

	int eAI_GetStance()
	{
		int stance = m_MovementState.m_iStanceIdx;

		if (stance >= DayZPlayerConstants.STANCEIDX_RAISED)
			stance -= DayZPlayerConstants.STANCEIDX_RAISED;

		return stance;
	}

	override bool eAI_IsSideSteppingObstacles()
	{
		auto cmd = GetCommand_Move();
		return cmd && m_eAI_CommandMove.IsSideSteppingObstacles();
	}

	bool eAI_IsSideSteppingVehicle()
	{
		if (m_eAI_SideStepObject && m_eAI_SideStepObject.IsTransport())
			return true;

		return false;
	}
	bool eAI_ShouldGetUp()
	{
		return m_eAI_ShouldGetUp;
	}

	void SetMovementSpeedLimit(int pSpeed, bool roaming = false)
	{
		m_MovementSpeedLimit = eAI_GetMovementSpeed(pSpeed);

		if (roaming)
			m_eAI_RoamingMovementSpeedLimit = m_MovementSpeedLimit;
	}

	void SetMovementSpeedLimits(int pSpeed, int pSpeedUnderThreat = -1)
	{
		eAI_SetSpeedLimitPreference(pSpeed);
		if (pSpeedUnderThreat == -1)
			pSpeedUnderThreat = pSpeed;
		m_MovementSpeedLimitUnderThreat = eAI_GetMovementSpeed(pSpeedUnderThreat);
	}

	void eAI_SetBestSpeedLimit(int speedLimit)
	{
		if (speedLimit <= 1)
			return;

		eAIGroup group = GetGroup();
		eAIBase ai;

		if (group.GetWaypointBehaviour() == eAIWaypointBehavior.ROAMING && speedLimit < m_eAI_RoamingMovementSpeedLimit)
			speedLimit = m_eAI_RoamingMovementSpeedLimit;

		for (int i = 0; i < group.Count(); i++)
		{
			if (Class.CastTo(ai, group.GetMember(i)) && ai != this)
			{
				if (ai.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS || (!IsSwimming() && ai.IsSwimming()))
				{
					if (m_MovementSpeedLimit > 1)
						SetMovementSpeedLimit(1);

					return;
				}

				if (speedLimit > 2 && vector.DistanceSq(GetPosition(), ai.GetPosition()) > 900)
				{
					if (m_MovementSpeedLimit > 2)
						SetMovementSpeedLimit(2);

					return;
				}
			}
		}

		if (m_MovementSpeedLimit < speedLimit)
			SetMovementSpeedLimit(speedLimit);
	}

	void eAI_SetSpeedLimitPreference(int speedLimit)
	{
		SetMovementSpeedLimit(speedLimit);
		m_eAI_SpeedLimitPreference = m_MovementSpeedLimit;
	}

	int GetMovementSpeedLimit()
	{
		return m_MovementSpeedLimit;
	}

	int GetMovementSpeedLimitUnderThreat()
	{
		return m_MovementSpeedLimitUnderThreat;
	}

	float eAI_GetCurrentMovementSpeedLimit()
	{
		auto cmd = GetCommand_Move();
		if (cmd)
			return m_eAI_CommandMove.GetSpeedLimit();

		return -1;
	}

	void OverrideMovementSpeed(bool pActive, int pSpeed)
	{
		m_MovementSpeedActive = pActive;
		m_MovementSpeed = pSpeed;
	}

	void OverrideMovementDirection(bool pActive, float pDirection)
	{
		if (!pActive && m_SideStepAngle)
			return;

		m_MovementDirectionActive = pActive;
		m_MovementDirection = pDirection;
	}
	static int eAI_GetMovementSpeed(eAIMovementSpeed pSpeed)
	{
		switch (pSpeed)
		{
			case eAIMovementSpeed.RANDOM:
				return Math.RandomIntInclusive(0, 3);
			case eAIMovementSpeed.RANDOM_NONSTATIC:
				return Math.RandomIntInclusive(1, 3);
		}

		return pSpeed;
	}

	bool eAI_IsUnreachable(float minDistance, vector position)
	{
		vector aiPosition = GetPosition();

		if (Math.AbsFloat(position[1] - aiPosition[1]) > minDistance && Math.IsPointInCircle(position, minDistance, aiPosition))
			return true;

		return false;
	}

	void eAI_ThreatOverride(EntityAI entity, bool state)
	{
		if (state)
			m_eAI_ThreatOverride[entity] = true;
		else
			m_eAI_ThreatOverride.Remove(entity);
	}

	bool eAI_GetThreatOverride(EntityAI entity)
	{
		return m_eAI_ThreatOverride[entity];
	}

	void eAI_PurgeThreatOverride()
	{
		int currentCount = m_eAI_ThreatOverride.Count();

		if (currentCount < 10)
			return;

		array<EntityAI> threats = {};

		foreach (EntityAI threat, bool state: m_eAI_ThreatOverride)
		{
			if (threat)
				threats.Insert(threat);
		}

		int nullCount = currentCount - threats.Count();

		if (nullCount > 0)
		{
			vector position = GetPosition();

			m_eAI_ThreatOverride.Clear();

			foreach (EntityAI candidate: threats)
			{
				if (Math.IsPointInCircle(position, 1000.0, candidate.GetPosition()))
					m_eAI_ThreatOverride[candidate] = true;
			}
		}
	}

	bool eAI_HasLOS()
	{
		return m_eAI_HasLOS;
	}

	bool eAI_HasLOS(eAITargetInformation info)
	{
		if (!info.IsEntity())
			return false;

		eAITarget target = info.GetTargetForAI(this);
		if (target)
			return target.m_LOS;

		return false;
	}

	float eAI_GetThreatDistanceFactor(float distance)
	{
		float threatDistanceThreshold = 900;

		if (distance > threatDistanceThreshold)
			return ExpansionMath.PowerConversion(1100, threatDistanceThreshold, distance, 0.0, 1.0, 2.0);

		return 1.0;
	}

	void eAI_ForceSideStep(float duration, Object obj = null, float angle = 0.0, bool allowBackPedaling = true, bool sideStepCancelOnLOS = false)
	{
		bool blockedLeft;
		bool blockedRight;

		if (angle == 0.0)
		{
			auto cmd = GetCommand_Move();
			if (cmd)
			{
				blockedLeft = m_eAI_CommandMove.CheckBlockedLeft();
				blockedRight = m_eAI_CommandMove.CheckBlockedRight();
			}

			vector transform[4];
			GetTransform(transform);

			if (!blockedLeft)
				blockedLeft = m_PathFinding.IsBlocked(transform[3], transform[3] + (-0.5 * transform[0]));

			if (!blockedRight)
				blockedRight = m_PathFinding.IsBlocked(transform[3], transform[3] + (0.5 * transform[0]));

			if (blockedLeft && blockedRight)
			{
				if (!allowBackPedaling)
					return;

				m_SideStepAngle = -180;
			}
			else if (blockedLeft || (!blockedRight && Math.RandomIntInclusive(0, 1)))
			{
				m_SideStepAngle = 90;
			}
			else
			{
				m_SideStepAngle = -90;
			}
		}
		else
		{
			m_SideStepAngle = angle;
		}

		m_eAI_SideStepObject = obj;
		m_eAI_SideStepTimeout = duration;
		m_eAI_SideStepCancelOnLOS = sideStepCancelOnLOS;

		if (m_EmoteManager.IsEmotePlaying())
			m_EmoteManager.ServerRequestEmoteCancel();

		OverrideMovementDirection(true, m_SideStepAngle);
	}

	void eAI_CancelSidestep()
	{
		m_eAI_SideStepTimeout = 0.0;
		eAI_ResetSidestep();
	}

	protected void eAI_ResetSidestep()
	{
		m_SideStepAngle = 0.0;
		m_eAI_SideStepCancelOnLOS = false;
		m_eAI_SideStepObject = null;

		OverrideMovementDirection(true, 0.0);
		m_eAI_ResetMovementDirectionActive = true;

		m_PathFinding.StopOverride();
	}

	bool eAI_IsSideStepping()
	{
		return m_eAI_SideStepTimeout > 0.0;
	}

	float eAI_GetSideStepTimeout()
	{
		return m_eAI_SideStepTimeout;
	}
	void eAI_JumpOrClimb()
	{
		m_JumpClimb.JumpOrClimb();
	}

	bool eAI_IsLargeGate(Object obj)
	{
		string type = obj.GetType();

		type.ToLower();

		if (type.IndexOf("gate") > -1)
		{
			vector minMax[2];
			if (obj.GetCollisionBox(minMax) && vector.DistanceSq(minMax[0], minMax[1]) > 9.0)
				return true;
		}

		return false;
	}

	ActionBase StartAction(typename actionType, ActionTarget target, ItemBase mainItem = null)
	{
		ActionBase action = m_eActionManager.GetAction(actionType);

		if (!mainItem)
			mainItem = GetItemInHands();

		m_eActionManager.PerformActionStart(action, target, mainItem);

		m_eAI_PositionOverrideTimeout = 0;

		return action;
	}

	ActionBase StartActionObject(typename actionType, Object target, ItemBase mainItem = null)
	{
		ActionTarget actionTgt = new ActionTarget(target, null, -1, vector.Zero, -1.0);
		return StartAction(actionType, actionTgt, mainItem);
	}
	bool HandleBuildingDoors(eAICommandMove hcm, float pDt)
	{
		//if (!m_PathFinding.IsDoor())
			//return;

		if (IsRestrained())
			return false;

		float speed = hcm.GetCurrentMovementSpeed();
		if (m_eAI_PositionIsFinal && Math.Round(speed) == 0.0)
			return false;

		int time = g_Game.GetTime();

		if (time - m_eAI_LastDoorInteractionTime < 1000)
			return false;

		BuildingBase cover;
		if (!hcm.IsBlockedByBuildingWithDoor() && (!m_eAI_CurrentCoverObject || !Class.CastTo(cover, m_eAI_CurrentCoverObject) || cover.Expansion_IsEnterable()))
			return false;

		vector position = m_ExTransformPlayer[3] + (m_ExTransformPlayer[1] * 1.1);
		vector direction = m_ExTransformPlayer[2];

		float fwdBwd = 1.0;
		if (Math.AbsFloat(Expansion_GetMovementAngle()) > 135.0)
			fwdBwd = -1.0;

		vector p0 = position;
		vector p1 = position + (direction * 1.5 * Math.Clamp(speed, 1.0, 1.333) * fwdBwd);

		if (GetWeaponManager().IsRunning())
		{
		#ifdef DIAG_DEVELOPER
			Expansion_DebugObject(22222, "0 0 0", "ExpansionDebugSphereSmall_White");
			Expansion_DebugObject(22223, p1, "ExpansionDebugSphereSmall_Black", direction, position);
		#endif
			return false;
		}

#ifdef EXTRACE_DIAG
		auto trace = EXTrace.Profile(EXTrace.AI_PROFILE, this, "CommandHandler(13) -> HandleBuildingDoors");
#endif

	#ifdef DIAG_DEVELOPER
		Expansion_DebugObject(22222, p1, "ExpansionDebugSphereSmall_White", direction, position);
		Expansion_DebugObject(22223, "0 0 0", "ExpansionDebugSphereSmall_Black");
	#endif

		RaycastRVParams params = new RaycastRVParams(p0, p1, this, 0.5);
		array<ref RaycastRVResult> results();
		array<Object> excluded();
		excluded.Insert(this);

		params.with = this;
		params.flags = CollisionFlags.ALLOBJECTS;
		params.type = ObjIntersectView;

		if (!DayZPhysics.RaycastRVProxy(params, results, excluded))
			return false;

		int inactiveDuration;  //! ms

		foreach (auto result : results)
		{
			BuildingBase building;
			if (!Class.CastTo(building, result.obj))
				continue;

			int doorIndex = building.GetDoorIndex(result.component);
			if (doorIndex == -1)
			{
				//! Why has everything to be a special case in vanilla >:(
				//! GetDoorIndex can not be used, and these special doors are not included in GetDoorCount either
				if (building.Expansion_IsUndergroundEntrance() && building.GetActionComponentName(result.component).IndexOf("door") == 0)
				{
					Land_Underground_EntranceBase entrance;
					//! CanManipulate == true means door is closed
					if (Class.CastTo(entrance, building) && entrance.CanManipulate())
					{
						if (m_PathFinding.m_IsBlocked)
						{
							m_PathFinding.m_IsUnreachable = true;
							m_PathFinding.m_IsTargetUnreachable = true;
						}

						if (m_PathFinding.m_AllowClosedDoors)
							m_PathFinding.AllowClosedDoors(false, 15.0);
					}

					break;
				}

				continue;
			}

			bool isDoorOpen = building.IsDoorOpen(doorIndex);
			bool isEnterable = building.Expansion_IsEnterable();

			bool canInteract = true;

			if (isDoorOpen)
			{
				if (isEnterable)
				{
					Object blockingObject = hcm.GetBlockingObject();
					if (blockingObject || !hcm.CheckBlocked())
					{
						//! If we're blocked less than 3 seconds, and there's enough free space to move fwd/left/right,
						//! and speed limit is zero or movement speed is zero or velocity is higher than 0.001 m/s, ignore opened door
						bool blockedShortTime = m_eAI_BlockedTime < 3.0;
						bool hasFreeSpace = false;
						if (CheckFreeSpace(vector.Forward, 0.3, false))
							hasFreeSpace = true;
						else if (CheckFreeSpace(vector.Aside, 0.5, false))
							hasFreeSpace = true;
						else if (CheckFreeSpace(-vector.Aside, 0.5, false))
							hasFreeSpace = true;

						bool canRetryBlockedMove = false;
						if (m_MovementSpeedLimit == 0)
							canRetryBlockedMove = true;
						else if (Math.Floor(speed) == 0.0)
							canRetryBlockedMove = true;
						else if (m_eAI_PositionTime < 1.5)
							canRetryBlockedMove = true;

						if (blockedShortTime && hasFreeSpace && canRetryBlockedMove)
						{
							if (hcm.IsBlocked() && m_eAI_BlockedTime > 0.5)
							{
								m_PathFinding.ForceRecalculate(true);
								break;
							}
							else if (m_eAI_BlockedTime < 4.0 - speed)
							{
								break;
							}
						}
					}
				}

				if (!building.CanDoorBeClosed(doorIndex))
				{
					break;
				}
			}
			else if (!isEnterable)
			{
				//! Do not open doors on structures that cannot be entered, e.g. wrecks (less chance of getting stuck on them when closed)
				break;
			}
			//! @note vanilla Building::CanDoorBeOpened is implemented awkardly: If you pass `true` in the 2nd argument (`checkIfLocked`),
			//! then it will return false if the door is closed and locked.
			//! But if you pass in `false` for `checkIfLocked` (which is also the default), then it will return false if the door is
			//! closed and *not* locked, which is unexpected.
			//! The only reason we use it to begin with is because mods can override it.
			else if (building.IsDoorLocked(doorIndex) && building.CanDoorBeOpened(doorIndex, false))
			{
				bool canHitToUnlock = building.GetAllowDamage();

			#ifdef EXPANSIONMOD
				if (GetExpansionSettings().GetGeneral().DisableShootToUnlock)
					canHitToUnlock = false;
			#endif

				if (canHitToUnlock && (building.GetLockCompatibilityType(doorIndex) & (1 << EBuildingLockType.LOCKPICK)))
				{
					//auto info = building.eAI_GetDoorTargetInformation(doorIndex, result.pos);
					auto info = building.eAI_GetDoorTargetInformation(doorIndex, building.GetDoorSoundPos(doorIndex));
					info.AddAI(this);
					break;
				}
				else
				{
					//! If door cannot be opened and path is blocked, stop moving
					if (m_PathFinding.m_IsBlocked)
					{
						m_PathFinding.m_IsUnreachable = true;
						m_PathFinding.m_IsTargetUnreachable = true;
					}

					canInteract = false;
				}
			}
			else if (!building.CanDoorBeOpened(doorIndex, true))
			{
				//! If door cannot be opened and path is blocked, stop moving
				if (m_PathFinding.m_IsBlocked)
					m_PathFinding.m_IsUnreachable = true;

				canInteract = false;
			}

			int timeTresh = building.eAI_GetDoorAnimationTime(doorIndex) * 1000;
			if (time - building.m_eAI_LastDoorInteractionTime[doorIndex] < timeTresh + 500)
				break;

			/**
			 * TODO: Use FSM events to notify that the door should be opened. 
			 * TODO: Add support for animation action logic to FSM states
			 * TODO: FSM file just for doors. Opening, picking, breaking, closing. Vehicles and Buildings.
			 * 
			 *	GetFSM().Start("OpenDoor")
			 * 
			 * Events are stored in the transitions
			 */

			//! Decrease chance of AI getting stuck between wall and opened door by temporarily stopping before opening
			if (!isDoorOpen && !m_eAI_Halt)
			{
				eAI_SetHalt(true);
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(eAI_SetHalt, timeTresh * 0.65, false, false);
			}

			//! Prevent AI getting pushed by opening/closing door by temporarily disabling collision
			/*
			PhxInteractionLayers layer = dBodyGetInteractionLayer(this);
			if (layer != PhxInteractionLayers.AI && !m_eAI_ResetInteractionLayer)
			{
				m_eAI_ResetInteractionLayer = true;

				if (m_eAI_Halt)
					inactiveDuration = Math.Min(timeTresh, 1000);
				else
					inactiveDuration = ExpansionMath.PowerConversion(1, 3, m_MovementSpeedLimit, Math.Min(timeTresh, 1000), timeTresh * 0.2, 2.6);

				dBodySetInteractionLayer(this, PhxInteractionLayers.AI);
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(eAI_dBodySetInteractionLayer, inactiveDuration, false, layer);
			}
			*/

			if (isDoorOpen)
			{
				StartActionObject(eAIActionCloseDoorsDummy, building);
				building.CloseDoor(doorIndex);
			}
			else
			{
				StartActionObject(eAIActionOpenDoorsDummy, building);
				//! @note if the door cannot be opened because it is locked,
				//! the game will prevent opening and play the usual "doorknob rattle" sound
				if (canInteract || building.IsDoorLocked(doorIndex))
					building.OpenDoor(doorIndex);
			}

			if (canInteract)
			{
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(m_PathFinding.ForceRecalculate, 34, false, true);
			}
			else
			{
				if (m_PathFinding.m_AllowClosedDoors)
					m_PathFinding.AllowClosedDoors(false, 15.0);
			}

			building.m_eAI_LastDoorInteractionTime[doorIndex] = time;  //! To prevent any AI from interacting with this specific door (cooldown)
			m_eAI_LastDoorInteractionTime = time;  //! To prevent this AI from interacting with any door (cooldown)
			return canInteract;
		}

		return false;
	}

	void eAI_SetHalt(bool halt)
	{
		m_eAI_Halt = halt;
	}

	void eAI_dBodySetInteractionLayer(PhxInteractionLayers layer)
	{
		m_eAI_ResetInteractionLayer = false;

		if (!IsDamageDestroyed())
			dBodySetInteractionLayer(this, layer);
	}

	void eAI_UpdateVisitedBuildings()
	{
		if (GetGroup().GetWaypointBehaviour() == eAIWaypointBehavior.ROAMING && GetGroup().GetFormationLeader() == this)
		{
			vector begPos = GetPosition() - "0 0.1 0";  //! Start raycast slightly under ground in case building has no roof
			//! Raycast up instead of down because not all buildings have floors (e.g. sheds)
			//! Raycast height is chosen so that it can still hit ceiling of Land_Cementworks_Hall2 from floor
			vector endPos = GetPosition() + "0 11 0";
			vector hitPosition;
			vector hitNormal;
			int contactComponent;
			set<Object> results = new set<Object>;

			if (DayZPhysics.RaycastRV(begPos, endPos, hitPosition, hitNormal, contactComponent, results, null, this, false, false, ObjIntersectGeom, 0.0, CollisionFlags.ALLOBJECTS))
			{
				BuildingBase building;
				foreach (Object obj: results)
				{
					if (Class.CastTo(building, obj))
					{
						GetGroup().m_VisitedBuildings.Insert(building);
					#ifdef DIAG_DEVELOPER
						EXTrace.Print(EXTrace.AI, this, "Looted " + ExpansionStatic.GetDebugInfo(building));
					#endif
						break;
					}
				}
			}
		}
	}
	void eAI_OnMovementUpdate(float pDt)
	{
		int actualCommandID = GetCurrentCommandID();

		if (!m_eAI_SkipScript && eAI_IsLocomotionCmd(actualCommandID))
		{
			eAICommandMove ecm;

			int performCommand;
			if (actualCommandID != DayZPlayerConstants.COMMANDID_VEHICLE)
				performCommand = EAI_COMMANDID_MOVE;

			ecm = m_eAI_CommandMove;

			switch (performCommand)
			{
			case EAI_COMMANDID_MOVE:
				if (m_eAI_IsOnLadder)
				{
					//! Do nothing
				}
				else if (m_eAI_CommandMove.m_CDT > 0)
				{
					//! Do nothing
				}
				else if (AI_HANDLEDOORS && HandleBuildingDoors(ecm, pDt))
				{
					break;
				}
				else if (AI_HANDLEVAULTING && (m_eAI_JumpClimb || HandleVaulting(ecm, pDt)))
				{
					//! Unsafe to call animation-related commands outside cmdhandler, can break character animation
					m_eAI_JumpClimb = true;
					break;
				}
				else if (m_PathFinding.m_IsJumpClimb && m_PathFinding.m_AllowJumpClimb && !m_PathFinding.m_IsBlockedPhysically && ecm.IsBlocked() && m_eAI_BlockedTime > pDt && !IsSwimming())
				{
					//! If we are still blocked, find way around obstacle
					//! (need to be blocked for at least one commandhandler tick
					//! else it messes with climbing after door interaction)
					m_PathFinding.SetAllowJumpClimb(false, 15.0);
				}
				
				float speedLimit;
				if (m_eAI_Halt)
					speedLimit = 0.0;
				else if (eAI_IsSideSteppingVehicle())
					speedLimit = 3.0;
				else if (m_eAI_CurrentThreatToSelfActive >= 0.4 || (m_FSM && m_FSM.IsInState("Flank")))
					speedLimit = m_MovementSpeedLimitUnderThreat;
				else if (m_eAI_IsInDangerByArea)
					speedLimit = 3.0;
				else
					speedLimit = m_MovementSpeedLimit;

				if (m_WeaponRaised)
					speedLimit = Math.Min(speedLimit, 2);

				vector lookTargetRelAngles = m_eAI_LookDirectionTarget_ModelSpace.VectorToAngles();
				vector aimTargetRelAngles = m_eAI_AimDirectionTarget_ModelSpace.VectorToAngles();

				float turnTarget;
				bool setTurnTarget;

				if (m_eAI_TurnTargetActive)
				{
					turnTarget = m_eAI_TurnTarget;
					setTurnTarget = true;
				}
				else if (m_eAI_AimDirection_Recalculate || (Math.AbsFloat(m_eAI_AimRelAngles[0]) > 90 && aimTargetRelAngles[0] != 0))
				{
					turnTarget = GetAimDirectionTarget().VectorToAngles()[0];
					setTurnTarget = true;

					if (!m_eAI_LookDirection_Recalculate)
					{
						lookTargetRelAngles[0] = 0.0;
						m_eAI_LookDirectionTarget_ModelSpace = lookTargetRelAngles.AnglesToVector();
					}

					if (!m_eAI_AimDirection_Recalculate)
					{
						aimTargetRelAngles[0] = 0.0;
						m_eAI_AimDirectionTarget_ModelSpace = aimTargetRelAngles.AnglesToVector();
					}
				}
				else if (IsInherited(eAINPCBase) && ecm.GetCurrentMovementSpeed() == 0.0)
				{
					turnTarget = GetOrientation()[0];
					setTurnTarget = true;
				}
				else if (speedLimit == 0 && GetGroup().GetWaypointBehaviour() == eAIWaypointBehavior.HALT)
				{
					turnTarget = m_eAI_DefaultLookAngle;
					setTurnTarget = true;
				}

				if (setTurnTarget && !m_eAI_IsOnLadder)
					ecm.SetTurnTarget(turnTarget, m_eAI_TurnTargetActive);

				if (GetInputController().LimitsIsSprintDisabled())
				{
					speedLimit = Math.Min(speedLimit, 2);
				}

				if (!m_MovementDirectionActive)
					m_MovementDirection = 0;

				ecm.SetSpeedOverrider(m_MovementSpeedActive);
				ecm.SetTargetSpeed(m_MovementSpeed);
				ecm.SetSpeedLimit(speedLimit);
				ecm.SetTargetDirection(m_MovementDirection, m_MovementDirectionActive);

				if (ecm.GetCurrentMovementSpeed() >= 0.5 && m_eAI_PositionTime > 4.98 && Expansion_IsAnimationIdle())
				{
					vector playerPosition = m_ExTransformPlayer[3];

				#ifdef DIAG_DEVELOPER
					string pos = ExpansionStatic.VectorToString(playerPosition);
					string msg = "Movement speed " + ecm.GetCurrentMovementSpeed();
					msg = msg + " but not moving for " + m_eAI_PositionTime;
					msg = msg + " s " + Debug.GetDebugName(this);
					msg = msg + " (pos=" + pos + ")";
					msg = msg + ", item in hands " + ExpansionStatic.GetDebugInfo(GetItemInHands());

					EXPrint(this, msg);

					msg.Replace("<", "Р В Р’В Р В РІР‚В Р В Р’В Р Р†Р вЂљРЎв„ўР В Р вЂ Р Р†Р вЂљРЎвЂєР Р†Р вЂљРІР‚Сљ");
					msg.Replace(">", "Р В Р’В Р В РІР‚В Р В Р’В Р Р†Р вЂљРЎв„ўР В Р Р‹Р Р†Р вЂљРЎСљ");
					ExpansionNotification("NOT MOVING", msg).Error();
				#endif

					Object blockingObject = m_eAI_CommandMove.GetBlockingObject();
					if ((blockingObject && !blockingObject.IsPlainObject() && !blockingObject.IsRock()) || m_PathFinding.m_PathGlueIdx != -1)
					{
						vector waypoint = m_eAI_CommandMove.GetWaypoint();
						if (waypoint != playerPosition && waypoint != vector.Zero)
						{
							m_eAI_PositionTime = 0;
							//! Variant A: do NOT teleport AI when stuck. Official Expansion uses this as a recovery hack,
							//! but in this port it causes visible snapping because path/target updates are not 1:1 with Expansion.
							eAI_ResetPathfinding();

						#ifdef DIAG_DEVELOPER
							pos = ExpansionStatic.VectorToString(waypoint);
							float dist = vector.Distance(playerPosition, waypoint);
							msg = "Skipping teleport of " + Debug.GetDebugName(this) + " to " + pos + " (distance " + dist + " m)";

							EXPrint(this, msg);

							msg.Replace("<", "Р В Р’В Р В РІР‚В Р В Р’В Р Р†Р вЂљРЎв„ўР В Р вЂ Р Р†Р вЂљРЎвЂєР Р†Р вЂљРІР‚Сљ");
							msg.Replace(">", "Р В Р’В Р В РІР‚В Р В Р’В Р Р†Р вЂљРЎв„ўР В Р Р‹Р Р†Р вЂљРЎСљ");
							ExpansionNotification("NO TELEPORT", msg).Error();
						#endif
						}
						else
						{
							m_eAI_PositionTime = 0;
							eAI_ResetPathfinding();
						}
					}
					else
					{
						m_eAI_PositionTime = 0;
						eAI_ResetPathfinding();
					}
				}

				break;
			default:
			#ifdef DIAG_DEVELOPER
				Expansion_DebugObject(22222, "0 0 0", "ExpansionDebugSphereSmall_White");
				Expansion_DebugObject(22223, "0 0 0", "ExpansionDebugSphereSmall_Black");
			#endif
				break;
			}
		}
	}

	EntityAI eAI_SetHasProjectileWeaponInHands()
	{
		EntityAI entityInHands = GetHumanInventory().GetEntityInHands();

		if (entityInHands && entityInHands.IsWeapon())
			m_eAI_HasProjectileWeaponInHands = true;
		else
			m_eAI_HasProjectileWeaponInHands = false;

		return entityInHands;
	}

	void eAI_CheckIsInCover()
	{
		m_eAI_IsInCover = false;
		if (m_eAI_CurrentCoverObject && m_eAI_CurrentCoverPosition != vector.Zero)
		{
			if (vector.DistanceSq(GetPosition(), m_eAI_CurrentCoverPosition) <= 2.25)
				m_eAI_IsInCover = true;
		}
	}

	void eAI_CheckIsInDangerByArea()
	{
		m_eAI_IsInDangerByArea = eAI_IsDangerousAltitude();
	}

	void eAI_CheckIsAttachedToMovingParent()
	{
		m_eAI_IsAttachedToMovingParent = false;
	}

	void UpdateTargets(float pDt, EntityAI entityInHands = null)
	{
		eAIGroup group = GetGroup();
		if (!group)
			return;

		if (eAI_IsPassive())
			return;

		eAIFaction faction = group.GetFaction();
		if (!faction)
			return;

		vector center = GetPosition();

		m_eAI_UpdateTargetsTick += pDt;
		if (m_eAI_UpdateTargetsTick < Math.RandomFloat(0.1, 0.2))
			return;

		m_eAI_UpdateTargetsTick = 0;
		m_eAI_CurrentPotentialTargetIndex = 0;

		m_eAI_PotentialTargetEntities.Clear();

		vector min = Vector(center[0] - 30, center[1] - 30, center[2] - 30);
		vector max = Vector(center[0] + 30, center[1] + 30, center[2] + 30);

		bool updateCoverObjects;
		if (m_eAI_ShouldTakeCover)
			updateCoverObjects = true;
		else if (group.GetWaypointBehaviour() == eAIWaypointBehavior.ROAMING && group.GetFormationLeader() == this)
			updateCoverObjects = true;

		int flags = QueryFlags.DYNAMIC;
		if (updateCoverObjects && (m_eAI_UpdateNearTargetsCount % 33 == 0 || m_eAI_UpdatePotentialCoverObjects))
		{
			m_eAI_PotentialCoverObjects.Clear();
			DayZPlayerUtils.PhysicsGetEntitiesInBox(min, max, m_eAI_PotentialTargetEntities);
			m_eAI_UpdatePotentialCoverObjects = false;

			for (int i = m_eAI_PotentialTargetEntities.Count() - 1; i >= 0; i--)
			{
				EntityAI potentialCoverObj = m_eAI_PotentialTargetEntities[i];
				if (!potentialCoverObj)
					continue;

				bool isPotentialCoverObject = false;

				if (potentialCoverObj.IsBuilding())
				{
					string type = potentialCoverObj.GetType();
					if ((type.IndexOf("Land_") == 0 || type.IndexOf("Wreck_") == 0) && type.IndexOf("Gate") == -1)
						isPotentialCoverObject = true;
				}
				else if (!m_eAI_ShouldTakeCover)
				{
					continue;
				}
				else if (potentialCoverObj.IsRock() || potentialCoverObj.IsTree() || potentialCoverObj.IsTransport())
				{
					isPotentialCoverObject = true;
				}

				if (isPotentialCoverObject)
				{
					m_eAI_PotentialCoverObjects.Insert(potentialCoverObj);
					m_eAI_PotentialTargetEntities.Remove(i);
				}
			}
		}
		else
		{
			DayZPlayerUtils.SceneGetEntitiesInBox(min, max, m_eAI_PotentialTargetEntities, flags);
		}

		m_eAI_UpdateNearTargetsCount++;
		eAI_PurgeThreatOverride();

		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (!player)
				continue;

			if (player == this)
				continue;

			if (!player.IsPlayerLoaded())
				continue;

			if (!player.IsAlive())
				continue;

			if (player.IsDamageDestroyed())
				continue;

			if (player.IsAI() && player.eAI_IsPassive())
				continue;

			float visibilityLimit = eAI_GetVisibilityDistanceLimit(player);
			if (!Math.IsPointInCircle(center, visibilityLimit, player.GetPosition()))
				continue;

			if (!faction.IsObserver())
			{
				EntityAI playerEntity = player;
				if (m_eAI_PotentialTargetEntities.Find(playerEntity) == -1)
					m_eAI_PotentialTargetEntities.Insert(playerEntity);
			}
		}

		PlayerBase playerThreat;
		bool playerIsEnemy;

		while (m_eAI_CurrentPotentialTargetIndex < m_eAI_PotentialTargetEntities.Count())
		{
			EntityAI entity = m_eAI_PotentialTargetEntities[m_eAI_CurrentPotentialTargetIndex++];
			if (!entity)
				continue;

			if (Class.CastTo(playerThreat, entity))
			{
				if (playerThreat == this)
					continue;

				if (playerThreat.eAI_IsPassive())
					continue;

				if (!playerThreat.IsPlayerLoaded())
					continue;

				playerIsEnemy = PlayerIsEnemy(playerThreat, true);
				if (!playerIsEnemy)
					continue;
			}
			else if (entity.IsInherited(DayZPlayerImplement))
			{
				continue;
			}

			eAITargetInformation info = eAITargetInformation.GetTargetInformation(entity);
			if (!info)
				continue;

			bool created;
			eAITarget target = info.AddAI(this, -1, true, created);
			if (!target)
				continue;

			target.UpdateThreat(false);
		}
	}

	bool eAI_RemoveTargets()
	{
		int count = m_eAI_Targets.Count();

		for (int i = count - 1; i >= 0; i--)
		{
			eAITarget target = m_eAI_Targets[i];
			if (target && target.ShouldRemove())
				target.Remove();
		}

		return count != m_eAI_Targets.Count();
	}

	void eAI_PrioritizeTargets()
	{
		float threat;
		float maxThreat = 0.099999;
		int maxThreatIndex;

		m_eAI_NoiseTarget = 0;
		m_eAI_AcuteDangerTargetCount = 0;
		m_eAI_AcuteDangerPlayerTargetCount = 0;

		foreach (int i, eAITarget target : m_eAI_Targets)
		{
			if (!target)
				continue;

			threat = target.GetThreat();
			if (threat > maxThreat)
			{
				maxThreatIndex = i;
				maxThreat = threat;
			}
			else if (target.IsNoise() && !IsUnconscious())
			{
				m_eAI_NoiseTarget = i;
			}

			if (threat >= 0.4 && target.IsAcuteDanger())
				eAI_UpdateAcuteDangerTargetCount(target, 1);
		}

		m_eAI_TargetChanged = false;

		if (maxThreatIndex > 0 && maxThreatIndex < m_eAI_Targets.Count())
		{
			eAITarget selectedTarget = m_eAI_Targets[maxThreatIndex];
			eAITarget previousTarget = m_eAI_Targets[0];
			m_eAI_Targets[0] = selectedTarget;
			if (m_eAI_NoiseTarget == maxThreatIndex)
				m_eAI_NoiseTarget = 0;
			else if (previousTarget.IsNoise() && !IsUnconscious())
				m_eAI_NoiseTarget = maxThreatIndex;
			m_eAI_Targets[maxThreatIndex] = previousTarget;
			m_eAI_TargetChanged = true;
			m_eAI_SyncCurrentTarget = true;
		}
	}

	void eAI_UpdateAcuteDangerTargetCount(eAITarget target, int delta)
	{
		m_eAI_AcuteDangerTargetCount = m_eAI_AcuteDangerTargetCount + delta;
		if (target && target.IsPlayer())
			m_eAI_AcuteDangerPlayerTargetCount = m_eAI_AcuteDangerPlayerTargetCount + delta;
	}

	void eAI_UpdateLeanTarget()
	{
		eAITarget target = GetTarget();
		if (!target)
			return;

		vector p1 = GetPosition();
		vector p2 = p1;
		if (m_PathFinding)
			p2 = p1 + m_PathFinding.m_PathSegmentDirection;

		vector p = target.GetPosition();
		if (vector.DistanceSq(p1, p) < vector.DistanceSq(p2, p))
		{
			p1 = p2;
			p2 = GetPosition();
		}

		if (ExpansionMath.Side(p1, p2, p) < 0)
			m_eAI_LeanTarget = -1.0;
		else
			m_eAI_LeanTarget = 1.0;

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(eAI_ResetLeanTarget, Math.RandomInt(1000, 2000), false);
	}

	void eAI_ResetLeanTarget()
	{
		m_eAI_LeanTarget = 0.0;
	}

	float eAI_CalculateFOVHalfAngleH(float dist)
	{
		float nearHalfAngleH = Math.Max(m_eAI_FOVNear_HalfAngleH, 95.0);
		float farHalfAngleH = Math.Max(m_eAI_FOVFar_HalfAngleH, 45.0);

		if (farHalfAngleH == nearHalfAngleH || dist <= m_eAI_FOVNear_DistThreshold)
			return nearHalfAngleH;

		float farDistThresh = Math.Max(m_eAI_FOVFar_DistThreshold, 60.0);
		if (dist < farDistThresh)
		{
			float range = farDistThresh - m_eAI_FOVNear_DistThreshold;
			if (range <= 0.0)
				return farHalfAngleH;

			float t = dist / range;
			if (t < 0.0)
				t = 0.0;
			if (t > 1.0)
				t = 1.0;

			float exponent = m_eAI_FOVTransitionExponent;
			if (exponent <= 0.0)
				exponent = 1.0;
			t = Math.Pow(t, exponent);
			return ExpansionMath.LinearConversion(0.0, 1.0, t, nearHalfAngleH, farHalfAngleH);
		}

		return farHalfAngleH;
	}

	float eAI_GetVisibilityDistance(float distance, eAITarget target)
	{
		if (distance > m_Expansion_VisibilityDistThreshold)
		{
			if (m_Expansion_Visibility <= m_eAI_NightVisibility)
			{
				if (target && target.IsLit())
					return Math.Min(distance, Math.Min(m_eAI_ThreatDistanceLimit, 250.0));
			}

			return Math.Min(distance, 1100.0 * m_Expansion_Visibility);
		}

		return distance;
	}

	float eAI_GetVisibilityDistanceLimit(PlayerBase player)
	{
		if (m_Expansion_Visibility <= m_eAI_NightVisibility)
		{
			if (player && player.eAI_IsLit())
				return Math.Min(m_eAI_ThreatDistanceLimit, 250.0);
		}

		return 1100.0 * m_Expansion_Visibility;
	}

	bool EnforceLOS(float pDt)
	{
		if (!GetGame())
			return false;

		if (!m_eAI_Targets.Count())
			return false;

		eAITarget target = GetTarget();
		if (!target)
			return false;

		EntityAI targetEntity = target.GetEntity();
		if (!targetEntity)
			return false;

		eAITargetInformationState state = target;

		float interval;
		if (s_eAI_FTO < 2)
			interval = LOS_CHECK_INTERVAL;
		else
			interval = LOS_CHECK_INTERVAL * (pDt / ExpansionWorld.AI_UPDATE_INTERVAL);

		if (m_eAI_LOSCheckDT >= interval)
			m_eAI_LOSCheckDT = 0;
		m_eAI_LOSCheckDT = m_eAI_LOSCheckDT + pDt;

		if (m_eAI_LOSCheckDT < interval && !m_eAI_TargetChanged)
			return state.m_LOS;

		if (IsUnconscious())
		{
			state.m_LOS = false;
			return false;
		}

		string boneName;
		if (IsRaised())
			boneName = "Neck";
		else
			boneName = "Head";

		vector begPos = GetBonePositionWS(GetBoneIndexByName(boneName));
		vector aimOffset = target.GetAimOffset();
		vector aimPosition = target.GetPosition(true) + aimOffset;
		vector dir = vector.Direction(begPos, aimPosition);
		float targetDist = dir.Length();
		float dist = eAI_GetVisibilityDistance(targetDist, target);
		dir.Normalize();
		vector endPos = begPos + dir * dist + dir * 0.5;

		Object parent;
		PlayerBase targetPlayer;
		Class.CastTo(targetPlayer, targetEntity);

		vector toTargetAngles = dir.VectorToAngles();
		float toTargetAngleH = toTargetAngles[0];

		if (targetPlayer)
		{
			parent = Object.Cast(targetPlayer.Expansion_GetParent());
			if (!parent)
			{
				vector lookAngles = GetLookDirection().VectorToAngles();
				float lookAngleH = lookAngles[0];
				float angleDiffH = ExpansionMath.AngleDiff2(toTargetAngleH, lookAngleH);
				float threshAngleH = eAI_CalculateFOVHalfAngleH(dist);
				if (Math.AbsFloat(angleDiffH) > threshAngleH)
				{
					state.m_LOS = false;
					return false;
				}
			}
		}

		bool isItemTarget = targetEntity.IsItemBase();

		vector contactPos;
		vector contactDir;
		int contactComponent;
		set<Object> results = new set<Object>;
		float radius = 0.05;
		bool hadLOS = state.m_LOS;

		state.m_LOS = DayZPhysics.RaycastRV(begPos, endPos, contactPos, contactDir, contactComponent, results, null, this, false, false, ObjIntersectView, radius);
		if (!state.m_LOS)
		{
			if (hadLOS && m_eAI_IsFightingFSM)
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(eAI_UpdateLeanTarget, 1000, false);

			return false;
		}

		state.m_LOS = false;

		bool isCreatureTarget = targetEntity.IsDayZCreature();
		if (results.Count() && results[0].IsBuilding())
		{
			results.Clear();
			DayZPhysics.RaycastRV(contactPos - dir * 0.1, endPos, contactPos, contactDir, contactComponent, results, null, this, false, false, ObjIntersectFire, radius);
		}

		float targetDistSq = targetDist * targetDist;
		float contactToTargetDistSq = vector.DistanceSq(contactPos, aimPosition);

		DayZPlayerImplement player;
		bool sideStep;

		Object hitObj;
		EntityAI hitEntity;
		EntityAI hitEntityRoot;
		bool isCreatureHit;
		foreach (Object obj : results)
		{
			hitObj = obj;

			if (Class.CastTo(hitEntity, obj))
			{
				hitEntityRoot = hitEntity.GetHierarchyRoot();
				if (hitEntityRoot)
				{
					isCreatureHit = hitEntityRoot.IsDayZCreature();
					obj = hitEntityRoot;
				}
			}

			float toObjAngleH = (obj.GetPosition() - begPos).VectorToAngles()[0];
			float toObjAngleDiffHAbs = Math.AbsFloat(ExpansionMath.AngleDiff2(toTargetAngleH, toObjAngleH));
			vector contactObjPos = obj.GetPosition();
			contactObjPos[1] = contactPos[1];
			float toContactObjDistSq = vector.DistanceSq(begPos, contactObjPos);

			if (obj.IsTree() && toObjAngleDiffHAbs <= 22.5 && targetDistSq > toContactObjDistSq && !state.m_SearchPositionUpdateCount)
			{
				if (!isItemTarget)
					sideStep = state.m_ThreatLevelActive >= 0.4;
				break;
			}

			if (obj == targetEntity)
			{
				state.m_LOS = true;
			}
			else if (parent && obj == parent)
			{
				state.m_LOS = true;
			}
			else
			{
				if (Class.CastTo(player, obj))
				{
					if (isItemTarget && !player.IsAlive() && targetEntity.GetHierarchyRootPlayer() == player)
						state.m_LOS = true;
					else if (!isItemTarget && eAI_GetCachedThreat(player.GetTargetInformation()) < 0.2)
						sideStep = m_eAI_IsFightingFSM;
				}
				else if (isCreatureTarget && isCreatureHit)
				{
					state.m_LOS = true;
				}
				else if (contactToTargetDistSq <= 0.04)
				{
					state.m_LOS = true;
				}
				else if (obj.IsTree() || obj.IsBush())
				{
					if (toObjAngleDiffHAbs > 22.5)
					{
						if (contactToTargetDistSq <= 16.0)
						{
							vector contactPosTmp;
							vector contactDirTmp;
							int contactComponentTmp;
							set<Object> resultsTmp = new set<Object>;

							if (DayZPhysics.RaycastRV(contactPos, contactPos + dir * 4.0, contactPosTmp, contactDirTmp, contactComponentTmp, resultsTmp, obj, this, false, false, ObjIntersectGeom))
							{
								Object hitObjTmp = resultsTmp[0];
								if (hitObjTmp == targetEntity || hitObjTmp == parent)
									state.m_LOS = true;
							}
							else
							{
								state.m_LOS = true;
							}
						}
					}
					else if (targetDistSq < toContactObjDistSq)
					{
						state.m_LOS = true;
					}
				}
			}

			break;
		}

		if (state.m_LOS)
			state.m_SearchOnLOSLost = true;

		if (state.m_LOS && !hadLOS)
		{
			m_eAI_PositionOverrideTimeout = 0.0;
			if (m_eAI_LeanTarget)
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(eAI_ResetLeanTarget, 1000, false);
		}
		else if (hadLOS && !state.m_LOS && m_eAI_IsFightingFSM)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(eAI_UpdateLeanTarget, 1000, false);
		}

		if (sideStep)
		{
			if (m_eAI_SideStepTimeout > 0 || m_MovementDirectionActive)
				return state.m_LOS;

			vector targetPos = state.m_SearchPosition;
			vector targetDirection = vector.Direction(GetPosition(), targetPos).Normalized();
			float dot = vector.Dot(GetDirection(), targetDirection);
			if (dot >= 0.75)
				eAI_ForceSideStep(1.5, hitObj, 0.0, false, true);
		}

		return state.m_LOS;
	}

	void ReactToThreatChange(float pDt, EntityAI entityInHands = null)
	{
		// Compatibility version for bqBots Expansion AI port.
		// This function is called from eAI_Targeting().
		// It handles only safe threat reaction logic and does not require full Expansion Core.

		if (m_eAI_CurrentThreatToSelfActive >= 0.4)
		{
			if (m_ThreatClearedTimeout <= 0)
			{
				m_ThreatClearedTimeout = Math.RandomFloat(1.0, 3.0);
			}
		}
		else
		{
			if (m_ThreatClearedTimeout > 0)
			{
				m_ThreatClearedTimeout = m_ThreatClearedTimeout - pDt;

				if (m_ThreatClearedTimeout < 0)
				{
					m_ThreatClearedTimeout = 0;
				}
			}
		}

		if (IsRaised() && !GetWeaponManager().IsRunning())
		{
			if (!CanRaiseWeapon())
			{
				eAI_RaiseWeapon(false);
			}
			else if (m_eAI_CurrentThreatToSelfActive < 0.2 && !m_eAI_IsInCover)
			{
				if (!entityInHands || !entityInHands.IsWeapon())
				{
					eAI_RaiseWeapon(false);
				}
			}
		}
	}

	void eAI_Targeting(float pDt, EntityAI entityInHands, bool checkLOS = true)
	{
		eAITarget previousTarget = GetTarget();
		UpdateTargets(pDt, entityInHands);
		if (eAI_RemoveTargets() || m_eAI_UpdateTargetsTick == 0)
			eAI_PrioritizeTargets();

		bool hadLOS = m_eAI_HasLOS;

		if (checkLOS)
			m_eAI_HasLOS = EnforceLOS(pDt);

		if (!m_eAI_HasLOS && m_eAI_NoiseTarget > 0 && m_eAI_NoiseTarget < m_eAI_Targets.Count())
		{
			eAITarget swapTarget = m_eAI_Targets[0];
			m_eAI_Targets[0] = m_eAI_Targets[m_eAI_NoiseTarget];
			m_eAI_Targets[m_eAI_NoiseTarget] = swapTarget;
			m_eAI_NoiseTarget = 0;
		}

		eAITarget currentTarget = GetTarget();
		if (!currentTarget)
		{
			if (previousTarget)
				m_eAI_Callbacks.OnNoMoreTargets();
		}
		else if (currentTarget != previousTarget)
		{
			m_eAI_Callbacks.OnTargetSelected(currentTarget);
		}

		if (m_eAI_HasLOS != hadLOS)
		{
			if (m_eAI_HasLOS)
				m_eAI_Callbacks.OnLOS(currentTarget);
			else
				m_eAI_Callbacks.OnLOSLost(currentTarget);
		}

		DetermineThreatToSelf(pDt);

		if (!IsUnconscious() && !IsRestrained())
			ReactToThreatChange(pDt, entityInHands);

		if (m_eAI_HasLOS && m_eAI_CurrentThreatToSelfActive > 0.1 && m_eAI_AcuteDangerTargetCount <= 1 && m_eAI_AcuteDangerPlayerTargetCount == 0)
			m_eAI_SilentAttackViabilityTime = m_eAI_SilentAttackViabilityTime + pDt;
		else
			m_eAI_SilentAttackViabilityTime = 0;
	}


	void eAI_HandleAiming(float pDt, bool hasLOS)
	{
		eAITarget target = GetTarget();
		if (target && target.GetEntity())
		{
			vector aimPosition = target.GetPosition(false) + target.GetAimOffset();
			LookAtPosition(aimPosition);
			if (m_WeaponRaised)
			{
				AimAtPosition(aimPosition);
				BQBot_SetAimTarget(aimPosition);
			}
		}
		else
		{
			LookAtDirection("0 0 1");
			AimAtDirection("0 0 1");
		}
	}

	void eAI_OnUpdate(float pDt)
	{
		if (!m_eAI_CurrentCommandID)
			return;

		GetTransform(m_ExTransformPlayer);
		vector position = GetPosition();
		m_eAI_SurfaceY = GetGame().SurfaceY(position[0], position[2]);
		m_eAI_CommandTime = m_eAI_CommandTime + pDt;

		if (m_WeaponRaised)
			m_WeaponRaisedTimer = m_WeaponRaisedTimer + pDt;
		else
			m_WeaponRaisedTimer = 0.0;

		eAI_CheckIsInCover();
		eAI_CheckIsInDangerByArea();
		eAI_CheckIsAttachedToMovingParent();

		EntityAI entityInHands = eAI_SetHasProjectileWeaponInHands();

		string eaiUpd = "bot=" + ToString();
		eaiUpd = eaiUpd + " dt=" + pDt.ToString();
		eaiUpd = eaiUpd + " cmd=" + m_eAI_CurrentCommandID.ToString();
		eaiUpd = eaiUpd + " pos=" + GetPosition().ToString();
		eaiUpd = eaiUpd + " hands=" + BQExpansionDiag.Obj(entityInHands);
		eaiUpd = eaiUpd + " raised=" + BQExpansionDiag.B(m_WeaponRaised);
		eaiUpd = eaiUpd + " raisedTimer=" + m_WeaponRaisedTimer.ToString();
		eaiUpd = eaiUpd + " inCover=" + BQExpansionDiag.B(m_eAI_IsInCover);
		eaiUpd = eaiUpd + " danger=" + BQExpansionDiag.B(m_eAI_IsInDangerByArea);
		eaiUpd = eaiUpd + " attached=" + BQExpansionDiag.B(m_eAI_IsAttachedToMovingParent);
		BQExpansionDiag.Log("EAI_UPDATE", eaiUpd);
		eAI_UpdatePositionOverrideTimeout(pDt);
		eAI_UpdatePotentialCoverObjects(pDt);
		eAI_Targeting(pDt, entityInHands);
		eAITarget dbgTarget = GetTarget();
		EntityAI dbgTargetEntity;
		vector dbgTargetPos = "0 0 0";
		if (dbgTarget)
		{
			dbgTargetEntity = dbgTarget.GetEntity();
			dbgTargetPos = dbgTarget.GetPosition();
		}
		string eaiTgtLog = "bot=" + ToString();
		eaiTgtLog = eaiTgtLog + " hasLOS=" + BQExpansionDiag.B(m_eAI_HasLOS);
		eaiTgtLog = eaiTgtLog + " threatActive=" + m_eAI_CurrentThreatToSelfActive.ToString();
		eaiTgtLog = eaiTgtLog + " threatAbs=" + m_eAI_CurrentThreatToSelf.ToString();
		eaiTgtLog = eaiTgtLog + " target=" + BQExpansionDiag.Obj(dbgTargetEntity);
		eaiTgtLog = eaiTgtLog + " targetPos=" + dbgTargetPos.ToString();
		eaiTgtLog = eaiTgtLog + " targetCount=" + m_eAI_Targets.Count().ToString();
		BQExpansionDiag.Log("EAI_TARGETING", eaiTgtLog);

		int simulationPrecision = 0;
		int actualCommandID = m_eAI_CurrentCommandID;
		if (actualCommandID != DayZPlayerConstants.COMMANDID_LADDER)
		{
			eAI_HandleAiming(pDt, m_eAI_HasLOS);

			bool shouldUpdatePath = m_PathFinding && actualCommandID != DayZPlayerConstants.COMMANDID_CLIMB && !m_eAI_IsAttachedToMovingParent && (m_eAI_IsInDangerByArea || m_PathFinding.GetOverride() || (m_FSM && !m_FSM.IsInState("Idle")));
			bool dbgIsClimb = actualCommandID == DayZPlayerConstants.COMMANDID_CLIMB;
			bool dbgHasOverride = m_PathFinding && m_PathFinding.GetOverride();
			bool dbgFsmIdle = m_FSM && m_FSM.IsInState("Idle");
			string pathGate = "bot=" + ToString();
			pathGate = pathGate + " update=" + BQExpansionDiag.B(shouldUpdatePath);
			pathGate = pathGate + " cmd=" + actualCommandID.ToString();
			pathGate = pathGate + " isClimb=" + BQExpansionDiag.B(dbgIsClimb);
			pathGate = pathGate + " attached=" + BQExpansionDiag.B(m_eAI_IsAttachedToMovingParent);
			pathGate = pathGate + " danger=" + BQExpansionDiag.B(m_eAI_IsInDangerByArea);
			pathGate = pathGate + " override=" + BQExpansionDiag.B(dbgHasOverride);
			pathGate = pathGate + " fsmIdle=" + BQExpansionDiag.B(dbgFsmIdle);
			BQExpansionDiag.Log("EAI_PATH_GATE", pathGate);
			if (shouldUpdatePath)
				m_PathFinding.OnUpdate(pDt, simulationPrecision);
		}
		else
		{
			LookAtDirection("0 0 1");
			AimAtDirection("0 0 1");
		}
	}

	void eAI_SchedulerTick(float pDt, int simulationPrecision)
	{
		bool isDisabled = GetIsSimulationDisabled();
		if (!isDisabled)
		{
			eAI_OnUpdate(pDt);
			eAI_OnWeaponAimUpdate();
		}

		eAI_UpdateFSM(pDt, simulationPrecision);

		if (!isDisabled && m_eAI_CommandMove)
		{
			m_eAI_CommandMove.AvoidObstacles(pDt);
			eAI_OnMovementUpdate(pDt);
		}
	}
	void eAI_OnWeaponAimUpdate()
	{
		// bqBots owns weapon firing/reload/aim pitch; keep Expansion scheduler contract without taking over weapons.
	}

	void eAI_UpdateFSM(float pDt, int simulationPrecision)
	{
		if (m_FSM && m_eAI_CommandTime > pDt)
			m_FSM.Update(pDt, simulationPrecision);
	}

	bool CanRaiseWeapon()
	{
		if (IsClimbing() || IsFalling() || IsSwimming() || m_eAI_IsOnLadder)
			return false;

		return true;
	}

	// Expansion positioning uses this to mark raised intent while taking/holding cover.
	// bqBots still owns the actual vanilla fire/pose bridge; this only preserves Expansion state semantics.
	void eAI_RaiseWeapon(bool up = true)
	{
		if (m_WeaponRaised != up)
			BQExpansionDiag.Log("RAISE", "bot=" + ToString() + " up=" + BQExpansionDiag.B(up));
		m_WeaponRaised = up;
		if (up)
		{
			BQBot_SetCombatPoseWanted(true);
		}
		else
		{
			BQBot_SetCombatPoseWanted(false);
			BQBot_SetVanillaTriggerHoldWanted(false);
			m_eAI_QueuedShots = 0;
		}
	}

	void eAI_ResetRaised()
	{
		m_WeaponRaised = false;
		m_WeaponRaisedPrev = false;
		m_WeaponRaisedTimer = 0.0;
	}

	override bool IsRaised()
	{
		return m_WeaponRaised;
	}

	override bool IsWeaponRaiseCompleted()
	{
		return m_WeaponRaised && m_WeaponRaisedTimer >= 0.15;
	}
	bool eAI_IsLocomotionCmd(int cmdID)
	{
		switch (cmdID)
		{
			case DayZPlayerConstants.COMMANDID_MELEE2:
			case DayZPlayerConstants.COMMANDID_MOVE:
			case DayZPlayerConstants.COMMANDID_LADDER:
			case DayZPlayerConstants.COMMANDID_SWIM:
			case DayZPlayerConstants.COMMANDID_VEHICLE:
				return true;
		}

		return false;
	}
};