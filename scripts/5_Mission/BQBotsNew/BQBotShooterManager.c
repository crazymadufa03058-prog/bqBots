class BQBotGroupSearchState
{
	string m_GroupId;
	bool m_UpdateSearchPosition;

	void BQBotGroupSearchState(string groupId = "")
	{
		m_GroupId = groupId;
		m_UpdateSearchPosition = false;
	}
}

	class BQBotShooterManager
	{
		protected const string LOG_PREFIX = "[EAI] ";
	protected const bool VERBOSE_LOGS = false;
		protected const bool LOS_DEBUG_LOGS = false;
		protected const bool AIM_DEBUG = false;
		protected const bool AIM_DIAG_LOGS = false;
		protected const bool FIRE_DIAG_LOGS = false;
		protected const bool PERF_DIAG_LOGS = false;
		protected const bool FRIENDLY_FIRE_DIAG_LOGS = false;
		protected const bool COMBAT_DIAG_LOGS = false;
		protected const bool JAM_DEBUG_LOGS = false;
		protected const bool TURN_DEBUG_LOGS = false;
	protected const bool SEARCH_DIAG_LOGS = false;
	protected const bool SEARCH_PACKET_DIAG_LOGS = false;
		protected const bool MOVEMENT_DIAG_LOGS = false;
		protected const bool FSM_DIAG_LOGS = false;
		protected const int FSM_DIAG_INTERVAL_MS = 2500;
		protected const int DEBUG_INTERVAL_MS = 2000;
		protected const int LOS_DEBUG_INTERVAL_MS = 250;
		protected const int COMBAT_DIAG_INTERVAL_MS = 350;
		protected const int HOLD_DIAG_INTERVAL_MS = 350;
		protected const int TURN_DIAG_INTERVAL_MS = 120;
		protected const int JAM_DEBUG_INTERVAL_MS = 250;
		protected const int SEARCH_DIAG_INTERVAL_MS = 220;
		protected const int SEARCH_PACKET_DIAG_INTERVAL_MS = 140;
		protected const int MOVEMENT_DIAG_INTERVAL_MS = 450;
		protected const int POSE_DEBUG_INTERVAL_MS = 400;
		protected const int AIM_DIAG_INTERVAL_MS = 220;
		protected const int FIRE_DIAG_INTERVAL_MS = 150;
		protected const int FRIENDLY_FIRE_DIAG_INTERVAL_MS = 120;
		protected const int PERF_DIAG_INTERVAL_MS = 5000;
		protected const int STAGE_PULSE_INTERVAL_MS = 350;
		protected const bool FRIENDLY_FIRELINE_RAYCAST_ENABLED = false;
		protected const int FRIENDLY_FIRELINE_CHECK_INTERVAL_MS = 40;
		protected const float FRIENDLY_FIRELINE_FALLBACK_RADIUS_M = 0.85;
		protected const float FRIENDLY_FIRE_CONE_MIN_COS = 0.95; // ~18 deg half-angle
	protected const int FIRE_ENTRY_SYNC_DELAY_MS = 50;
		protected const int POST_RELOAD_SYNC_DELAY_MS = 100;
		protected const int POSE_LOS_GRACE_MAX_MS = 800;
		protected const int MAX_CONCURRENT_RELOADS = 100;
		protected const int BOT_INIT_STAGGER_MS = 500;
		protected const int RELOAD_QUEUE_CHECK_MS = 300;
	protected const int BURST_MIN_SHOTS = 3;
		protected const int BURST_MAX_SHOTS = 5;
		protected const int BURST_SHOT_INTERVAL_MS = 60;
		protected const int BURST_CYCLE_COOLDOWN_MS = 200;
		protected const int SINGLE_SHOT_INTERVAL_MS = 80;
		protected const int UNJAM_INFLIGHT_GUARD_MS = 6500;
		protected const int UNJAM_RETRY_INTERVAL_MS = 450;
		protected const int UNJAM_RETRY_WHEN_BUSY_MS = 150;
		protected const int CHAMBER_INFLIGHT_GUARD_MS = 1800;
		protected const int CHAMBER_RETRY_WHEN_BUSY_MS = 120;
		protected const int FIRE_INTENT_MODE_AUTO = 0;
		protected const int FIRE_INTENT_MODE_BURST = 1;
		protected const int FIRE_INTENT_MODE_SINGLE = 2;
	protected const float CHASE_TURN_STEP_DEG = 16.0;
		protected const float AIM_TURN_STEP_DEG = 45.0;
		protected const float TURN_DEADZONE_DEG = 0.5;
		protected const float TURN_SMOOTH_TIME = 0.08;
		protected const float TURN_MAX_VELOCITY = 2500.0;
		// Expansion eAIGroupFormationState semantics: NONE=0, IN=1, FLANK=2.
		protected const int GROUP_FORMATION_STATE_NONE = 0;
		protected const int GROUP_FORMATION_STATE_IN = 1;
		protected const int GROUP_FORMATION_STATE_FLANK = 2;
		// Compatibility alias for older local code: OUT meant tactical/flank spread.
		protected const int GROUP_FORMATION_STATE_OUT = 2;
		protected const int GROUP_WAYPOINT_LOOP = 0;
		protected const int GROUP_WAYPOINT_ALTERNATE = 1;
		protected const int GROUP_WAYPOINT_HALT = 2;
		protected const int GROUP_WAYPOINT_ROAMING = 3;
		protected const int GROUP_WAYPOINT_ROAMING_LOCAL = 4;
		protected const int GROUP_COMBAT_ROLE_ANCHOR = 0;
		protected const int GROUP_COMBAT_ROLE_ASSAULT = 1;
		protected const int GROUP_COMBAT_ROLE_SUPPORT = 2;
		protected const int FLANK_PROGRESS_CHECK_MS = 450;
		protected const int FLANK_STALE_PROGRESS_MS = 3600;
		protected const int FLANK_REEVAL_INTERVAL_MS = 4200;
		protected const float FLANK_TARGET_DRIFT_REPLAN_M = 18.0;
		protected const float FLANK_REACH_M = 2.2;
		protected const float FLANK_MIN_TRAVEL_M = 3.6;
		protected const float FLANK_MIN_ANGLE_DEG = 42.0;
		protected const float FLANK_MAX_ANGLE_DEG = 108.0;
		protected const int CORE_STATE_IDLE = 0;
		protected const int CORE_STATE_PURSUIT = 1;
		protected const int CORE_STATE_ENGAGE = 2;
		protected const int TACTICAL_STATE_IDLE = 0;
		protected const int TACTICAL_STATE_PURSUIT = 1;
		protected const int TACTICAL_STATE_TRAVERSING_WAYPOINTS = 2;
		protected const int TACTICAL_STATE_FIGHTING_POSITIONING = 10;
		protected const int TACTICAL_STATE_FIGHTING_FIREWEAPON = 11;
		protected const int TACTICAL_STATE_FIGHTING_EVADE = 12;
		protected const int TACTICAL_STATE_FOLLOW_FORMATION = 13;
		protected const int TACTICAL_STATE_FLANK = 14;
		protected const float GROUP_PATROL_WAYPOINT_REACH_M = 2.2;
		protected const float GROUP_FOLLOW_IDLE_REACH_M = 1.4;
		protected const int GROUP_TARGET_SHARE_MAX_AGE_MS = 3000;
		protected const float CLOSE_AWARENESS_RADIUS_M = 45.0;
		protected const int NO_LOS_GOAL_HOLD_MS = 4200;
		protected const float NO_LOS_GOAL_REPLACE_DIST_M = 12.0;
		protected const float NO_LOS_GOAL_ALIGN_MIN_DOT = 0.20;
		protected const float NO_LOS_DIRECT_PURSUIT_DIST_M = 26.0;
		protected const int NO_LOS_DIRECT_PURSUIT_VISUAL_GRACE_MS = 1600;
		protected const int MOVE_GOAL_HOLD_MIN_MS = 3000;
		protected const int MOVE_GOAL_HOLD_MAX_MS = 8000;
		protected const float MOVE_GOAL_FORCE_REPLACE_DIST_M = 18.0;
		protected const float MOVE_GOAL_REACHED_EXTRA_M = 0.8;
		protected const float GROUP_TARGET_SHARE_AGGRO_DIST_M = 400.0;
		protected const float BQBOT_FORCE_DETECT_RADIUS_M = 400.0;
		protected const float BQBOT_FORCE_FIRE_RADIUS_M = 400.0;
		protected const float BQBOT_SHOT_HEARING_RADIUS_M = 400.0;
		protected const int BQBOT_SHOT_HEARING_MEMORY_MS = 8000;

		protected static ref array<Man> s_PlayerSnapshot;
		protected static int s_NextPlayerSnapshotRefreshTime;
		protected static ref array<ref BQBotShooterManager> s_AllManagers;
		protected static int s_NextManagerOrder;
		protected static ref array<ref BQBotSpawnConfig> s_QueuedSpawnOverrides;
		protected static int s_NextQueuedSpawnOverrideIndex;
		protected static ref array<ref BQBotSettings> s_QueuedSettingsOverrides;
		protected static int s_NextQueuedSettingsOverrideIndex;
		protected static int s_ActiveReloadOperations;
		protected static int s_NextGlobalReloadQueueCheck;
		protected static int s_BotInitStagger;
		protected static int s_ActiveManagers;
		protected static int s_PerfWindowStartTime;
		protected static int s_PerfUpdateTicks;
		protected static int s_PerfTargetScans;
		protected static int s_PerfLosChecks;
		protected static int s_PerfLosBlocked;
		protected static int s_PerfCandidateChecks;
		protected static int s_PerfSkippedBotCandidates;
		protected static int s_PerfLosCacheHits;
		protected static int s_PerfTargetScanBudgetDeferred;
		protected static int s_PerfLosBudgetDeferred;
		protected static int s_GlobalBudgetWindowStartTime;
		protected static int s_GlobalTargetScanBudgetUsed;
		protected static int s_GlobalLosBudgetUsed;
		protected static ref array<ref BQBotGroupSearchState> s_GroupSearchStates;

		protected ref BQBotSettings m_Settings;
	protected bqBots_Boris m_Bot;
		protected Weapon_Base m_BotWeapon;
		protected int m_UpdateIntervalMs;
		protected int m_MaintenanceIntervalMs;
		protected int m_AimUpdateIntervalMs;
		protected int m_TargetScanIntervalMs;
		protected int m_TargetScanNoTargetIntervalMs;
		protected int m_TargetScanBudgetPerWindow;
		protected int m_PlayerSnapshotIntervalMs;
		protected int m_LosCheckIntervalMs;
		protected int m_LosBudgetPerWindow;
		protected int m_BudgetWindowMs;
		protected int m_FireStartStaggerMs;
		protected int m_NearestDebugIntervalMs;
		protected int m_NextDebugTime;
		protected int m_NextLosDebugTime;
		protected int m_NextMaintenanceTime;
		protected int m_NextReloadAttemptTime;
		protected int m_NextReloadDebugTime;
		protected int m_NextAimUpdateTime;
		protected int m_NextTargetScanTime;
		protected int m_NextNearestDistanceRefreshTime;
		protected Magazine m_ReserveMag;
		protected bool m_IsReloading;
		protected int m_ReloadStartTime;
		protected Magazine m_ReloadPrevCurrentMag;
		protected Magazine m_ReloadIncomingMag;
		protected bool m_CombatPoseActive;
		protected int m_NextPoseDebugTime;
		protected int m_NextAimDiagTime;
		protected int m_NextFireDiagTime;
		protected int m_NextWeaponTraceTime;
		protected int m_NextFriendlyFireDiagTime;
		protected int m_NextStagePulseTime;
		protected int m_NextMovementDiagTime;
		protected int m_NextFsmDiagTime;
		protected int m_NextFlankGuardDiagTime;
		protected PlayerBase m_CurrentTarget;
		protected EntityAI m_LastHandsEntity;
		protected float m_LastNearestDistance;
		protected string m_LastStageTag;
		protected bool m_LastLosResultKnown;
		protected bool m_LastLosResult;
		protected string m_LastLosBlocker;
		protected PlayerBase m_LastLosTarget;
		protected int m_NextLosCheckTime;
		protected bool m_LastLosBudgetDeferred;
		protected bool m_LastFriendlyFirelineKnown;
		protected bool m_LastFriendlyFirelineBlocked;
		protected PlayerBase m_LastFriendlyFirelineTarget;
		protected int m_NextFriendlyFirelineCheckTime;
		protected int m_UpdateCounter;
		protected bool m_WasInFireRadius;
		protected int m_FireEntrySyncUntilTime;
		protected int m_PostReloadSyncUntilTime;
		protected int m_BurstShotsRemaining;
		protected int m_NextBurstShotTime;
		protected int m_NextBurstStartTime;
		protected int m_NextSingleRequestTime;
		protected int m_NextUnjamAttemptTime;
		protected bool m_UnjamEventInFlight;
		protected int m_LastUnjamPostTime;
		protected bool m_ChamberEventInFlight;
		protected int m_LastChamberPostTime;
		protected int m_NextChamberAttemptTime;
		protected int m_NextJamDebugTime;
		protected string m_LastJamStage;
		protected AIWorld m_ChaseAIWorld;
		protected ref PGFilter m_ChaseSampleFilter;
		protected bool m_ChaseMoveActive;
		protected int m_TargetMemoryMs;
		protected int m_TargetSearchTimeoutMs;
		protected int m_TargetSearchUpdateIntervalMs;
		protected float m_TargetSearchReachM;
		protected int m_LastTargetVisibleTime;
		protected vector m_LastKnownTargetPos;
		protected bool m_HasLastKnownTargetPos;
		protected bool m_AggroLatched;
		protected bool m_TargetSearchActive;
		protected vector m_TargetSearchPosition;
		protected vector m_TargetSearchDirection;
		protected int m_TargetSearchPositionUpdateCount;
		protected int m_TargetSearchPositionUpdateTime;
		protected int m_TargetSearchStartTime;
		protected bool m_TargetSearchOnLOSLost;
		protected vector m_TargetSearchLastKnownPosition;
		protected ref BQBotTargetMemoryState m_TargetMemoryState;
		protected vector m_NoLosChaseGoal;
		protected int m_NoLosChaseGoalUntilTime;
		protected string m_NoLosChaseGoalSource;
		protected float m_ChaseStopDistanceM;
		protected float m_ChaseJogDistanceM;
		protected float m_ChaseSprintDistanceM;
		protected int m_ChaseProgressCheckMs;
		protected int m_ChaseStuckRepathMs;
	protected int m_ChaseStrafeDurationMs;
		protected bool m_CoverEnabled;
		protected float m_CoverSearchRadius;
		protected float m_CoverMinDistance;
		protected float m_CoverMaxDistance;
		protected float m_CoverThreatThreshold;
		protected int m_CoverRecheckIntervalMs;
		protected bool m_FlankingEnabled;
		protected bool m_EnableFlankingOutsideCombat;
		protected float m_MaxFlankingDistance;
		protected float m_FlankThreatMin;
		protected float m_FlankThreatMax;
		protected int m_FlankTimeoutMs;
		protected int m_MovementSpeedLimit;
		protected int m_MovementSpeedLimitUnderThreat;
		protected int m_RoamingMovementSpeedLimit;
		protected int m_SpeedLimitPreference;
		protected float m_FOVNearDistThreshold;
		protected float m_FOVFarDistThreshold;
		protected float m_FOVNearHalfAngleH;
		protected float m_FOVFarHalfAngleH;
		protected float m_ThreatCombatEnter;
		protected vector m_CoverPosition;
		protected bool m_CoverMoveActive;
		protected ref set<Object> m_PotentialCoverObjects;
		protected Object m_CurrentCoverObject;
		protected bool m_IsInCover;
		protected int m_NextCoverCheckTime;
		protected vector m_FlankPosition;
		protected bool m_FlankActive;
		protected int m_FlankExpireTime;
		protected vector m_FlankAnchorTargetPos;
		protected int m_FlankSideSign;
		protected int m_FlankNextReplanTime;
		protected vector m_FlankLastProgressPos;
		protected int m_FlankNextProgressCheckTime;
		protected int m_FlankNoProgressAccumMs;
		protected int m_PositioningSideSign;
		protected int m_NextPositioningSideFlipTime;
		protected bool m_PositioningMoveActive;
		protected vector m_PositioningMoveGoal;
		protected vector m_PositioningMoveAnchorTargetPos;
		protected int m_PositioningMoveExpireTime;
		protected string m_LastTacticalGoalReason;
		protected int m_NextChaseProgressCheckTime;
		protected vector m_LastChaseProgressPos;
		protected float m_LastChaseProgressGoalDist;
		protected int m_ChaseBlockedAccumMs;
		protected int m_ChaseOverrideUntilTime;
		protected float m_ChaseOverrideMovementAngle;
		protected int m_NextEmergencyUnstuckTime;
		protected string m_ChaseOverrideReason;
		// Hard-stuck recovery: when bot has not moved for several seconds, force a
		// large perpendicular jolt and suppress sticky/tactical for a cooldown so
		// path handler can build a fresh route from a different anchor.
		protected int m_HardStuckCooldownUntilTime;
		protected int m_HardStuckSideSign;
		// Breach-follow goal stabilizer: anchor (squadmate position) drifts every tick,
		// so cache the last computed goal and only refresh on meaningful change.
		protected vector m_BreachFollowGoalCache;
		protected vector m_BreachFollowAnchorCache;
		protected int m_BreachFollowGoalCacheUntilTime;
		protected int m_GroupFormationBypassUntilTime;
		protected bool m_StableMoveGoalActive;
		protected vector m_StableMoveGoal;
		protected vector m_StableMoveAnchorTargetPos;
		protected int m_StableMoveGoalUntilTime;
		protected string m_StableMoveGoalSource;
		protected string m_StableMoveGoalKind;
		protected int m_NextGroupSteerBypassDiagTime;
		protected int m_NextCombatDiagTime;
		protected bool m_LastDiagInFireRadius;
		protected bool m_LastDiagHasLos;
		protected bool m_LastDiagShouldKeepPose;
		protected bool m_LastDiagCanFire;
		protected bool m_HasDiagSnapshot;
		protected int m_NextHoldDiagTime;
		protected int m_NextTurnDiagTime;
		protected string m_LastHoldDiagReason;
		protected int m_NextSearchDiagTime;
		protected string m_LastSearchDiagReason;
		protected int m_NextSearchPacketDiagTime;
		protected string m_LastSearchPacketDiagTag;
		protected string m_LastMovementDiagTag;
		protected int m_LastTurnUpdateTimeMs;
		protected float m_TurnSmoothVel[1];
		protected int m_ManagerOrder;
		protected string m_GroupId;
		protected int m_GroupSlot;
		protected string m_GroupFormation;
		protected float m_GroupFormationScale;
		protected float m_GroupFormationLooseness;
		protected bool m_GroupFormationGoalActive;
		protected vector m_GroupFormationGoal;
		protected float m_GroupFormationDistance;
		protected bool m_GroupIsLeader;
		protected int m_GroupMemberCount;
		protected bool m_GroupPatrolEnabled;
		protected int m_GroupWaypointBehaviour;
		protected int m_GroupPatrolWaypointIndex;
		protected bool m_GroupPatrolBacktracking;
		protected float m_GroupRegroupDistanceM;
		protected float m_GroupBreakFormationDistanceM;
		protected int m_GroupFormationState;
		protected int m_NextExpansionGroupSyncTime;
		protected ref bqCombatFormation m_FormationObj;
		protected string m_FormationObjName;
		protected int m_GroupCombatRole;
		// --- Phase1: Expansion-style combat strafe ---
		// Bot strafes left/right while shooting; on collision, backs off then tries same side again.
		protected bool m_IsSniper;                      // static bot: no combat strafe
		protected int m_CoreState;
		protected int m_PrevCoreState;
		protected int m_TacticalState;
		protected int m_PrevTacticalState;
		protected int m_TacticalStateEnterTime;
		protected int m_LastTacticalEvadeTime;
		protected ref map<int, ref BQBotStateBase> m_FSMStates;
		protected ref BQBotStateBase m_FSMActiveState;
		protected int m_FSMActiveStateId;
		protected bool m_FSMTransitionRequested;
		protected int m_FSMRequestedStateId;
		protected bool m_FSMForcedTacticalStateActive;
		protected int m_FSMForcedTacticalState;
		protected bool m_GroupShouldTakeCover;

		// --- Phase 10: group role coordination (attacker/defender swap, panic, low-ammo retreat) ---
		protected const int BQBOT_GROUP_ROLE_UNASSIGNED   = 0;
		protected const int BQBOT_GROUP_ROLE_ATTACKER     = 1;
		protected const int BQBOT_GROUP_ROLE_DEFENDER     = 2;
		protected const float BQBOT_ROLE_SWAP_DIST_M      = 45.0;   // attacker becomes support only after a real push, not right after the gate
		protected const float BQBOT_PANIC_ALIVE_RATIO     = 0.5;    // panic when group has ------------------------------------------------------------------50 percent members left
		protected const float BQBOT_PANIC_CLOSE_FIRE_DIST_M = 22.0;
		protected const float BQBOT_LOW_AMMO_RATIO        = 0.25;   // retreat when current mag ------------------------------------------------------------------25 percent
		protected const int   BQBOT_ROLE_UPDATE_INTERVAL_MS = 1200;
		protected const int   BQBOT_DEFENDER_SUPPORT_HOLD_MS = 4500;
		protected const float BQBOT_DEFENDER_SUPPORT_REANCHOR_DRIFT_M = 10.0;
		protected const float BQBOT_DEFENDER_SUPPORT_DESIRED_LOS_M = 32.0;
		protected const float BQBOT_DEFENDER_SUPPORT_DESIRED_NOLOS_M = 28.0;
		protected const float BQBOT_DEFENDER_SUPPORT_MAX_STEP_M = 5.5;
		protected const int   BQBOT_DEFENDER_SUPPORT_SPEED_LIMIT = 2;
		protected const int   BQBOT_BREACH_FOLLOW_SEEN_AGE_MS = 4500;
		protected const float BQBOT_BREACH_FOLLOW_MIN_DIST_M = 3.0;

		protected int   m_BQGroupRole;              // BQBOT_GROUP_ROLE_*
		protected vector m_BQRoleStartPos;          // pos when current role was assigned
		protected bool  m_BQPanicMode;              // sprint-away-from-target, fire only from cover
		protected bool  m_BQLowAmmoRetreat;         // seek cover, lower weapon until reload
		protected int   m_BQNextRoleUpdateTime;
		protected int   m_BQInitialGroupAliveCount; // snapshot at first combat entry (for alive ratio)
		protected int   m_BQLastGroupAliveCount;
		protected bool  m_BQGroupCombatSeen;

	void BQBotShooterManager()
		{
			if (!GetGame().IsServer())
				return;

			s_ActiveManagers++;
			m_ManagerOrder = s_NextManagerOrder;
			s_NextManagerOrder++;
			LoadSettings();
			RegisterManager();
			int staggerDelay = s_BotInitStagger;
			s_BotInitStagger = s_BotInitStagger + BOT_INIT_STAGGER_MS;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpawnBotDelayed, staggerDelay, false);
		}

		protected void SpawnBotDelayed()
		{
			SpawnBotInternal();
		}

		static void QueueSpawnOverride(string pos, string ori = "0 0 0", string classname = "bqBots_Boris_AI", int allowDamage = 1)
		{
			if (!s_QueuedSpawnOverrides)
				s_QueuedSpawnOverrides = new array<ref BQBotSpawnConfig>();

			BQBotSpawnConfig cfg = new BQBotSpawnConfig();
			if (classname != "")
				cfg.m_Classname = classname;
			if (pos != "")
				cfg.m_Position = pos;
			if (ori != "")
				cfg.m_Orientation = ori;
			cfg.m_AllowDamage = allowDamage;

			s_QueuedSpawnOverrides.Insert(cfg);
		}

	static void QueueSettingsOverride(BQBotSettings settings)
		{
			if (!settings)
				return;

			if (!s_QueuedSettingsOverrides)
				s_QueuedSettingsOverrides = new array<ref BQBotSettings>();

			s_QueuedSettingsOverrides.Insert(settings);
		}

		static void ResetBotInitStagger()
		{
			s_BotInitStagger = 0;
			s_ActiveReloadOperations = 0;
			s_GlobalBudgetWindowStartTime = 0;
			s_GlobalTargetScanBudgetUsed = 0;
			s_GlobalLosBudgetUsed = 0;
			s_NextManagerOrder = 0;
		}

		void ~BQBotShooterManager()
		{
			if (GetGame())
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Update);
			}

			UnregisterManager();

			if (s_ActiveManagers > 0)
				s_ActiveManagers--;
		}













		protected void SpawnBot()
		{
			string botClass = GetSpawnClassname();
			vector spawnPosRequested = GetSpawnPosition();
			vector spawnPos = ResolveGroundSpawnPosition(spawnPosRequested);
			vector spawnOri = GetSpawnOrientation();
			Object obj = GetGame().CreateObjectEx(botClass, spawnPos, ECE_PLACE_ON_SURFACE);
			m_Bot = bqBots_Boris.Cast(obj);
			if (!m_Bot)
			{
				return;
			}

			m_Bot.SetPosition(spawnPos);
			m_Bot.SetOrientation(spawnOri);
			m_Bot.SetAllowDamage(GetSpawnAllowDamage());
			m_Bot.BQBot_SetIsBot(true);
			ApplyAccuracyProfile();
			ApplyDamageProfile();
			SyncFlankingSettingsToBot();
			m_Bot.BQBot_SetFireIntentMode(GetFireIntentModeId());
			m_Bot.BQBot_ResetSurvivalInitFlag();
			m_Bot.BQBot_ApplySurvivalDisabledOnce();
			ForceStandPose();
			ApplyConfiguredClothing();
			EnsureBotStorage();

			EntityAI inHands = m_Bot.GetHumanInventory().CreateInHands(GetWeaponType());
			m_BotWeapon = Weapon_Base.Cast(inHands);
			m_CurrentTarget = null;
			m_CoreState = CORE_STATE_IDLE;
			m_PrevCoreState = CORE_STATE_IDLE;
			m_TacticalState = TACTICAL_STATE_IDLE;
			m_PrevTacticalState = TACTICAL_STATE_IDLE;
			m_TacticalStateEnterTime = 0;
			m_LastTacticalEvadeTime = 0;
			m_FSMStates = null;
			m_FSMActiveState = null;
			m_FSMActiveStateId = -1;
			m_FSMTransitionRequested = false;
			m_FSMRequestedStateId = TACTICAL_STATE_IDLE;
			m_FSMForcedTacticalStateActive = false;
			m_FSMForcedTacticalState = TACTICAL_STATE_IDLE;
			m_GroupShouldTakeCover = false;
			m_NextAimUpdateTime = 0;
			m_NextTargetScanTime = 0;
			m_NextNearestDistanceRefreshTime = 0;
			m_LastNearestDistance = -1.0;
			m_NextStagePulseTime = 0;
			m_NextMovementDiagTime = 0;
			m_NextFsmDiagTime = 0;
			m_LastStageTag = "";
			m_UpdateCounter = 0;
			m_WasInFireRadius = false;
			m_FireEntrySyncUntilTime = 0;
			m_PostReloadSyncUntilTime = 0;
			m_BurstShotsRemaining = 0;
			m_NextBurstShotTime = 0;
			m_NextBurstStartTime = 0;
			m_NextSingleRequestTime = 0;
			m_NextUnjamAttemptTime = 0;
			m_UnjamEventInFlight = false;
			m_LastUnjamPostTime = 0;
			m_ChamberEventInFlight = false;
			m_LastChamberPostTime = 0;
			m_NextChamberAttemptTime = 0;
			m_NextJamDebugTime = 0;
			m_LastJamStage = "";
			m_LastLosResultKnown = false;
			m_LastLosResult = false;
			m_LastLosBlocker = "none";
			m_LastLosTarget = null;
			m_NextLosCheckTime = 0;
			m_LastLosBudgetDeferred = false;
			m_LastFriendlyFirelineKnown = false;
			m_LastFriendlyFirelineBlocked = false;
			m_LastFriendlyFirelineTarget = null;
			m_NextFriendlyFirelineCheckTime = 0;
			m_LastTargetVisibleTime = 0;
			m_LastKnownTargetPos = "0 0 0";
			m_HasLastKnownTargetPos = false;
			m_TargetSearchActive = false;
			m_TargetSearchPosition = "0 0 0";
			m_TargetSearchDirection = "0 0 1";
			m_TargetSearchPositionUpdateCount = 0;
			m_TargetSearchPositionUpdateTime = 0;
			m_TargetSearchStartTime = 0;
			m_TargetSearchOnLOSLost = false;
			m_TargetSearchLastKnownPosition = "0 0 0";
			m_TargetMemoryState = null;
			SyncTargetSearchStateLastKnownFromMemory();
			m_AggroLatched = false;
			m_NoLosChaseGoal = "0 0 0";
			m_NoLosChaseGoalUntilTime = 0;
			m_NoLosChaseGoalSource = "";
			m_ChaseMoveActive = false;
			m_CoverPosition = "0 0 0";
			m_CoverMoveActive = false;
			m_PotentialCoverObjects = new set<Object>();
			m_CurrentCoverObject = null;
			m_IsInCover = false;
			m_NextCoverCheckTime = 0;
			m_FlankPosition = "0 0 0";
			m_FlankActive = false;
			m_FlankExpireTime = 0;
			m_FlankAnchorTargetPos = "0 0 0";
			m_FlankSideSign = 0;
			m_FlankNextReplanTime = 0;
			m_FlankLastProgressPos = "0 0 0";
			m_FlankNextProgressCheckTime = 0;
			m_FlankNoProgressAccumMs = 0;
			m_PositioningSideSign = 0;
			m_NextPositioningSideFlipTime = 0;
			m_PositioningMoveActive = false;
			m_PositioningMoveGoal = "0 0 0";
			m_PositioningMoveAnchorTargetPos = "0 0 0";
			m_PositioningMoveExpireTime = 0;
			m_LastTacticalGoalReason = "none";
			m_NextChaseProgressCheckTime = 0;
			m_LastChaseProgressPos = m_Bot.GetPosition();
			m_LastChaseProgressGoalDist = -1.0;
			m_ChaseBlockedAccumMs = 0;
			m_ChaseOverrideUntilTime = 0;
			m_ChaseOverrideMovementAngle = 0.0;
			m_NextEmergencyUnstuckTime = 0;
			m_ChaseOverrideReason = "";
			m_HardStuckCooldownUntilTime = 0;
			m_HardStuckSideSign = 1;
			m_BreachFollowGoalCache = "0 0 0";
			m_BreachFollowAnchorCache = "0 0 0";
			m_BreachFollowGoalCacheUntilTime = 0;
			m_GroupFormationBypassUntilTime = 0;
			m_StableMoveGoalActive = false;
			m_StableMoveGoal = "0 0 0";
			m_StableMoveAnchorTargetPos = "0 0 0";
			m_StableMoveGoalUntilTime = 0;
			m_StableMoveGoalSource = "";
			m_StableMoveGoalKind = "";
			m_NextGroupSteerBypassDiagTime = 0;
			m_IsSniper = false;
			m_NextCombatDiagTime = 0;
			m_LastDiagInFireRadius = false;
			m_LastDiagHasLos = false;
			m_LastDiagShouldKeepPose = false;
			m_LastDiagCanFire = false;
			m_HasDiagSnapshot = false;
			m_NextHoldDiagTime = 0;
			m_NextTurnDiagTime = 0;
			m_LastHoldDiagReason = "";
			m_NextSearchDiagTime = 0;
			m_LastSearchDiagReason = "";
			m_NextSearchPacketDiagTime = 0;
			m_LastSearchPacketDiagTag = "";
			m_LastMovementDiagTag = "";
			m_LastTurnUpdateTimeMs = 0;
			m_TurnSmoothVel[0] = 0.0;
			m_GroupFormationGoalActive = false;
			m_GroupFormationGoal = "0 0 0";
			m_GroupFormationDistance = 0.0;
			m_GroupIsLeader = false;
			m_GroupMemberCount = 1;
			m_GroupPatrolWaypointIndex = 0;
			m_GroupPatrolBacktracking = false;
			m_GroupFormationState = GROUP_FORMATION_STATE_IN;
			m_NextExpansionGroupSyncTime = 0;
			m_GroupCombatRole = GROUP_COMBAT_ROLE_ANCHOR;
			m_GroupShouldTakeCover = false;
			m_BQGroupRole = BQBOT_GROUP_ROLE_UNASSIGNED;
			m_BQRoleStartPos = "0 0 0";
			m_BQPanicMode = false;
			m_BQLowAmmoRetreat = false;
			m_BQNextRoleUpdateTime = 0;
			m_BQInitialGroupAliveCount = 0;
			m_BQLastGroupAliveCount = 0;
			m_BQGroupCombatSeen = false;
			ClearChasePath();
			InitFSMStates();
			TransitionFSMState(TACTICAL_STATE_IDLE, GetGame().GetTime());
			InitChaseNavigation();
			EnsureWeaponReady();
			EnsureAutoFireMode(true);

			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Update, m_UpdateIntervalMs, true);
		}

		protected void SpawnBotInternal()
		{
			SpawnBot();
		}

		protected vector ResolveGroundSpawnPosition(vector requestedPos)
		{
			float surfaceY = GetGame().SurfaceY(requestedPos[0], requestedPos[2]);
			return Vector(requestedPos[0], surfaceY, requestedPos[2]);
		}

		protected void EnsureBotStorage()
		{
			if (!m_Bot)
				return;

			// Guarantee cargo space for reserve magazines, otherwise reload cannot start.
			string vestType = GetVestType();
			if (vestType != "" && !m_Bot.GetInventory().FindAttachment(InventorySlots.VEST))
				m_Bot.GetInventory().CreateAttachment(vestType);

			string backpackType = GetBackpackType();
			if (backpackType != "" && !m_Bot.GetInventory().FindAttachment(InventorySlots.BACK))
				m_Bot.GetInventory().CreateAttachment(backpackType);
		}

		protected void ApplyAccuracyProfile()
		{
			if (!m_Bot || !m_Settings || !m_Settings.m_Runtime)
				return;

			BQBotRuntimeConfig rt = m_Settings.m_Runtime;
			m_Bot.BQBot_SetAccuracyProfile(rt.m_AccuracyBase, rt.m_AccuracyNearDistance, rt.m_AccuracyFarDistance, rt.m_AccuracyNearFactor);
			m_Bot.BQBot_SetAccuracyFarFactor(rt.m_AccuracyFarFactor);
			m_Bot.BQBot_SetAccuracyMaxSpreadDeg(rt.m_AccuracyMaxSpreadDeg);
		}

		protected void ApplyDamageProfile()
		{
			if (!m_Bot || !m_Settings)
				return;

			m_Bot.BQBot_SetDamageMultiplier(m_Settings.m_DamageMultiplier);
		}

		protected void SyncFlankingSettingsToBot()
		{
			if (!m_Bot || !m_Settings || !m_Settings.m_Runtime)
				return;

			BQBotRuntimeConfig rt = m_Settings.m_Runtime;

			// Sync flanking distance to eAIBase (used by OverrideTargetPosition(eAITarget))
			if (m_MaxFlankingDistance > 0.0)
				m_Bot.eAI_SetMaxFlankingDistance(m_MaxFlankingDistance);

			// Sync flanking outside combat toggle
			if (m_EnableFlankingOutsideCombat)
				m_Bot.eAI_SetEnableFlankingOutsideCombat(1);
			else
				m_Bot.eAI_SetEnableFlankingOutsideCombat(0);
		}

		protected void RegisterManager()
		{
			if (!s_AllManagers)
				s_AllManagers = new array<ref BQBotShooterManager>();

			if (s_AllManagers.Find(this) == -1)
				s_AllManagers.Insert(this);
		}

		protected void UnregisterManager()
		{
			if (!s_AllManagers)
				return;

			string removedGroupId = m_GroupId;
			int idx = s_AllManagers.Find(this);
			if (idx != -1)
				s_AllManagers.Remove(idx);

			RemoveGroupSearchStateIfUnused(removedGroupId);
		}

		protected static BQBotGroupSearchState GetOrCreateGroupSearchState(string groupId)
		{
			if (groupId == "")
				return null;

			if (!s_GroupSearchStates)
				s_GroupSearchStates = new array<ref BQBotGroupSearchState>();

			foreach (BQBotGroupSearchState state : s_GroupSearchStates)
			{
				if (!state)
					continue;
				if (state.m_GroupId == groupId)
					return state;
			}

			BQBotGroupSearchState created = new BQBotGroupSearchState(groupId);
			s_GroupSearchStates.Insert(created);
			return created;
		}

		protected static void RemoveGroupSearchStateIfUnused(string groupId)
		{
			if (groupId == "" || !s_GroupSearchStates || !s_AllManagers)
				return;

			foreach (BQBotShooterManager manager : s_AllManagers)
			{
				if (!manager)
					continue;
				if (manager.m_GroupId == groupId)
					return;
			}

			for (int i = s_GroupSearchStates.Count() - 1; i >= 0; i--)
			{
				BQBotGroupSearchState state = s_GroupSearchStates.Get(i);
				if (!state)
					continue;
				if (state.m_GroupId == groupId)
				{
					s_GroupSearchStates.Remove(i);
					break;
				}
			}
		}

		protected bool GetGroupSearchUpdateFlag()
		{
			if (m_GroupId == "")
				return false;

			BQBotGroupSearchState state = GetOrCreateGroupSearchState(m_GroupId);
			if (!state)
				return false;

			return state.m_UpdateSearchPosition;
		}

		protected void SetGroupSearchUpdateFlag(bool value)
		{
			if (m_GroupId == "")
				return;

			BQBotGroupSearchState state = GetOrCreateGroupSearchState(m_GroupId);
			if (!state)
				return;

			state.m_UpdateSearchPosition = value;
		}

		protected bool ParseGroupFromCompositeId(string id, out string groupId, out int slot)
		{
			groupId = "";
			slot = -1;
			if (id == "")
				return false;

			TStringArray parts = {};
			id.Split(".", parts);
			if (parts.Count() < 2)
				return false;

			groupId = parts.Get(0);
			if (groupId == "")
				return false;

			int parsedSlot = parts.Get(1).ToInt();
			if (parsedSlot > 0)
				slot = parsedSlot;

			return true;
		}

		protected string NormalizeGroupFormationName(string formation)
		{
			if (formation == "")
				return "vee";

			formation.ToLower();
			bool formationKnown = false;
			if (formation == "vee" || formation == "invvee" || formation == "column" || formation == "invcolumn")
				formationKnown = true;
			if (formation == "file" || formation == "invfile" || formation == "wall" || formation == "line")
				formationKnown = true;
			if (formation == "loose" || formation == "circle" || formation == "circledot")
				formationKnown = true;
			if (formation == "star" || formation == "stardot")
				formationKnown = true;

			if (formationKnown)
				return formation;

			return "vee";
		}

		protected string ResolveActiveFormationName(string formation, bool combatMode = false)
		{
			string active = NormalizeGroupFormationName(formation);
			if (!combatMode)
				return active;

			// Expansion-like combat behavior: avoid pure single-file columns under pressure.
			if (active == "column" || active == "invcolumn" || active == "file" || active == "invfile")
				return "vee";

			return active;
		}

		protected int ParseGroupWaypointBehaviour(string behaviourRaw)
		{
			string behaviour = behaviourRaw;
			behaviour.ToLower();
			if (behaviour == "alternate")
				return GROUP_WAYPOINT_ALTERNATE;
			if (behaviour == "halt")
				return GROUP_WAYPOINT_HALT;
			if (behaviour == "roaming")
				return GROUP_WAYPOINT_ROAMING;
			if (behaviour == "roaming_local")
				return GROUP_WAYPOINT_ROAMING_LOCAL;
			return GROUP_WAYPOINT_LOOP;
		}

		protected void ResolveGroupSetupFromSpawn()
		{
			string previousGroupId = m_GroupId;
			m_GroupId = "";
			m_GroupSlot = -1;
			m_GroupFormation = "vee";
			m_GroupFormationScale = 1.0;
			m_GroupFormationLooseness = 0.6;
			m_GroupFormationGoalActive = false;
			m_GroupFormationGoal = "0 0 0";
			m_GroupFormationDistance = 0.0;
			m_GroupIsLeader = false;
			m_GroupMemberCount = 1;
			m_GroupFormationBypassUntilTime = 0;
			m_GroupPatrolEnabled = true;
			m_GroupWaypointBehaviour = GROUP_WAYPOINT_LOOP;
			m_GroupPatrolWaypointIndex = 0;
			m_GroupPatrolBacktracking = false;
			m_GroupRegroupDistanceM = 8.0;
			m_GroupBreakFormationDistanceM = 18.0;
			m_GroupFormationState = GROUP_FORMATION_STATE_IN;

			if (!m_Settings || !m_Settings.m_Spawn)
				return;

			BQBotSpawnConfig spawnCfg = m_Settings.m_Spawn;
			if (spawnCfg.m_GroupId != "")
				m_GroupId = spawnCfg.m_GroupId;
			if (spawnCfg.m_GroupSlot > 0)
				m_GroupSlot = spawnCfg.m_GroupSlot;

			if (m_GroupId == "" || m_GroupSlot < 1)
			{
				string parsedGroupId;
				int parsedSlot;
				if (ParseGroupFromCompositeId(spawnCfg.m_ZoneId, parsedGroupId, parsedSlot))
				{
					if (m_GroupId == "")
						m_GroupId = parsedGroupId;
					if (m_GroupSlot < 1 && parsedSlot > 0)
						m_GroupSlot = parsedSlot;
				}
			}

			if (spawnCfg.m_Formation != "")
				m_GroupFormation = spawnCfg.m_Formation;
			m_GroupFormation = NormalizeGroupFormationName(m_GroupFormation);

			m_GroupFormationScale = spawnCfg.m_FormationScale;
			m_GroupFormationLooseness = spawnCfg.m_FormationLooseness;
			if (m_GroupFormationScale < 0.5)
				m_GroupFormationScale = 0.5;
			if (m_GroupFormationScale > 4.0)
				m_GroupFormationScale = 4.0;
			if (m_GroupFormationLooseness < 0.0)
				m_GroupFormationLooseness = 0.0;
			if (m_GroupFormationLooseness > 3.0)
				m_GroupFormationLooseness = 3.0;

			m_GroupPatrolEnabled = spawnCfg.m_GroupPatrolEnabled != 0;
			m_GroupWaypointBehaviour = ParseGroupWaypointBehaviour(spawnCfg.m_GroupWaypointBehaviour);
			m_GroupRegroupDistanceM = spawnCfg.m_GroupRegroupDistanceM;
			m_GroupBreakFormationDistanceM = spawnCfg.m_GroupBreakFormationDistanceM;
			if (m_GroupRegroupDistanceM < 2.0)
				m_GroupRegroupDistanceM = 2.0;
			if (m_GroupRegroupDistanceM > 30.0)
				m_GroupRegroupDistanceM = 30.0;
			if (m_GroupBreakFormationDistanceM < (m_GroupRegroupDistanceM + 1.0))
				m_GroupBreakFormationDistanceM = m_GroupRegroupDistanceM + 1.0;
			if (m_GroupBreakFormationDistanceM > 80.0)
				m_GroupBreakFormationDistanceM = 80.0;

			if (m_GroupId != "")
				GetOrCreateGroupSearchState(m_GroupId);
			if (previousGroupId != "" && previousGroupId != m_GroupId)
				RemoveGroupSearchStateIfUnused(previousGroupId);
		}

		protected int GetGroupSortKey()
		{
			if (m_GroupSlot > 0)
				return m_GroupSlot;

			return 100000 + m_ManagerOrder;
		}

		protected void InsertManagerByGroupOrder(array<BQBotShooterManager> managers, BQBotShooterManager manager)
		{
			if (!managers || !manager)
				return;

			int key = manager.GetGroupSortKey();
			int insertAt = managers.Count();
			for (int i = 0; i < managers.Count(); i++)
			{
				BQBotShooterManager existing = managers.Get(i);
				if (!existing)
					continue;
				if (key < existing.GetGroupSortKey())
				{
					insertAt = i;
					break;
				}
			}

			managers.InsertAt(manager, insertAt);
		}

		protected bool CollectAliveGroupMembers(array<BQBotShooterManager> outMembers)
		{
			if (!outMembers)
				return false;

			outMembers.Clear();
			if (m_GroupId == "" || !s_AllManagers)
				return false;

			foreach (BQBotShooterManager manager : s_AllManagers)
			{
				if (!manager || manager.m_GroupId != m_GroupId)
					continue;
				if (!manager.m_Bot || !manager.m_Bot.IsAlive())
					continue;

				InsertManagerByGroupOrder(outMembers, manager);
			}

			return outMembers.Count() > 0;
		}

		protected bqCombatFormation GetOrCreateFormation(string name)
		{
			if (!m_FormationObj || m_FormationObjName != name)
			{
				m_FormationObj = bqCombatFormation.Create(name);
				m_FormationObjName = name;
			}
			m_FormationObj.SetScale(m_GroupFormationScale);
			return m_FormationObj;
		}

		protected vector GetFormationLocalOffset(int memberNo, string formationOverride = "")
		{
			string formation = m_GroupFormation;
			if (formationOverride != "")
				formation = formationOverride;
			float scale = m_GroupFormationScale;
			formation.ToLower();
			if (formation == "loose")
				scale = scale * 1.35;

			if (formation == "file")
				return Vector(0.0, 0.0, -2.0 * memberNo * scale);
			if (formation == "invfile")
				return Vector(0.0, 0.0, 2.0 * memberNo * scale);

			if (formation == "circle" || formation == "circledot")
			{
				float radius = Math.Max(2.0, 2.0 * scale);
				if (formation == "circledot")
					radius = Math.Max(2.5, 2.7 * scale);
				float angleStep = 360.0 / Math.Max(1, m_GroupMemberCount);
				float angle = (memberNo - 1) * angleStep;
				float rad = angle * Math.DEG2RAD;
				float x = Math.Sin(rad) * radius;
				float z = -Math.Cos(rad) * radius;
				return Vector(x, 0.0, z);
			}

			if (formation == "star" || formation == "stardot")
			{
				float outer = Math.Max(2.5, 2.8 * scale);
				float inner = Math.Max(1.2, 1.4 * scale);
				if (formation == "stardot")
					inner = Math.Max(0.9, 1.0 * scale);
				int idx = Math.Max(0, memberNo - 1);
				float angleStar = idx * 36.0;
				float radStar = angleStar * Math.DEG2RAD;
				float radiusStar = outer;
				if ((BQBot_ModInt(idx, 2)) == 1)
					radiusStar = inner;
				return Vector(Math.Sin(radStar) * radiusStar, 0.0, -Math.Cos(radStar) * radiusStar);
			}

			int offset = Math.Floor((memberNo + 1) / 2);
			float scaledOffset = 2.0 * offset * scale;

			if (formation == "column")
			{
				if (BQBot_ModInt(memberNo, 2) == 0)
					return Vector(2.0 * scale, 0.0, -scaledOffset);
				return Vector(-2.0 * scale, 0.0, -scaledOffset);
			}
			if (formation == "invcolumn")
			{
				if (BQBot_ModInt(memberNo, 2) == 0)
					return Vector(2.0 * scale, 0.0, scaledOffset);
				return Vector(-2.0 * scale, 0.0, scaledOffset);
			}

			if (formation == "wall" || formation == "line")
			{
				if (BQBot_ModInt(memberNo, 2) == 0)
					return Vector(scaledOffset, 0.0, 0.0);
				return Vector(-scaledOffset, 0.0, 0.0);
			}

			if (formation == "invvee")
			{
				if (BQBot_ModInt(memberNo, 2) == 0)
					return Vector(scaledOffset, 0.0, scaledOffset);
				return Vector(-scaledOffset, 0.0, scaledOffset);
			}

			// Default to Expansion-like vee.
			if (BQBot_ModInt(memberNo, 2) == 0)
				return Vector(scaledOffset, 0.0, -scaledOffset);
			return Vector(-scaledOffset, 0.0, -scaledOffset);
		}

		protected vector ApplyFormationLooseness(vector localPos, int memberNo)
		{
			float loose = m_GroupFormationLooseness;
			if (loose <= 0.0)
				return localPos;

			float side = loose * 0.35;
			float depth = loose * 0.20;
			if (BQBot_ModInt(memberNo, 2) == 0)
				localPos[0] = localPos[0] + side;
			else
				localPos[0] = localPos[0] - side;

			if ((BQBot_ModInt(memberNo / 2, 2)) == 0)
				localPos[2] = localPos[2] + depth;
			else
				localPos[2] = localPos[2] - depth;

			return localPos;
		}

		protected bool TryGetGroupFormationGoal(PlayerBase target, int now, out vector formationGoal)
		{
			formationGoal = "0 0 0";
			m_GroupFormationGoalActive = false;
			m_GroupFormationDistance = 0.0;
			// If the original leader dies and only one bot remains, do not leave him
			// stuck in follower/defender logic. A solo alive member acts as its own leader.
			m_GroupIsLeader = true;
			m_GroupMemberCount = 1;

			if (!m_Bot || m_GroupId == "")
				return false;

			array<BQBotShooterManager> members = new array<BQBotShooterManager>();
			if (!CollectAliveGroupMembers(members))
				return false;

			m_GroupMemberCount = members.Count();
			if (members.Count() <= 1)
			{
				m_GroupIsLeader = true;
				return false;
			}

			BQBotShooterManager leader = members.Get(0);
			if (!leader || !leader.m_Bot || !leader.m_Bot.IsAlive())
				return false;

			m_GroupIsLeader = leader == this;
			if (m_GroupIsLeader)
				return false;

			int memberIndex = members.Find(this);
			if (memberIndex < 0)
				return false;

			vector leaderPos = leader.m_Bot.GetPosition();
			vector forward = "0 0 0";
			float forwardLenSq = 0.0;
			if (target && IsCandidateHumanTarget(target))
			{
				bool formationTargetLos = HasLineOfSightToTarget(target);
				forward = GetTargetMemoryPosition(target, formationTargetLos) - leaderPos;
				forward[1] = 0.0;
				forwardLenSq = forward[0] * forward[0] + forward[2] * forward[2];
			}
			if (forwardLenSq < 0.0001)
			{
				vector leaderOri = leader.m_Bot.GetOrientation();
				float leaderYaw = leaderOri[0];
				forward = Vector(leaderYaw, 0.0, 0.0).AnglesToVector();
				forward[1] = 0.0;
				forwardLenSq = forward[0] * forward[0] + forward[2] * forward[2];
			}
			if (forwardLenSq < 0.0001)
				return false;
			float invForwardLen = 1.0 / Math.Sqrt(forwardLenSq);
			forward[0] = forward[0] * invForwardLen;
			forward[2] = forward[2] * invForwardLen;

			bool combatFormation = target != null || leader.m_CurrentTarget != null || m_AggroLatched || leader.m_AggroLatched;
			string activeFormation = ResolveActiveFormationName(m_GroupFormation, combatFormation);
			bqCombatFormation formation = GetOrCreateFormation(activeFormation);
			formation.SetMemberCount(m_GroupMemberCount);
			formation.UpdateForced(leaderPos, forward);
			vector localPos = formation.GetPosition(memberIndex);
			localPos = ApplyFormationLooseness(localPos, memberIndex);

			vector goal = formation.ToWorld(localPos, leaderPos);
			vector projected = ProjectToNavigablePosition(goal, 2.5);
			if (projected != "0 0 0")
				goal = projected;

			float distToSlot = vector.Distance(m_Bot.GetPosition(), goal);
			m_GroupFormationDistance = distToSlot;
			m_GroupFormationGoal = goal;

			bool leaderInCombat = leader.m_CurrentTarget != null || leader.m_AggroLatched;
			if (!leaderInCombat && !target && distToSlot <= 1.4)
				return false;

			m_GroupFormationGoalActive = true;
			formationGoal = goal;
			return true;
		}

		protected PlayerBase ResolveGroupTarget(PlayerBase candidateTarget, float detectRadius)
		{
			if (!m_Bot || m_GroupId == "" || !s_AllManagers || s_AllManagers.Count() <= 1)
			{
				m_GroupIsLeader = true;
				m_GroupMemberCount = 1;
				return candidateTarget;
			}

			array<BQBotShooterManager> members = new array<BQBotShooterManager>();
			if (!CollectAliveGroupMembers(members) || members.Count() <= 1)
			{
				m_GroupIsLeader = true;
				m_GroupMemberCount = 1;
				return candidateTarget;
			}

			BQBotShooterManager leader = members.Get(0);
			if (!leader || !leader.m_Bot || !leader.m_Bot.IsAlive())
				return candidateTarget;

			m_GroupIsLeader = leader == this;
			if (m_GroupIsLeader)
			{
				if (candidateTarget)
					return candidateTarget;
				int nowLeader = GetGame().GetTime();
				PlayerBase leaderShared = GetGroupSharedTargetCandidate(members, detectRadius, nowLeader);
				if (leaderShared)
					return leaderShared;
				return candidateTarget;
			}

			PlayerBase leaderTarget = leader.m_CurrentTarget;
			if (leaderTarget && IsCandidateHumanTarget(leaderTarget))
			{
				float detectSqLeader = detectRadius * detectRadius;
				float aggroShareDist = Math.Max(detectRadius + 20.0, GROUP_TARGET_SHARE_AGGRO_DIST_M);
				float aggroShareDetectSqLeader = aggroShareDist * aggroShareDist;
				float distSqLeader = vector.DistanceSq(m_Bot.GetPosition(), leaderTarget.GetPosition());
				if (distSqLeader <= detectSqLeader || (leader.m_AggroLatched && distSqLeader <= aggroShareDetectSqLeader))
					return leaderTarget;
			}

			int now = GetGame().GetTime();
			PlayerBase sharedTarget = GetGroupSharedTargetCandidate(members, detectRadius, now);
			if (sharedTarget)
				return sharedTarget;

			return candidateTarget;
		}

		protected PlayerBase GetGroupSharedTargetCandidate(array<BQBotShooterManager> members, float detectRadius, int now)
		{
			if (!members || members.Count() == 0)
				return null;

			BQBotShooterManager groupLeader = members.Get(0);
			float detectSq = detectRadius * detectRadius;
			float bestScore = -1000000.0;
			PlayerBase bestTarget = null;

			foreach (BQBotShooterManager member : members)
			{
				if (!member)
					continue;
				PlayerBase candidate = member.m_CurrentTarget;
				if (!IsCandidateHumanTarget(candidate))
					continue;

				float distSq = vector.DistanceSq(m_Bot.GetPosition(), candidate.GetPosition());
				float shareDetectSq = detectSq;
				if (member.m_AggroLatched)
				{
					float aggroShareDist = Math.Max(detectRadius + 20.0, GROUP_TARGET_SHARE_AGGRO_DIST_M);
					shareDetectSq = aggroShareDist * aggroShareDist;
				}
				if (distSq > shareDetectSq)
					continue;

				int seenAgeMs = 999999;
				if (member.m_LastTargetVisibleTime > 0)
					seenAgeMs = now - member.m_LastTargetVisibleTime;
				if (seenAgeMs > GROUP_TARGET_SHARE_MAX_AGE_MS && member != this && !member.m_AggroLatched)
					continue;
				if (member.m_AggroLatched && seenAgeMs > (GROUP_TARGET_SHARE_MAX_AGE_MS * 3))
					continue;

				float score = 0.0;
				score = score - Math.Sqrt(distSq) * 0.03;
				score = score - seenAgeMs * 0.002;
				if (member == this)
					score = score + 4.0;
				if (member == groupLeader)
					score = score + 6.0;

				if (score > bestScore)
				{
					bestScore = score;
					bestTarget = candidate;
				}
			}

			return bestTarget;
		}

		protected vector GetManagerConfiguredSpawnPosition(BQBotShooterManager manager)
		{
			if (!manager)
				return "0 0 0";

			if (manager.m_Settings && manager.m_Settings.m_Spawn && manager.m_Settings.m_Spawn.m_Position != "")
				return manager.ParseVectorFromConfig(manager.m_Settings.m_Spawn.m_Position, "0 0 0");

			if (manager.m_Bot)
				return manager.m_Bot.GetPosition();

			return "0 0 0";
		}

		protected void BuildGroupPatrolWaypoints(array<BQBotShooterManager> members, out array<vector> waypoints)
		{
			waypoints = new array<vector>();
			if (!members)
				return;

			foreach (BQBotShooterManager member : members)
			{
				vector wp = GetManagerConfiguredSpawnPosition(member);
				if (wp == "0 0 0")
					continue;
				// Project waypoint Y to surface so navmesh sampling works (config may store Y=0)
				wp[1] = GetGame().SurfaceY(wp[0], wp[2]);
				waypoints.Insert(wp);
			}
		}

		protected float GetMaxFollowerDistanceFromLeader(array<BQBotShooterManager> members, BQBotShooterManager leader)
		{
			if (!members || !leader || !leader.m_Bot)
				return 0.0;

			vector leaderPos = leader.m_Bot.GetPosition();
			float maxDist = 0.0;
			foreach (BQBotShooterManager member : members)
			{
				if (!member || member == leader || !member.m_Bot || !member.m_Bot.IsAlive())
					continue;
				float d = vector.Distance(leaderPos, member.m_Bot.GetPosition());
				if (d > maxDist)
					maxDist = d;
			}

			return maxDist;
		}

		protected int DetermineGroupFormationState(PlayerBase target, bool hasLos, bool canFire)
		{
			if (m_GroupId == "" || !m_Bot)
				return GROUP_FORMATION_STATE_NONE;

			if (!target)
			{
				if (m_AggroLatched)
				{
					// Keep tactical spread in aggro/search phase, but allow regroup when followers drift too far.
					if (m_GroupFormationDistance > (m_GroupRegroupDistanceM * 1.25))
						return GROUP_FORMATION_STATE_IN;
					return GROUP_FORMATION_STATE_OUT;
				}
				return GROUP_FORMATION_STATE_IN;
			}

			if (!hasLos)
			{
				// Expansion-like: temporary LOS loss should not force strict train formation.
				// Only regroup if slot error is large; otherwise allow tactical OUT behavior.
				if (m_GroupFormationDistance > (m_GroupRegroupDistanceM * 1.35))
					return GROUP_FORMATION_STATE_IN;
				if (m_AggroLatched)
					return GROUP_FORMATION_STATE_OUT;
				return GROUP_FORMATION_STATE_IN;
			}

			if (m_GroupFormationDistance > m_GroupRegroupDistanceM)
				return GROUP_FORMATION_STATE_IN;

			bool targetDistLos = HasLineOfSightToTarget(target);
			float distToTarget = vector.Distance(m_Bot.GetPosition(), GetTargetMemoryPosition(target, targetDistLos));
			if (distToTarget >= m_GroupBreakFormationDistanceM)
				return GROUP_FORMATION_STATE_IN;

			if (canFire)
				return GROUP_FORMATION_STATE_OUT;

			return GROUP_FORMATION_STATE_IN;
		}

		protected bool IsFormationBypassActive(int now)
		{
			if (m_GroupFormationBypassUntilTime <= 0)
				return false;

			if (now >= m_GroupFormationBypassUntilTime)
			{
				m_GroupFormationBypassUntilTime = 0;
				return false;
			}

			if (m_GroupId == "")
			{
				m_GroupFormationBypassUntilTime = 0;
				return false;
			}

			if (m_GroupIsLeader)
				return false;

			return true;
		}

		protected void UpdateChaseProgressState(int now, vector moveGoalPos, string sourceTag = "")
		{
			if (!m_Bot || m_ChaseProgressCheckMs <= 0)
				return;

			if (m_NextChaseProgressCheckTime <= 0)
			{
				m_LastChaseProgressPos = m_Bot.GetPosition();
				m_LastChaseProgressGoalDist = vector.Distance(m_LastChaseProgressPos, moveGoalPos);
				m_NextChaseProgressCheckTime = now + m_ChaseProgressCheckMs;
				return;
			}

			if (now < m_NextChaseProgressCheckTime)
				return;

			vector botPos = m_Bot.GetPosition();
			float movedSq = vector.DistanceSq(botPos, m_LastChaseProgressPos);
			// Only count XZ movement - vertical micro-jitter on beams must NOT count as progress.
			float dx = botPos[0] - m_LastChaseProgressPos[0];
			float dz = botPos[2] - m_LastChaseProgressPos[2];
			float movedXZSq = dx * dx + dz * dz;
			float distGoal = vector.Distance(botPos, moveGoalPos);

			// Tighter thresholds: bot must actually displace XZ by ~20-30cm per check
			// (i.e. true walk pace), otherwise we count it as blocked.
			float minMoveSq = 0.04; // 20cm
			if (distGoal > Math.Max(10.0, m_ChaseJogDistanceM))
				minMoveSq = 0.09; // 30cm (jogging)
			else if (distGoal > Math.Max(6.0, m_ChaseStopDistanceM + 3.0))
				minMoveSq = 0.0625; // 25cm
			movedSq = movedXZSq;

			float goalGain = 0.0;
			bool hasGoalDistHistory = m_LastChaseProgressGoalDist >= 0.0;
			if (hasGoalDistHistory)
				goalGain = m_LastChaseProgressGoalDist - distGoal;
			bool goalProgressed = !hasGoalDistHistory || goalGain > 0.06;
			bool farFromGoal = distGoal > (m_ChaseStopDistanceM + 1.2);

			if (farFromGoal && movedSq < minMoveSq && !goalProgressed)
			{
				m_ChaseBlockedAccumMs = m_ChaseBlockedAccumMs + m_ChaseProgressCheckMs;
			}
			else if (m_ChaseBlockedAccumMs > 0)
			{
				int decay = m_ChaseProgressCheckMs;
				if (decay < 50)
					decay = 50;
				m_ChaseBlockedAccumMs = m_ChaseBlockedAccumMs - decay;
				if (m_ChaseBlockedAccumMs < 0)
					m_ChaseBlockedAccumMs = 0;
			}


			m_LastChaseProgressPos = botPos;
			m_LastChaseProgressGoalDist = distGoal;
			m_NextChaseProgressCheckTime = now + m_ChaseProgressCheckMs;
		}

		protected bool TryGetAggroPursuitGoal(int now, out vector pursuitGoal)
		{
			pursuitGoal = "0 0 0";
			int bestSeenTime = 0;
			int pursuitMemoryMs = m_TargetSearchTimeoutMs;
			if (m_TargetMemoryMs > pursuitMemoryMs)
				pursuitMemoryMs = m_TargetMemoryMs;
			if (pursuitMemoryMs < 0)
				pursuitMemoryMs = 0;

			if (m_HasLastKnownTargetPos && m_LastTargetVisibleTime > 0)
			{
				int ownAgeMs = now - m_LastTargetVisibleTime;
				if (ownAgeMs >= 0 && (pursuitMemoryMs <= 0 || ownAgeMs <= pursuitMemoryMs))
				{
					pursuitGoal = m_LastKnownTargetPos;
					bestSeenTime = m_LastTargetVisibleTime;
				}
			}

			if (m_GroupId != "")
			{
				array<BQBotShooterManager> members = new array<BQBotShooterManager>();
				if (CollectAliveGroupMembers(members))
				{
					foreach (BQBotShooterManager member : members)
					{
						if (!member || !member.m_HasLastKnownTargetPos || member.m_LastTargetVisibleTime <= 0)
							continue;

						int memberAgeMs = now - member.m_LastTargetVisibleTime;
						int memberMemoryMs = member.m_TargetSearchTimeoutMs;
						if (member.m_TargetMemoryMs > memberMemoryMs)
							memberMemoryMs = member.m_TargetMemoryMs;
						if (memberMemoryMs <= 0)
							memberMemoryMs = pursuitMemoryMs;
						if (memberAgeMs < 0 || (memberMemoryMs > 0 && memberAgeMs > memberMemoryMs))
							continue;

						if (member.m_LastTargetVisibleTime > bestSeenTime)
						{
							pursuitGoal = member.m_LastKnownTargetPos;
							bestSeenTime = member.m_LastTargetVisibleTime;
						}
					}
				}
			}

			if (bestSeenTime <= 0 || pursuitGoal == "0 0 0")
				return false;

			pursuitGoal = ProjectToNavigablePosition(pursuitGoal, 3.0);
			return pursuitGoal != "0 0 0";
		}

		protected void LoadSettings()
		{
			m_Settings = new BQBotSettings();
			BQBotRuntimeConfig runtimeDefaults = new BQBotRuntimeConfig();

			ApplyQueuedSettingsOverride();
			if (!m_Settings.m_Spawn)
				m_Settings.m_Spawn = new BQBotSpawnConfig();
			if (!m_Settings.m_Weapon)
				m_Settings.m_Weapon = new BQBotWeaponConfig();
			if (!m_Settings.m_Clothing)
				m_Settings.m_Clothing = new BQBotClothingConfig();
			if (!m_Settings.m_Runtime)
				m_Settings.m_Runtime = new BQBotRuntimeConfig();

			bool migratedSpawnClass = false;
			// Migrate legacy classname to dedicated bot class so all bot-only patches apply.
			if (m_Settings.m_Spawn.m_Classname == "" || m_Settings.m_Spawn.m_Classname == "SurvivorM_Mirek" || m_Settings.m_Spawn.m_Classname == "BQBots_Mirek" || m_Settings.m_Spawn.m_Classname == "bqBots_Mirek")
			{
				m_Settings.m_Spawn.m_Classname = "bqBots_Boris_AI";
				migratedSpawnClass = true;
			}

			// Force faster defaults if config has slow values
		m_UpdateIntervalMs = m_Settings.m_Runtime.m_UpdateIntervalMs;
		if (m_UpdateIntervalMs > 150)
			m_UpdateIntervalMs = 100;
			m_MaintenanceIntervalMs = m_Settings.m_Runtime.m_MaintenanceIntervalMs;
			m_AimUpdateIntervalMs = m_Settings.m_Runtime.m_AimUpdateIntervalMs;
			m_TargetScanIntervalMs = m_Settings.m_Runtime.m_TargetScanIntervalMs;
			m_TargetScanNoTargetIntervalMs = m_Settings.m_Runtime.m_TargetScanNoTargetIntervalMs;
			m_TargetScanBudgetPerWindow = m_Settings.m_Runtime.m_TargetScanBudgetPerWindow;
			m_PlayerSnapshotIntervalMs = m_Settings.m_Runtime.m_PlayerSnapshotIntervalMs;
			m_LosCheckIntervalMs = m_Settings.m_Runtime.m_LosCheckIntervalMs;
			m_LosBudgetPerWindow = m_Settings.m_Runtime.m_LosBudgetPerWindow;
			m_BudgetWindowMs = m_Settings.m_Runtime.m_BudgetWindowMs;
			m_FireStartStaggerMs = m_Settings.m_Runtime.m_FireStartStaggerMs;
			m_NearestDebugIntervalMs = m_Settings.m_Runtime.m_NearestDebugIntervalMs;
			m_TargetMemoryMs = m_Settings.m_Runtime.m_LostTargetMemoryMs;
			m_TargetSearchTimeoutMs = m_Settings.m_Runtime.m_TargetSearchTimeoutMs;
			m_TargetSearchUpdateIntervalMs = m_Settings.m_Runtime.m_TargetSearchUpdateIntervalMs;
			m_TargetSearchReachM = m_Settings.m_Runtime.m_TargetSearchReachM;
			m_ChaseStopDistanceM = m_Settings.m_Runtime.m_ChaseStopDistanceM;
			m_ChaseJogDistanceM = m_Settings.m_Runtime.m_ChaseJogDistanceM;
			m_ChaseSprintDistanceM = m_Settings.m_Runtime.m_ChaseSprintDistanceM;
			m_ChaseProgressCheckMs = m_Settings.m_Runtime.m_ChaseProgressCheckMs;
			m_ChaseStuckRepathMs = m_Settings.m_Runtime.m_ChaseStuckRepathMs;
			m_ChaseStrafeDurationMs = m_Settings.m_Runtime.m_ChaseStrafeDurationMs;
			m_CoverEnabled = m_Settings.m_Runtime.m_CoverEnabled;
			m_CoverSearchRadius = m_Settings.m_Runtime.m_CoverSearchRadius;
			m_CoverMinDistance = m_Settings.m_Runtime.m_CoverMinDistance;
			m_CoverMaxDistance = m_Settings.m_Runtime.m_CoverMaxDistance;
			m_CoverThreatThreshold = m_Settings.m_Runtime.m_CoverThreatThreshold;
			m_CoverRecheckIntervalMs = m_Settings.m_Runtime.m_CoverRecheckIntervalMs;
			m_FlankingEnabled = m_Settings.m_Runtime.m_FlankingEnabled;
			m_EnableFlankingOutsideCombat = m_Settings.m_Runtime.m_EnableFlankingOutsideCombat;
			m_MaxFlankingDistance = m_Settings.m_Runtime.m_MaxFlankingDistance;
			m_FlankThreatMin = m_Settings.m_Runtime.m_FlankThreatMin;
			m_FlankThreatMax = m_Settings.m_Runtime.m_FlankThreatMax;
			m_FlankTimeoutMs = m_Settings.m_Runtime.m_FlankTimeoutMs;

			// Old bqBots loadouts may not contain these Expansion-style runtime fields;
			// missing JSON bool/number values can arrive as false/zero and disable the FSM.
			if (!m_CoverEnabled)
				m_CoverEnabled = runtimeDefaults.m_CoverEnabled;
			if (!m_FlankingEnabled)
				m_FlankingEnabled = runtimeDefaults.m_FlankingEnabled;
			if (m_MaxFlankingDistance <= 0.0)
				m_MaxFlankingDistance = runtimeDefaults.m_MaxFlankingDistance;
			if (m_FlankThreatMin <= 0.0)
				m_FlankThreatMin = runtimeDefaults.m_FlankThreatMin;
			if (m_FlankThreatMax <= 0.0)
				m_FlankThreatMax = runtimeDefaults.m_FlankThreatMax;
			if (m_FlankTimeoutMs <= 0)
				m_FlankTimeoutMs = runtimeDefaults.m_FlankTimeoutMs;

			m_MovementSpeedLimit = m_Settings.m_Runtime.m_MovementSpeedLimit;
			m_MovementSpeedLimitUnderThreat = m_Settings.m_Runtime.m_MovementSpeedLimitUnderThreat;
			m_RoamingMovementSpeedLimit = m_Settings.m_Runtime.m_RoamingMovementSpeedLimit;
			m_SpeedLimitPreference = m_Settings.m_Runtime.m_SpeedLimitPreference;
			m_FOVNearDistThreshold = m_Settings.m_Runtime.m_FOVNearDistThreshold;
			m_FOVFarDistThreshold = m_Settings.m_Runtime.m_FOVFarDistThreshold;
			m_FOVNearHalfAngleH = m_Settings.m_Runtime.m_FOVNearHalfAngleH;
			m_FOVFarHalfAngleH = m_Settings.m_Runtime.m_FOVFarHalfAngleH;
			m_ThreatCombatEnter = m_Settings.m_Runtime.m_ThreatCombatEnter;
			if (m_UpdateIntervalMs < 50)
				m_UpdateIntervalMs = 50;
			if (m_MaintenanceIntervalMs < 100)
				m_MaintenanceIntervalMs = 100;
			if (m_AimUpdateIntervalMs < 60)
				m_AimUpdateIntervalMs = 60;
			if (m_TargetScanIntervalMs < 100)
				m_TargetScanIntervalMs = 100;
			if (m_TargetScanNoTargetIntervalMs < m_TargetScanIntervalMs)
				m_TargetScanNoTargetIntervalMs = m_TargetScanIntervalMs;
			if (m_TargetScanBudgetPerWindow < 4)
				m_TargetScanBudgetPerWindow = 4;
			if (m_PlayerSnapshotIntervalMs < 100)
				m_PlayerSnapshotIntervalMs = 100;
			if (m_LosCheckIntervalMs < 60)
				m_LosCheckIntervalMs = 60;
			if (m_LosBudgetPerWindow < 8)
				m_LosBudgetPerWindow = 8;
			if (m_BudgetWindowMs < 50)
				m_BudgetWindowMs = 50;
			if (m_BudgetWindowMs > 1000)
				m_BudgetWindowMs = 1000;
			if (m_FireStartStaggerMs < 0)
				m_FireStartStaggerMs = 0;
			if (m_FireStartStaggerMs > 500)
				m_FireStartStaggerMs = 500;
			if (m_NearestDebugIntervalMs < 500)
				m_NearestDebugIntervalMs = 500;
			if (m_TargetMemoryMs < 0)
				m_TargetMemoryMs = 0;
			if (m_TargetMemoryMs > 60000)
				m_TargetMemoryMs = 60000;
			// Backward compatibility for old loadouts.json where new search fields are absent
			// and DayZ JSON loader yields zero values instead of script defaults.
			if (m_TargetSearchTimeoutMs <= 0)
				m_TargetSearchTimeoutMs = runtimeDefaults.m_TargetSearchTimeoutMs;
			if (m_TargetSearchTimeoutMs > 120000)
				m_TargetSearchTimeoutMs = 120000;
			if (m_TargetSearchUpdateIntervalMs <= 0)
				m_TargetSearchUpdateIntervalMs = runtimeDefaults.m_TargetSearchUpdateIntervalMs;
			if (m_TargetSearchUpdateIntervalMs < 300)
				m_TargetSearchUpdateIntervalMs = 300;
			if (m_TargetSearchUpdateIntervalMs > 15000)
				m_TargetSearchUpdateIntervalMs = 15000;
			if (m_TargetSearchReachM <= 0.0)
				m_TargetSearchReachM = runtimeDefaults.m_TargetSearchReachM;
			if (m_TargetSearchReachM < 0.5)
				m_TargetSearchReachM = 0.5;
			if (m_TargetSearchReachM > 8.0)
				m_TargetSearchReachM = 8.0;
			if (m_ChaseStopDistanceM < 0.8)
				m_ChaseStopDistanceM = 0.8;
			if (m_ChaseStopDistanceM > 20.0)
				m_ChaseStopDistanceM = 20.0;
			if (m_ChaseJogDistanceM < 1.5)
				m_ChaseJogDistanceM = 1.5;
			if (m_ChaseJogDistanceM > 80.0)
				m_ChaseJogDistanceM = 80.0;
			if (m_ChaseSprintDistanceM < (m_ChaseJogDistanceM + 1.0))
				m_ChaseSprintDistanceM = m_ChaseJogDistanceM + 1.0;
			if (m_ChaseSprintDistanceM > 300.0)
				m_ChaseSprintDistanceM = 300.0;
			if (m_ChaseProgressCheckMs < 100)
				m_ChaseProgressCheckMs = 100;
			if (m_ChaseProgressCheckMs > 2000)
				m_ChaseProgressCheckMs = 2000;
			if (m_ChaseStuckRepathMs < (m_ChaseProgressCheckMs * 2))
				m_ChaseStuckRepathMs = m_ChaseProgressCheckMs * 2;
			if (m_ChaseStuckRepathMs > 15000)
				m_ChaseStuckRepathMs = 15000;
			if (m_ChaseStrafeDurationMs < 120)
				m_ChaseStrafeDurationMs = 120;
			if (m_ChaseStrafeDurationMs > 2500)
				m_ChaseStrafeDurationMs = 2500;
			if (m_CoverSearchRadius < 6.0)
				m_CoverSearchRadius = 6.0;
			if (m_CoverSearchRadius > 60.0)
				m_CoverSearchRadius = 60.0;
			if (m_CoverMinDistance < 1.0)
				m_CoverMinDistance = 1.0;
			if (m_CoverMaxDistance < (m_CoverMinDistance + 1.0))
				m_CoverMaxDistance = m_CoverMinDistance + 1.0;
			if (m_CoverMaxDistance > m_CoverSearchRadius)
				m_CoverMaxDistance = m_CoverSearchRadius;
			m_CoverThreatThreshold = Math.Clamp(m_CoverThreatThreshold, 0.2, 0.95);
			if (m_CoverRecheckIntervalMs < 400)
				m_CoverRecheckIntervalMs = 400;
			if (m_CoverRecheckIntervalMs > 10000)
				m_CoverRecheckIntervalMs = 10000;
			if (m_MaxFlankingDistance < 6.0)
				m_MaxFlankingDistance = 6.0;
			if (m_MaxFlankingDistance > 200.0)
				m_MaxFlankingDistance = 200.0;
			m_FlankThreatMin = Math.Clamp(m_FlankThreatMin, 0.05, 0.85);
			m_FlankThreatMax = Math.Clamp(m_FlankThreatMax, 0.1, 0.95);
			if (m_FlankThreatMax <= m_FlankThreatMin)
				m_FlankThreatMax = m_FlankThreatMin + 0.1;
			if (m_FlankThreatMax > 0.99)
				m_FlankThreatMax = 0.99;
			if (m_FlankTimeoutMs < 1000)
				m_FlankTimeoutMs = 1000;
			if (m_FlankTimeoutMs > 60000)
				m_FlankTimeoutMs = 60000;
			if (m_MovementSpeedLimit <= 0)
				m_MovementSpeedLimit = runtimeDefaults.m_MovementSpeedLimit;
			if (m_MovementSpeedLimitUnderThreat <= 0)
				m_MovementSpeedLimitUnderThreat = runtimeDefaults.m_MovementSpeedLimitUnderThreat;
			if (m_RoamingMovementSpeedLimit <= 0)
				m_RoamingMovementSpeedLimit = runtimeDefaults.m_RoamingMovementSpeedLimit;
			if (m_SpeedLimitPreference <= 0)
				m_SpeedLimitPreference = runtimeDefaults.m_SpeedLimitPreference;
			if (m_FOVFarDistThreshold <= 0.0)
				m_FOVFarDistThreshold = runtimeDefaults.m_FOVFarDistThreshold;
			if (m_FOVNearHalfAngleH <= 0.0)
				m_FOVNearHalfAngleH = runtimeDefaults.m_FOVNearHalfAngleH;
			if (m_FOVFarHalfAngleH <= 0.0)
				m_FOVFarHalfAngleH = runtimeDefaults.m_FOVFarHalfAngleH;
			if (m_ThreatCombatEnter <= 0.0)
				m_ThreatCombatEnter = runtimeDefaults.m_ThreatCombatEnter;
			if (m_MovementSpeedLimit < 1)
				m_MovementSpeedLimit = 1;
			if (m_MovementSpeedLimit > 3)
				m_MovementSpeedLimit = 3;
			if (m_MovementSpeedLimitUnderThreat < 1)
				m_MovementSpeedLimitUnderThreat = 1;
			if (m_MovementSpeedLimitUnderThreat > 3)
				m_MovementSpeedLimitUnderThreat = 3;
			if (m_RoamingMovementSpeedLimit < 1)
				m_RoamingMovementSpeedLimit = 1;
			if (m_RoamingMovementSpeedLimit > 3)
				m_RoamingMovementSpeedLimit = 3;
			if (m_SpeedLimitPreference < 1)
				m_SpeedLimitPreference = 1;
			if (m_SpeedLimitPreference > 3)
				m_SpeedLimitPreference = 3;
			if (m_FOVNearDistThreshold < 0.0)
				m_FOVNearDistThreshold = 0.0;
			if (m_FOVFarDistThreshold < (m_FOVNearDistThreshold + 1.0))
				m_FOVFarDistThreshold = m_FOVNearDistThreshold + 1.0;
			m_FOVNearHalfAngleH = Math.Clamp(m_FOVNearHalfAngleH, 35.0, 179.0);
			m_FOVFarHalfAngleH = Math.Clamp(m_FOVFarHalfAngleH, 10.0, m_FOVNearHalfAngleH);
			m_ThreatCombatEnter = Math.Clamp(m_ThreatCombatEnter, 0.15, 0.95);
			if (m_Settings.m_Runtime.m_AccuracyBase < 0.0)
				m_Settings.m_Runtime.m_AccuracyBase = 0.0;
			if (m_Settings.m_Runtime.m_AccuracyBase > 1.0)
				m_Settings.m_Runtime.m_AccuracyBase = 1.0;
			if (m_Settings.m_Runtime.m_AccuracyNearDistance < 1.0)
				m_Settings.m_Runtime.m_AccuracyNearDistance = 1.0;
			if (m_Settings.m_Runtime.m_AccuracyFarDistance < (m_Settings.m_Runtime.m_AccuracyNearDistance + 1.0))
				m_Settings.m_Runtime.m_AccuracyFarDistance = m_Settings.m_Runtime.m_AccuracyNearDistance + 1.0;
			if (m_Settings.m_Runtime.m_AccuracyNearFactor < 0.0)
				m_Settings.m_Runtime.m_AccuracyNearFactor = 0.0;
			if (m_Settings.m_Runtime.m_AccuracyNearFactor > 1.0)
				m_Settings.m_Runtime.m_AccuracyNearFactor = 1.0;
			if (m_Settings.m_Runtime.m_AccuracyFarFactor < 0.0)
				m_Settings.m_Runtime.m_AccuracyFarFactor = 0.0;
			if (m_Settings.m_Runtime.m_AccuracyFarFactor > 1.0)
				m_Settings.m_Runtime.m_AccuracyFarFactor = 1.0;
			if (m_Settings.m_Runtime.m_AccuracyMaxSpreadDeg < 0.0)
				m_Settings.m_Runtime.m_AccuracyMaxSpreadDeg = 0.0;
			if (m_Settings.m_Runtime.m_AccuracyMaxSpreadDeg > 45.0)
				m_Settings.m_Runtime.m_AccuracyMaxSpreadDeg = 45.0;
			if (m_Settings.m_DamageMultiplier < 0.0)
				m_Settings.m_DamageMultiplier = 0.0;
			if (m_Settings.m_DamageMultiplier > 10.0)
				m_Settings.m_DamageMultiplier = 10.0;

			if (VERBOSE_LOGS)
			{
				string cfgMsg = "[cfg] searchTimeoutMs=" + m_TargetSearchTimeoutMs.ToString();
				cfgMsg = cfgMsg + " searchUpdateMs=" + m_TargetSearchUpdateIntervalMs.ToString();
				cfgMsg = cfgMsg + " searchReachM=" + m_TargetSearchReachM.ToString();
				Print(LOG_PREFIX + cfgMsg);
			}

			ApplyQueuedSpawnOverride();
			ResolveGroupSetupFromSpawn();

	if (migratedSpawnClass)
			{
			}
		}

		protected void ApplyQueuedSpawnOverride()
		{
			if (!s_QueuedSpawnOverrides || s_NextQueuedSpawnOverrideIndex >= s_QueuedSpawnOverrides.Count())
				return;

			BQBotSpawnConfig queued = s_QueuedSpawnOverrides.Get(s_NextQueuedSpawnOverrideIndex);
			s_NextQueuedSpawnOverrideIndex++;
			if (!queued)
				return;

			if (!m_Settings.m_Spawn)
				m_Settings.m_Spawn = new BQBotSpawnConfig();

			if (queued.m_Classname != "")
				m_Settings.m_Spawn.m_Classname = queued.m_Classname;
			if (queued.m_Position != "")
				m_Settings.m_Spawn.m_Position = queued.m_Position;
			if (queued.m_Orientation != "")
				m_Settings.m_Spawn.m_Orientation = queued.m_Orientation;
			m_Settings.m_Spawn.m_AllowDamage = queued.m_AllowDamage;
		}

		protected void ApplyQueuedSettingsOverride()
		{
			if (!s_QueuedSettingsOverrides || s_NextQueuedSettingsOverrideIndex >= s_QueuedSettingsOverrides.Count())
				return;

			BQBotSettings queued = s_QueuedSettingsOverrides.Get(s_NextQueuedSettingsOverrideIndex);
			s_NextQueuedSettingsOverrideIndex++;
			if (!queued)
				return;

			m_Settings = queued;
		}

		protected void ApplyConfiguredClothing()
		{
			if (!m_Bot)
				return;

			ReplaceAttachmentAtSlot(InventorySlots.HEADGEAR, GetHeadType());
			ReplaceAttachmentAtSlot(InventorySlots.BODY, GetTopType());
			ReplaceAttachmentAtSlot(InventorySlots.LEGS, GetPantsType());
			ReplaceAttachmentAtSlot(InventorySlots.FEET, GetFeetType());
			ReplaceAttachmentAtSlot(InventorySlots.VEST, GetVestType());
			ReplaceAttachmentAtSlot(InventorySlots.BACK, GetBackpackType());
			ReplaceAttachmentAtSlot(InventorySlots.GLOVES, GetGlovesType());
			ReplaceAttachmentAtSlot(InventorySlots.MASK, GetMaskType());
		}

		protected void ReplaceAttachmentAtSlot(int slotId, string itemType)
		{
			if (!m_Bot || itemType == "")
				return;

			EntityAI existing = m_Bot.GetInventory().FindAttachment(slotId);
			if (existing && existing.GetType() == itemType)
				return;

			if (existing)
				GetGame().ObjectDelete(existing);

			m_Bot.GetInventory().CreateAttachment(itemType);
		}

		protected string GetSpawnClassname()
		{
			string classname = "bqBots_Boris_AI";
			if (m_Settings && m_Settings.m_Spawn && m_Settings.m_Spawn.m_Classname != "")
				classname = m_Settings.m_Spawn.m_Classname;

			// Backward compatibility for old configs.
			if (classname == "bqBots_Boris")
				classname = "bqBots_Boris_AI";

			// Safety fallback in case animation addon wasn't loaded.
			if (GetGame() && !GetGame().ConfigIsExisting("CfgVehicles " + classname))
				classname = "bqBots_Boris";

			return classname;
		}

		protected vector GetSpawnPosition()
		{
			if (m_Settings && m_Settings.m_Spawn && m_Settings.m_Spawn.m_Position != "")
				return ParseVectorFromConfig(m_Settings.m_Spawn.m_Position, "6108.52 298.921 7634.82");
			return "6108.52 298.921 7634.82";
		}

		protected vector GetSpawnOrientation()
		{
			if (m_Settings && m_Settings.m_Spawn && m_Settings.m_Spawn.m_Orientation != "")
				return ParseVectorFromConfig(m_Settings.m_Spawn.m_Orientation, "0 0 0");
			return "0 0 0";
		}

		protected vector ParseVectorFromConfig(string raw, string fallback)
		{
			if (raw == "")
				return fallback.ToVector();

			string normalized = raw;
			normalized.Replace("<", "");
			normalized.Replace(">", "");
			normalized.Replace(",", " ");
			normalized.Replace(";", " ");
			return normalized.ToVector();
		}

		protected bool GetSpawnAllowDamage()
		{
			if (m_Settings && m_Settings.m_Spawn)
				return m_Settings.m_Spawn.m_AllowDamage != 0;
			return true;
		}

		protected string GetWeaponType()
		{
			if (m_Settings && m_Settings.m_Weapon && m_Settings.m_Weapon.m_Weapon != "")
				return m_Settings.m_Weapon.m_Weapon;
			return "AKM";
		}

		protected string GetMagazineType()
		{
			if (m_Settings && m_Settings.m_Weapon && m_Settings.m_Weapon.m_Magazine != "")
				return m_Settings.m_Weapon.m_Magazine;
			return "Mag_AKM_30Rnd";
		}

		protected int GetMagazineAmmo()
		{
			if (m_Settings && m_Settings.m_Weapon && m_Settings.m_Weapon.m_MagAmmo > 0)
				return m_Settings.m_Weapon.m_MagAmmo;
			return 30;
		}

		protected int GetReserveMagAmmo()
		{
			if (m_Settings && m_Settings.m_Weapon && m_Settings.m_Weapon.m_ReserveMagAmmo > 0)
				return m_Settings.m_Weapon.m_ReserveMagAmmo;
			return 30;
		}

		protected string GetFireMode()
		{
			if (m_Settings && m_Settings.m_Weapon)
			{
				string mode = m_Settings.m_Weapon.m_FireMode;
				// Accept both "semi" and "single" spellings.
				if (mode == "semi" || mode == "Semi" || mode == "SEMI")
					return "single";
				if (mode == "single" || mode == "Single" || mode == "SINGLE")
					return "single";
				if (mode == "burst" || mode == "Burst" || mode == "BURST")
					return "burst";
			}
			return "auto";
		}

		protected int GetFireIntentModeId()
		{
			string fireMode = GetFireMode();
			if (fireMode == "burst")
				return FIRE_INTENT_MODE_BURST;
			if (fireMode == "single")
				return FIRE_INTENT_MODE_SINGLE;
			return FIRE_INTENT_MODE_AUTO;
		}

		protected float GetDetectRadius()
		{
			float radius = 300.0;
			if (m_Settings && m_Settings.m_Runtime && m_Settings.m_Runtime.m_DetectRadius > 0.0)
				radius = m_Settings.m_Runtime.m_DetectRadius;
			else if (m_Settings && m_Settings.m_Runtime && m_Settings.m_Runtime.m_AggroRadius > 0.0)
				radius = m_Settings.m_Runtime.m_AggroRadius;

			return Math.Max(radius, BQBOT_FORCE_DETECT_RADIUS_M);
		}
		protected float GetVisibilityMultiplier()
		{
			float visibility = 1.0;

			if (GetGame())
			{
				Weather weather = GetGame().GetWeather();
				if (weather)
				{
					visibility = visibility - weather.GetFog().GetActual() * 0.45;
					visibility = visibility - weather.GetRain().GetActual() * 0.20;
					visibility = visibility - weather.GetOvercast().GetActual() * 0.10;
				}

				if (GetGame().GetWorld())
				{
					int year;
					int month;
					int day;
					int hour;
					int minute;
					GetGame().GetWorld().GetDate(year, month, day, hour, minute);
					if (hour < 5 || hour >= 22)
						visibility = visibility - 0.35;
					else if (hour < 7 || hour >= 20)
						visibility = visibility - 0.20;
				}
			}

			return Math.Clamp(visibility, 0.35, 1.0);
		}

		protected float GetEffectiveDetectRadius()
		{
			return Math.Max(GetDetectRadius() * GetVisibilityMultiplier(), BQBOT_FORCE_DETECT_RADIUS_M);
		}
		protected float GetFireRadius()
		{
			float radius = 250.0;
			if (m_Settings && m_Settings.m_Runtime && m_Settings.m_Runtime.m_FireRadius > 0.0)
				radius = m_Settings.m_Runtime.m_FireRadius;
			else if (m_Settings && m_Settings.m_Runtime && m_Settings.m_Runtime.m_AggroRadius > 0.0)
				radius = m_Settings.m_Runtime.m_AggroRadius;

			return Math.Max(radius, BQBOT_FORCE_FIRE_RADIUS_M);
		}
		protected string GetHeadType()
		{
			if (m_Settings && m_Settings.m_Clothing)
				return m_Settings.m_Clothing.m_Head;
			return "";
		}

		protected string GetTopType()
		{
			if (m_Settings && m_Settings.m_Clothing)
				return m_Settings.m_Clothing.m_Top;
			return "";
		}

		protected string GetPantsType()
		{
			if (m_Settings && m_Settings.m_Clothing)
				return m_Settings.m_Clothing.m_Pants;
			return "";
		}

		protected string GetFeetType()
		{
			if (m_Settings && m_Settings.m_Clothing)
				return m_Settings.m_Clothing.m_Feet;
			return "";
		}

		protected string GetVestType()
		{
			if (m_Settings && m_Settings.m_Clothing)
				return m_Settings.m_Clothing.m_Vest;
			return "PressVest_Blue";
		}

		protected string GetBackpackType()
		{
			if (m_Settings && m_Settings.m_Clothing)
				return m_Settings.m_Clothing.m_Backpack;
			return "TaloonBag_Blue";
		}

		protected string GetGlovesType()
		{
			if (m_Settings && m_Settings.m_Clothing)
				return m_Settings.m_Clothing.m_Gloves;
			return "";
		}

		protected string GetMaskType()
		{
			if (m_Settings && m_Settings.m_Clothing)
				return m_Settings.m_Clothing.m_Mask;
			return "";
		}

	protected void EnsureWeaponReady()
		{
			EnsureWeaponReady(true);
		}

		protected void EnsureWeaponReady(bool force)
		{
			if (!m_BotWeapon)
				return;

			int now = GetGame().GetTime();
			if (!force && now < m_NextMaintenanceTime)
				return;

			Magazine mag = Magazine.Cast(m_BotWeapon.GetMagazine(m_BotWeapon.GetCurrentMuzzle()));
			if (!mag)
			{
				StageLog("READY_NO_MAG", "force=" + BoolTo01(force));
				// Important: attach via vanilla weapon event path.
				// Direct CreateAttachment() can race with weapon FSM and produce duplicate pending events.
				EnsureReserveMagazine(null);
				if (m_ReserveMag && m_ReserveMag.GetAmmoCount() < GetReserveMagAmmo())
					m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());

				DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
				bool invReady = inv && !inv.IsProcessing();
				bool weaponBusy = !m_BotWeapon.IsIdle() || m_BotWeapon.IsWaitingForActionFinish();
				bool waitingSameAttach = (m_ReloadIncomingMag && m_ReserveMag && m_ReloadIncomingMag == m_ReserveMag && m_ReloadStartTime > 0 && (now - m_ReloadStartTime) < 30000);

				// Sync internal state with external vanilla FSM activity.
				if (!m_IsReloading && (!invReady || weaponBusy))
				{
					m_IsReloading = true;
					if (m_ReloadStartTime <= 0)
						m_ReloadStartTime = now;
					m_ReloadPrevCurrentMag = null;
					m_ReloadIncomingMag = m_ReserveMag;
					StageLog("RELOAD_SYNC_EXTERNAL", "invReady=" + BoolTo01(invReady) + " weaponBusy=" + BoolTo01(weaponBusy), true);
					ReloadDebug("reload sync: external pending action detected invReady=" + BoolTo01(invReady) + " weaponBusy=" + BoolTo01(weaponBusy), true);
				}
				else if (!m_IsReloading && waitingSameAttach)
				{
					// We already posted ATTACH_MAGAZINE for this exact reserve mag.
					// Keep reload state latched to avoid duplicate PostWeaponEvent while vanilla queue is pending.
					m_IsReloading = true;
					StageLog("RELOAD_WAIT_PENDING_ATTACH", "ageMs=" + (now - m_ReloadStartTime).ToString() + " reserveSame=1", true);
				}

				if (!m_IsReloading)
				{
					WeaponManager wm = m_Bot.GetWeaponManager();
					bool canAttach = false;
					if (wm && m_ReserveMag)
						canAttach = wm.CanAttachMagazine(m_BotWeapon, m_ReserveMag, false);

					if (invReady && !weaponBusy && canAttach)
					{
						if (!CanPostWeaponEvent())
						{
							StageLog("RELOAD_POSTPONED", "reason=input_user_data_full");
							m_NextMaintenanceTime = now + 200;
							return;
						}
						WeaponTrace("post_event_attach", "reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag), true);
						inv.PostWeaponEvent(new WeaponEventAttachMagazine(m_Bot, m_ReserveMag));
						m_IsReloading = true;
						s_ActiveReloadOperations++;
						m_ReloadStartTime = now;
						m_ReloadPrevCurrentMag = null;
						m_ReloadIncomingMag = m_ReserveMag;
						StageLog("RELOAD_ATTACH_POSTED", "source=EnsureWeaponReady");
						m_NextReloadAttemptTime = now + 1400;
						VerboseLog("reload: started vanilla attach (no current mag)");
					}
					else
					{
						StageLog("RELOAD_ATTACH_SKIP", "invReady=" + BoolTo01(invReady) + " weaponBusy=" + BoolTo01(weaponBusy) + " canAttach=" + BoolTo01(canAttach));
						ReloadDebug("ready attach skipped: invReady=" + BoolTo01(invReady) + " weaponBusy=" + BoolTo01(weaponBusy) + " canAttach=" + BoolTo01(canAttach), true);
					}
				}

				m_NextMaintenanceTime = now + m_MaintenanceIntervalMs;
				return;
			}

			EnsureReserveMagazine(mag);
			if (m_ReserveMag && m_ReserveMag.GetAmmoCount() < GetReserveMagAmmo())
				m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());

			// One-time chamber prep on spawn/maintenance if needed.
			int muzzle = m_BotWeapon.GetCurrentMuzzle();
			if (force && (m_BotWeapon.IsChamberEmpty(muzzle) || m_BotWeapon.IsChamberFiredOut(muzzle)) && mag && mag.GetAmmoCount() > 0)
				m_BotWeapon.FillChamber();

			m_NextMaintenanceTime = now + m_MaintenanceIntervalMs;
		}

		protected void EnsureReserveMagazine(Magazine currentMag)
		{
			if (!m_Bot || !m_BotWeapon)
				return;

			bool needNewReserve = false;
			if (!m_ReserveMag)
				needNewReserve = true;
			else if (m_ReserveMag == currentMag)
				needNewReserve = true;
			else if (m_ReserveMag.GetHierarchyRootPlayer() != m_Bot)
				needNewReserve = true;
			else if (m_ReserveMag.IsDamageDestroyed())
				needNewReserve = true;

			if (needNewReserve)
			{
				m_ReserveMag = Magazine.Cast(m_Bot.GetInventory().CreateInInventory(GetMagazineType()));
				if (m_ReserveMag)
				{
					m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());
					VerboseLog("created reserve magazine");
				}
			}

			PruneExtraReserveMagazines(m_ReserveMag, currentMag);
		}

		protected string GetMagRootName(Magazine mag)
		{
			if (!mag)
				return "none";

			Object root = mag.GetHierarchyRoot();
			if (!root)
				return "none";

			return root.GetType();
		}

		protected string GetMagParentName(Magazine mag)
		{
			if (!mag)
				return "none";

			Object parent = mag.GetHierarchyParent();
			if (!parent)
				return "none";

			return parent.GetType();
		}

		protected bool RebuildReserveMagazine(Magazine currentMag)
		{
			if (!m_Bot)
				return false;

			Magazine rebuilt = Magazine.Cast(m_Bot.GetInventory().CreateInInventory(GetMagazineType()));
			if (!rebuilt)
			{
				ReloadDebug("reload blocked: reserve rebuild failed");
				return false;
			}

			rebuilt.ServerSetAmmoCount(GetReserveMagAmmo());
			m_ReserveMag = rebuilt;
			PruneExtraReserveMagazines(m_ReserveMag, currentMag);
			ReloadDebug("reload: reserve rebuilt");
			return true;
		}

		protected void PruneExtraReserveMagazines(Magazine keepReserve, Magazine keepCurrentMag)
		{
			if (!m_Bot || !GetGame())
				return;

			GameInventory inv = m_Bot.GetInventory();
			if (!inv)
				return;

			DayZPlayerInventory playerInv = m_Bot.GetDayZPlayerInventory();
			if (playerInv && playerInv.IsProcessing())
				return;

			array<EntityAI> itemsArray = new array<EntityAI>();
			inv.EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

			string magType = GetMagazineType();
			foreach (EntityAI item : itemsArray)
			{
				Magazine mag = Magazine.Cast(item);
				if (!mag)
					continue;

				if (mag.GetType() != magType)
					continue;

				if (mag == keepReserve || mag == keepCurrentMag)
					continue;

				if (mag.GetHierarchyRootPlayer() != m_Bot)
					continue;

				GetGame().ObjectDelete(mag);
			}
		}

	protected void HandleReload()
		{
			if (!m_Bot || !m_BotWeapon)
				return;

			int now = GetGame().GetTime();
			if (now < m_NextReloadAttemptTime)
			{
				StageLog("RELOAD_WAIT_COOLDOWN", "leftMs=" + (m_NextReloadAttemptTime - now).ToString());
				return;
			}

			if (m_IsReloading)
			{
				StageLog("RELOAD_WAIT_ACTIVE", "reason=flag_set");
				return;
			}

			if (s_ActiveReloadOperations >= MAX_CONCURRENT_RELOADS)
			{
				StageLog("RELOAD_GLOBAL_QUEUE_FULL", "activeOps=" + s_ActiveReloadOperations.ToString() + " max=" + MAX_CONCURRENT_RELOADS.ToString());
				m_NextReloadAttemptTime = now + RELOAD_QUEUE_CHECK_MS;
				return;
			}

			int muzzle = m_BotWeapon.GetCurrentMuzzle();
			Magazine currentMag = Magazine.Cast(m_BotWeapon.GetMagazine(muzzle));
			bool needReload = false;

			if (!currentMag)
				needReload = true;
			else if (currentMag.GetAmmoCount() <= 0)
				needReload = true;

			if (!needReload)
				return;
			StageLog("RELOAD_NEEDED", "hasMag=" + BoolTo01(currentMag != null) + " magAmmo=" + GetMagAmmoSafe(currentMag), true);
			WeaponTrace("reload_needed", "hasMag=" + BoolTo01(currentMag != null) + " magAmmo=" + GetMagAmmoSafe(currentMag), true);

			ReloadDebug("need reload: currentMag=" + BoolTo01(currentMag != null) + " currentAmmo=" + GetMagAmmoSafe(currentMag), true);

			EnsureReserveMagazine(currentMag);
			if (!m_ReserveMag)
			{
				ReloadDebug("reload blocked: reserve magazine missing");
				return;
			}

			if (m_ReserveMag.GetAmmoCount() < GetReserveMagAmmo())
				m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());

			bool reloadingStarted = false;
			DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
			if (!inv)
			{
				StageLog("RELOAD_BLOCK", "reason=no_inventory", true);
				ReloadDebug("reload blocked: no inventory component");
				return;
			}

			if (inv.IsProcessing())
			{
				StageLog("RELOAD_DELAY", "reason=inventory_processing");
				ReloadDebug("reload delayed: inventory processing");
				return;
			}

			if (!m_BotWeapon.IsIdle() || m_BotWeapon.IsWaitingForActionFinish())
			{
				StageLog("RELOAD_DELAY", "reason=weapon_busy");
				ReloadDebug("reload delayed: weapon FSM busy");
				return;
			}

			if (currentMag)
			{
				WeaponManager wm = m_Bot.GetWeaponManager();
				bool canSwap = false;
				if (wm)
					canSwap = wm.CanSwapMagazine(m_BotWeapon, m_ReserveMag, false);
				ReloadDebug("reload check: canSwap=" + BoolTo01(canSwap) + " currentAmmo=" + GetMagAmmoSafe(currentMag) + " reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag), true);
				if (!canSwap)
				{
					ReloadDebug("reload detail: reserveParent=" + GetMagParentName(m_ReserveMag) + " reserveRoot=" + GetMagRootName(m_ReserveMag) + " sameAsCurrent=" + BoolTo01(m_ReserveMag == currentMag), true);
					if (RebuildReserveMagazine(currentMag) && wm)
					{
						canSwap = wm.CanSwapMagazine(m_BotWeapon, m_ReserveMag, false);
						ReloadDebug("reload retry: canSwap=" + BoolTo01(canSwap) + " reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag), true);
					}
				}
	if (canSwap)
				{
					InventoryLocation reserveSrc = new InventoryLocation();
					if (m_ReserveMag.GetInventory().GetCurrentInventoryLocation(reserveSrc))
					{
						if (!CanPostWeaponEvent())
						{
							StageLog("RELOAD_POSTPONED", "reason=input_user_data_full_swap");
							m_NextReloadAttemptTime = now + 200;
							return;
						}
						WeaponTrace("post_event_swap", "source=HandleReload currentAmmo=" + GetMagAmmoSafe(currentMag) + " reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag), true);
						inv.PostWeaponEvent(new WeaponEventSwapMagazine(m_Bot, m_ReserveMag, reserveSrc));
						reloadingStarted = true;
						m_ReloadPrevCurrentMag = currentMag;
						m_ReloadIncomingMag = m_ReserveMag;
					}
					else
					{
						ReloadDebug("reload blocked: cannot get reserve magazine location");
					}

	if (reloadingStarted)
					{
						m_IsReloading = true;
						s_ActiveReloadOperations++;
						m_ReloadStartTime = now;
						StageLog("RELOAD_SWAP_POSTED", "source=HandleReload", true);
						VerboseLog("reload: started vanilla swap animation");
					}
				}
				else
				{
					ReloadDebug("reload blocked: CanSwapMagazine false");
				}
			}
			else
			{
				WeaponManager wm2 = m_Bot.GetWeaponManager();
				bool canAttach = false;
				if (wm2)
					canAttach = wm2.CanAttachMagazine(m_BotWeapon, m_ReserveMag, false);
	ReloadDebug("reload check: canAttach=" + BoolTo01(canAttach) + " reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag), true);
				if (canAttach)
				{
					if (!CanPostWeaponEvent())
					{
						StageLog("RELOAD_POSTPONED", "reason=input_user_data_full_attach");
						m_NextReloadAttemptTime = now + 200;
						return;
					}
					WeaponTrace("post_event_attach", "source=HandleReloadAttach reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag), true);
					inv.PostWeaponEvent(new WeaponEventAttachMagazine(m_Bot, m_ReserveMag));
					reloadingStarted = true;
					m_ReloadPrevCurrentMag = null;
					m_ReloadIncomingMag = m_ReserveMag;
					if (reloadingStarted)
					{
						m_IsReloading = true;
						s_ActiveReloadOperations++;
						m_ReloadStartTime = now;
						StageLog("RELOAD_ATTACH_POSTED", "source=HandleReload", true);
						VerboseLog("reload: started vanilla attach animation");
					}
				}
				else
				{
					ReloadDebug("reload blocked: CanAttachMagazine false");
				}
			}

			if (reloadingStarted)
				m_NextReloadAttemptTime = now + 1400;
		}

		protected void FinalizeReloadState()
		{
			if (!m_IsReloading || !m_BotWeapon)
				return;

			int now = GetGame().GetTime();
			if (m_ReloadStartTime <= 0)
			{
				m_ReloadStartTime = now;
				StageLog("RELOAD_REPAIR", "reason=missing_start_time", true);
				ReloadDebug("reload state repaired: missing start time");
				return;
			}
			int muzzle = m_BotWeapon.GetCurrentMuzzle();
			Magazine currentMag = Magazine.Cast(m_BotWeapon.GetMagazine(muzzle));
			DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
			bool invBusy = inv && inv.IsProcessing();
			bool weaponBusy = !m_BotWeapon.IsIdle() || m_BotWeapon.IsWaitingForActionFinish();
			bool minReloadTimePassed = (now - m_ReloadStartTime) >= 900;

	bool finished = false;
			if (currentMag && m_ReloadIncomingMag && currentMag == m_ReloadIncomingMag)
				finished = true;

			// Force finish if stuck too long OR weapon is wrongly marked busy
			if ((now - m_ReloadStartTime) > 2500)
			{
				weaponBusy = false;
				minReloadTimePassed = true;
			}

			if (finished)
			{
				// Allow fire after shorter wait or if stuck too long
				if (!minReloadTimePassed || weaponBusy)
				{
					StageLog("RELOAD_WAIT_SETTLE", "minTime=" + BoolTo01(minReloadTimePassed) + " invBusy=" + BoolTo01(invBusy) + " weaponBusy=" + BoolTo01(weaponBusy));
					ReloadDebug("reload waiting settle: minTime=" + BoolTo01(minReloadTimePassed) + " invBusy=" + BoolTo01(invBusy) + " weaponBusy=" + BoolTo01(weaponBusy));
					return;
				}

				Magazine oldMag = m_ReloadPrevCurrentMag;
				if (oldMag && oldMag.GetHierarchyRootPlayer() == m_Bot && !oldMag.IsDamageDestroyed())
				{
					m_ReserveMag = oldMag;
					if (m_ReserveMag.GetAmmoCount() < GetReserveMagAmmo())
						m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());
				}
				else
				{
					m_ReserveMag = null;
				}

				PruneExtraReserveMagazines(m_ReserveMag, currentMag);

				string doneDbg = "reload: completed";
				doneDbg = doneDbg + " currentAmmo=" + GetMagAmmoSafe(currentMag);
				doneDbg = doneDbg + " reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag);
				doneDbg = doneDbg + " chamberEmpty=" + BoolTo01(m_BotWeapon.IsChamberEmpty(muzzle));
				doneDbg = doneDbg + " chamberFiredOut=" + BoolTo01(m_BotWeapon.IsChamberFiredOut(muzzle));
				VerboseLog(doneDbg);
	StageLog("RELOAD_DONE", "currentAmmo=" + GetMagAmmoSafe(currentMag) + " reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag), true);
				ArmPostReloadSync("reload_done", true);
				m_IsReloading = false;
				if (s_ActiveReloadOperations > 0)
				{
					s_ActiveReloadOperations--;
					if (s_ActiveReloadOperations < 0)
						s_ActiveReloadOperations = 0;
				}
				m_ReloadPrevCurrentMag = null;
				m_ReloadIncomingMag = null;

				// Keep active mag state vanilla here; it already comes from reserve mag swap.
				// Important: call after m_IsReloading=false, otherwise EnsureRoundChambered exits early.
				EnsureRoundChambered("reload-finish", true);
				return;
			}

			if ((now - m_ReloadStartTime) > 5000 && !invBusy && !weaponBusy)
			{
				// Never timeout-reset when we are still waiting for a posted incoming mag attach/swap.
				// Otherwise EnsureWeaponReady can post duplicate ATTACH_MAGAZINE and crash with pending event already posted.
				if (!currentMag && m_ReloadIncomingMag)
				{
					StageLog("RELOAD_FALLBACK_SKIP", "reason=pending_incoming_mag ageMs=" + (now - m_ReloadStartTime).ToString(), true);
					return;
				}

				if (currentMag)
				{
					currentMag.ServerSetAmmoCount(GetMagazineAmmo());
				}

				EnsureReserveMagazine(currentMag);
				if (m_ReserveMag && m_ReserveMag.GetAmmoCount() < GetReserveMagAmmo())
					m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());

	ReloadDebug("reload fallback: timeout reached while idle, ammo restored");
				StageLog("RELOAD_FALLBACK", "timeoutMs=" + (now - m_ReloadStartTime).ToString(), true);
				ArmPostReloadSync("reload_fallback", true);
				m_IsReloading = false;
				if (s_ActiveReloadOperations > 0)
				{
					s_ActiveReloadOperations--;
					if (s_ActiveReloadOperations < 0)
						s_ActiveReloadOperations = 0;
				}
				m_ReloadPrevCurrentMag = null;
				m_ReloadIncomingMag = null;
			}
		}

		protected void ArmPostReloadSync(string reason, bool force = false)
		{
			if (!m_BotWeapon)
				return;

			int now = GetGame().GetTime();
			int newUntil = now + POST_RELOAD_SYNC_DELAY_MS;
			if (force || newUntil > m_PostReloadSyncUntilTime)
				m_PostReloadSyncUntilTime = newUntil;

			m_BotWeapon.Synchronize();
			StageLog("POST_RELOAD_SYNC_ARM", "reason=" + reason + " waitMs=" + POST_RELOAD_SYNC_DELAY_MS.ToString(), true);
		}

		protected void ReloadDebug(string msg, bool force = false)
		{
			if (!VERBOSE_LOGS)
				return;

			int now = GetGame().GetTime();
			if (!force && now < m_NextReloadDebugTime)
				return;

			m_NextReloadDebugTime = now + 500;
			VerboseLog(msg);
		}

		protected string GetMagAmmoSafe(Magazine mag)
		{
			if (!mag)
				return "none";

			return mag.GetAmmoCount().ToString();
		}



		protected void WeaponTrace(string tag, string details = "", bool force = false)
		{
			return;
		}

		protected string GetCoreStateName(int state)
		{
			if (state == CORE_STATE_IDLE)
				return "idle";
			if (state == CORE_STATE_PURSUIT)
				return "pursuit";
			if (state == CORE_STATE_ENGAGE)
				return "engage";
			return "unknown";
		}

		protected string GetTacticalStateName(int state)
		{
			if (state == TACTICAL_STATE_IDLE)
				return "idle";
			if (state == TACTICAL_STATE_PURSUIT)
				return "pursuit";
			if (state == TACTICAL_STATE_TRAVERSING_WAYPOINTS)
				return "traversing_waypoints";
			if (state == TACTICAL_STATE_FIGHTING_POSITIONING)
				return "fighting_positioning";
			if (state == TACTICAL_STATE_FIGHTING_FIREWEAPON)
				return "fighting_fireweapon";
			if (state == TACTICAL_STATE_FIGHTING_EVADE)
				return "fighting_evade";
			if (state == TACTICAL_STATE_FOLLOW_FORMATION)
				return "follow_formation";
			if (state == TACTICAL_STATE_FLANK)
				return "flank";
			return "unknown";
		}

		protected void SetCoreState(int state, int now)
		{
			if (m_CoreState == state)
				return;

			m_PrevCoreState = m_CoreState;
			m_CoreState = state;
			MovementDiag("state", "from=" + GetCoreStateName(m_PrevCoreState) + " to=" + GetCoreStateName(m_CoreState) + " now=" + now.ToString(), true);
			if (VERBOSE_LOGS)
				StageLog("STATE", "from=" + GetCoreStateName(m_PrevCoreState) + " to=" + GetCoreStateName(m_CoreState), true);
		}

		protected void SetTacticalState(int state, int now)
		{
			if (m_TacticalState == state)
				return;

			m_PrevTacticalState = m_TacticalState;
			m_TacticalState = state;
			m_TacticalStateEnterTime = now;
			MovementDiag("tactical_state", "from=" + GetTacticalStateName(m_PrevTacticalState) + " to=" + GetTacticalStateName(m_TacticalState) + " now=" + now.ToString(), true);
			if (VERBOSE_LOGS)
				StageLog("TACTICAL_STATE", "from=" + GetTacticalStateName(m_PrevTacticalState) + " to=" + GetTacticalStateName(m_TacticalState), true);
		}

		protected void InitFSMStates()
		{
			if (!m_FSMStates)
				m_FSMStates = new map<int, ref BQBotStateBase>();

			m_FSMStates.Clear();
			m_FSMActiveState = null;
			m_FSMActiveStateId = -1;
			m_FSMTransitionRequested = false;
			m_FSMRequestedStateId = TACTICAL_STATE_IDLE;
		}

		protected void TransitionFSMState(int desiredState, int now)
		{
			m_FSMActiveState = null;
			m_FSMActiveStateId = desiredState;
			m_FSMTransitionRequested = false;
		}

		protected int ResolveDesiredFSMState(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			if (!target)
			{
				if (m_AggroLatched)
					return TACTICAL_STATE_PURSUIT;
				if (FSM_GuardTraversingWaypoints(target))
					return TACTICAL_STATE_TRAVERSING_WAYPOINTS;
				return TACTICAL_STATE_IDLE;
			}

			vector botPos = m_Bot.GetPosition();
			bool hasLos = HasLineOfSightToTarget(target);
			vector targetPos = GetTargetMemoryPosition(target, hasLos);
			float distToTarget = vector.Distance(botPos, targetPos);
			float fireRadius = GetFireRadius();
			bool inFireRadius = vector.DistanceSq(botPos, targetPos) <= fireRadius * fireRadius;
			bool canFire = inFireRadius && hasLos;
			vector formationGoalPos;
			bool hasFormationGoal = TryGetGroupFormationGoal(target, now, formationGoalPos);
			return ResolveTacticalState(now, target, hasLos, canFire, hasFormationGoal, distToTarget);
		}

		int FSM_ResolveNextState(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			return ResolveDesiredFSMState(now, detectRadius, previousTarget, target);
		}

		int FSM_ResolveNextStateAfterFlank(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			if (!target)
			{
				if (m_AggroLatched)
					return TACTICAL_STATE_PURSUIT;
				if (FSM_GuardTraversingWaypoints(target))
					return TACTICAL_STATE_TRAVERSING_WAYPOINTS;
				return TACTICAL_STATE_IDLE;
			}

			vector botPos = m_Bot.GetPosition();
			bool hasLos = HasLineOfSightToTarget(target);
			vector targetPos = GetTargetMemoryPosition(target, hasLos);
			float distToTarget = vector.Distance(botPos, targetPos);
			float fireRadius = GetFireRadius();
			float fireRadiusSq = fireRadius * fireRadius;
			bool inFireRadius = vector.DistanceSq(botPos, targetPos) <= fireRadiusSq;
			bool canFire = inFireRadius && hasLos;
			if (canFire)
				return TACTICAL_STATE_FIGHTING_FIREWEAPON;

			vector formationGoalPos;
			bool hasFormationGoal = TryGetGroupFormationGoal(target, now, formationGoalPos);
			float formationFollowMinDist = Math.Max(m_ChaseStopDistanceM + 6.0, 12.0);
			if (!hasLos && m_AggroLatched)
				formationFollowMinDist = Math.Max(m_ChaseStopDistanceM + 3.0, 8.0);
			bool formationBypassActive = IsFormationBypassActive(now);
			bool allowFormationFollow = hasFormationGoal && !m_FlankActive && !m_CoverMoveActive && !formationBypassActive;
			if (allowFormationFollow)
			{
				bool formationStateIn = m_GroupFormationState == GROUP_FORMATION_STATE_IN;
				bool forceNoLosFormationFollow = !hasLos && m_AggroLatched;
				if ((formationStateIn || forceNoLosFormationFollow) && distToTarget >= formationFollowMinDist)
					return TACTICAL_STATE_FOLLOW_FORMATION;
			}

			if (!hasLos && m_AggroLatched)
				return TACTICAL_STATE_PURSUIT;

			return TACTICAL_STATE_FIGHTING_POSITIONING;
		}

		void FSM_RequestTransition(int stateId)
		{
			m_FSMRequestedStateId = stateId;
			m_FSMTransitionRequested = true;
		}

		bool FSM_TryConsumeTransition(out int stateId)
		{
			stateId = TACTICAL_STATE_IDLE;
			if (!m_FSMTransitionRequested)
				return false;

			stateId = m_FSMRequestedStateId;
			m_FSMTransitionRequested = false;
			return true;
		}

		void FSM_ForceTacticalState(int state)
		{
			m_FSMForcedTacticalState = state;
			m_FSMForcedTacticalStateActive = true;
		}

		void FSM_ClearForcedTacticalState()
		{
			m_FSMForcedTacticalStateActive = false;
		}

		int FSM_GetTacticalStateIdle() { return TACTICAL_STATE_IDLE; }
		int FSM_GetTacticalStatePursuit() { return TACTICAL_STATE_PURSUIT; }
		int FSM_GetTacticalStateTraversingWaypoints() { return TACTICAL_STATE_TRAVERSING_WAYPOINTS; }
		int FSM_GetTacticalStatePositioning() { return TACTICAL_STATE_FIGHTING_POSITIONING; }
		int FSM_GetTacticalStateFireWeapon() { return TACTICAL_STATE_FIGHTING_FIREWEAPON; }
		int FSM_GetTacticalStateEvade() { return TACTICAL_STATE_FIGHTING_EVADE; }
		int FSM_GetTacticalStateFollowFormation() { return TACTICAL_STATE_FOLLOW_FORMATION; }
		int FSM_GetTacticalStateFlank() { return TACTICAL_STATE_FLANK; }

		void FSM_SetShouldTakeCover(bool state)
		{
			// Disabled: Expansion FSM owns cover intent.
		}

		bqBots_Boris FSM_GetBot()
		{
			return m_Bot;
		}

		ExpansionPathHandler FSM_GetPathFinding()
		{
			return null;
		}

		int FSM_GetWaypointBehaviour()
		{
			return m_GroupWaypointBehaviour;
		}

		array<vector> FSM_GetGroupWaypoints()
		{
			return new array<vector>();
		}

		bool FSM_IsFormationLeader()
		{
			if (m_GroupId == "")
				return false;

			array<BQBotShooterManager> members = new array<BQBotShooterManager>();
			if (!CollectAliveGroupMembers(members) || members.Count() <= 0)
				return false;

			BQBotShooterManager leader = members.Get(0);
			return leader == this;
		}

		int FSM_GetCurrentWaypointIndex()
		{
			return m_GroupPatrolWaypointIndex;
		}

		void FSM_SetCurrentWaypointIndex(int index)
		{
		}

		bool FSM_GetWaypointBacktracking()
		{
			return m_GroupPatrolBacktracking;
		}

		void FSM_SetWaypointBacktracking(bool backtracking)
		{
		}

		void FSM_SetCurrentWaypoint(vector waypoint)
		{
		}

		vector FSM_FindClosestRoamingLocationPosition()
		{
			array<vector> waypoints = FSM_GetGroupWaypoints();
			vector center = "0 0 0";
			if (waypoints && waypoints.Count() > 0)
			{
				float bestSq = 999999999.0;
				vector botPos = m_Bot.GetPosition();
				foreach (vector wp : waypoints)
				{
					float dSq = vector.DistanceSq(botPos, wp);
					if (dSq < bestSq)
					{
						bestSq = dSq;
						center = wp;
					}
				}
			}
			else if (m_Bot)
			{
				center = m_Bot.GetPosition();
			}

			float roamMin = 8.0;
			float roamMax = 30.0;
			if (m_GroupWaypointBehaviour == GROUP_WAYPOINT_ROAMING_LOCAL)
			{
				roamMin = 4.0;
				roamMax = 12.0;
			}

			float angleDeg = Math.RandomFloat(0.0, 360.0);
			float angleRad = angleDeg * Math.DEG2RAD;
			vector radial = Vector(Math.Sin(angleRad), 0.0, Math.Cos(angleRad));
			vector roamGoal = center + radial * Math.RandomFloat(roamMin, roamMax);
			vector projected = ProjectToNavigablePosition(roamGoal, 3.0);
			if (projected != "0 0 0")
				return projected;
			return roamGoal;
		}

		void FSM_SetRoamingLocationReached(bool waypointReached)
		{
			MovementDiag("wp_roaming_reached", "reached=" + BoolTo01(waypointReached), true);
		}

		void FSM_LeaveCover()
		{
			m_CoverMoveActive = false;
			m_CoverPosition = "0 0 0";
			m_IsInCover = false;
			FSM_SetShouldTakeCover(false);
		}

		void FSM_SetBestSpeedLimit(int speedLimit)
		{
		}

		int FSM_GetSpeedLimitPreference()
		{
			if (m_SpeedLimitPreference <= 0)
				return 3;
			return m_SpeedLimitPreference;
		}

		float FSM_GetMovementSpeed()
		{
			if (!m_Bot)
				return 0.0;
			return m_Bot.BQBot_GetCurrentMovementSpeed();
		}

		float FSM_GetCurrentWaterLevel()
		{
			if (!m_Bot)
				return 0.0;
			return m_Bot.GetCurrentWaterLevel();
		}

		void FSM_GetUp(bool force = false)
		{
			if (!m_Bot)
				return;

			HumanCommandMove cm = m_Bot.GetCommand_Move();
			if (!cm && force)
			{
				m_Bot.StartCommand_Move();
				cm = m_Bot.GetCommand_Move();
			}
			if (cm)
				cm.ForceStanceUp(DayZPlayerConstants.STANCEIDX_ERECT);
		}

		void FSM_ResetPathfinding()
		{
			// Disabled. Expansion owns path reset.
		}

		protected PlayerBase ResolveExpansionMemoryTarget(float detectRadius)
		{
			if (!m_Bot)
				return null;

			eAITarget eaiTarget = m_Bot.GetTarget();
			if (!eaiTarget || !eaiTarget.IsActive())
				return null;

			EntityAI entity = eaiTarget.GetEntity();
			PlayerBase player;
			if (!Class.CastTo(player, entity))
				return null;
			if (!IsCandidateHumanTarget(player))
				return null;

			if (detectRadius > 0.0)
			{
				float distSq = vector.DistanceSq(m_Bot.GetPosition(), player.GetPosition());
				if (distSq > detectRadius * detectRadius)
					return null;
			}

			if (eaiTarget.m_ThreatLevelActive <= 0.0999 && eaiTarget.m_ThreatLevel <= 0.0999)
				return null;

			return player;
		}

		protected bool IsExpansionMemoryTarget(PlayerBase target)
		{
			if (!target || !m_Bot)
				return false;

			eAITarget eaiTarget = m_Bot.GetTarget();
			if (!eaiTarget || !eaiTarget.IsActive())
				return false;

			EntityAI entity = eaiTarget.GetEntity();
			if (entity != target)
				return false;

			return eaiTarget.m_ThreatLevelActive > 0.0999 || eaiTarget.m_ThreatLevel > 0.0999;
		}

		protected eAIWaypointBehavior ToExpansionWaypointBehaviour()
		{
			if (!m_GroupPatrolEnabled)
				return eAIWaypointBehavior.HALT;
			if (m_GroupWaypointBehaviour == GROUP_WAYPOINT_ALTERNATE)
				return eAIWaypointBehavior.ALTERNATE;
			if (m_GroupWaypointBehaviour == GROUP_WAYPOINT_HALT)
				return eAIWaypointBehavior.HALT;
			if (m_GroupWaypointBehaviour == GROUP_WAYPOINT_ROAMING)
				return eAIWaypointBehavior.ROAMING;
			if (m_GroupWaypointBehaviour == GROUP_WAYPOINT_ROAMING_LOCAL)
				return eAIWaypointBehavior.ROAMING_LOCAL;
			return eAIWaypointBehavior.LOOP;
		}

		protected void SyncExpansionGroupState(int now)
		{
			// Disabled: Expansion owns group state, formation and waypoints.
		}

		protected void SyncExpansionTarget(PlayerBase target, bool hasLos, vector targetPos, float threat)
		{
			// Disabled: Expansion eAIBase owns target acquisition, LOS and threat.
		}

		protected void SyncExpansionTargetState(eAITarget eaiTarget, bool hasLos, vector targetPos, float threat)
		{
			// Disabled: Expansion eAIBase owns eAITargetInformationState.
		}
		bool FSM_GuardEvade(int now, PlayerBase target, float distToTarget, bool hasLos)
		{
			if (!target)
				return false;
			return CanEnterEvadeState(now, distToTarget, hasLos);
		}

		bool FSM_GuardTraversingWaypoints(PlayerBase target)
		{
			// Disabled: BQ tactical FSM is not allowed to drive movement.
			return false;
		}

		bool FSM_GuardFlank(PlayerBase target)
		{
			// Disabled: Expansion XML FSM/eAIState_Flank owns flank transitions.
			return false;
		}

		bool FSM_HasLineOfSight(PlayerBase target)
		{
			if (!target || !m_Bot)
				return false;

			return HasLineOfSightToTarget(target);
		}

		bool FSM_CanFireAtTarget(PlayerBase target)
		{
			if (!target || !m_Bot)
				return false;

			vector botPos = m_Bot.GetPosition();
			bool hasLos = HasLineOfSightToTarget(target);
			vector targetPos = GetTargetMemoryPosition(target, hasLos);
			float fireRadius = GetFireRadius();
			bool inFireRadius = vector.DistanceSq(botPos, targetPos) <= fireRadius * fireRadius;
			if (!inFireRadius)
				return false;

			return hasLos;
		}

		void FSM_UpdateNoTarget(int now, float detectRadius, PlayerBase previousTarget)
		{
			UpdateStateNoTarget(now, detectRadius, previousTarget);
		}

		void FSM_UpdateEngageTarget(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			UpdateStateEngageTarget(now, detectRadius, previousTarget, target);
		}

		// State-owned update entry points (Expansion-style orchestration):
		// each FSM state drives its own behavior branch, transitions happen after state tick.
		void FSM_UpdateIdleState(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			UpdateStateNoTarget(now, detectRadius, previousTarget);
		}

		void FSM_UpdatePursuitState(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			if (target)
				UpdateStateEngageTarget(now, detectRadius, previousTarget, target, TACTICAL_STATE_PURSUIT);
			else
				UpdateStateNoTarget(now, detectRadius, previousTarget);
		}

		void FSM_UpdatePositioningState(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			if (!target)
				UpdateStateNoTarget(now, detectRadius, previousTarget);
			else
				UpdateStateEngageTarget(now, detectRadius, previousTarget, target, TACTICAL_STATE_FIGHTING_POSITIONING);
		}

		void FSM_UpdateFireWeaponState(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			if (!target)
				UpdateStateNoTarget(now, detectRadius, previousTarget);
			else
				UpdateStateEngageTarget(now, detectRadius, previousTarget, target, TACTICAL_STATE_FIGHTING_FIREWEAPON);
		}

		void FSM_UpdateEvadeState(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			if (!target)
				UpdateStateNoTarget(now, detectRadius, previousTarget);
			else
				UpdateStateEngageTarget(now, detectRadius, previousTarget, target, TACTICAL_STATE_FIGHTING_EVADE);
		}

		void FSM_UpdateFollowFormationState(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			if (!target)
				UpdateStateNoTarget(now, detectRadius, previousTarget);
			else
				UpdateStateEngageTarget(now, detectRadius, previousTarget, target, TACTICAL_STATE_FOLLOW_FORMATION);
		}

		void FSM_UpdateFlankState(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target)
		{
			if (!target)
				UpdateStateNoTarget(now, detectRadius, previousTarget);
			else
				UpdateStateEngageTarget(now, detectRadius, previousTarget, target, TACTICAL_STATE_FLANK);
		}

		protected bool CanEnterEvadeState(int now, float distToTarget, bool hasLos)
		{
			if (!m_Bot)
				return false;
			float threat = m_Bot.BQBot_GetThreatToSelf(false);
			if (distToTarget < 8.0 && threat > 0.3)
				return true;
			if (!hasLos && distToTarget < 25.0)
				return true;
			if (threat > 0.7)
				return true;
			return false;
		}

		protected int ResolveTacticalState(int now, PlayerBase target, bool hasLos, bool canFire, bool hasFormationGoal, float distToTarget)
		{
			if (!target)
			{
				if (m_AggroLatched)
					return TACTICAL_STATE_PURSUIT;
				if (FSM_GuardTraversingWaypoints(target))
					return TACTICAL_STATE_TRAVERSING_WAYPOINTS;
				return TACTICAL_STATE_IDLE;
			}


			// Expansion Flank guard runs on threat/target/group state, not after our local fire branch.
			if (FSM_GuardFlank(target))
				return TACTICAL_STATE_FLANK;

			if (canFire)
				return TACTICAL_STATE_FIGHTING_FIREWEAPON;


			float formationFollowMinDist = Math.Max(m_ChaseStopDistanceM + 6.0, 12.0);
			if (!hasLos && m_AggroLatched)
				formationFollowMinDist = Math.Max(m_ChaseStopDistanceM + 3.0, 8.0);
			bool formationBypassActive = IsFormationBypassActive(now);
			bool allowFormationFollow = hasFormationGoal && !m_FlankActive && !m_CoverMoveActive && !formationBypassActive;
			if (allowFormationFollow)
			{
				bool formationStateIn = m_GroupFormationState == GROUP_FORMATION_STATE_IN;
				bool forceNoLosFormationFollow = !hasLos && m_AggroLatched;
				if ((formationStateIn || forceNoLosFormationFollow) && distToTarget >= formationFollowMinDist)
					return TACTICAL_STATE_FOLLOW_FORMATION;
			}

			if (!hasLos && m_AggroLatched)
				return TACTICAL_STATE_PURSUIT;

			return TACTICAL_STATE_FIGHTING_POSITIONING;
		}

		protected void Update()
		{
			if (!m_Bot || !m_BotWeapon)
				return;

			PerfOnUpdateTick();
			m_UpdateCounter++;
			int now = GetGame().GetTime();

			if (!m_Bot.IsAlive())
			{
				m_Bot.BQBot_SetCombatTargetContext(null, 0.0, 0.0);
				m_Bot.BQBot_SetVanillaTriggerHoldWanted(false);
				m_Bot.BQBot_SetCombatPoseWanted(false);
				m_CurrentTarget = null;
				return;
			}

			/*
				PURE EXPANSION MOVEMENT / AGGRO MODE.

				This manager must not search targets, sync LOS/threat, set eAI fighting state,
				write waypoints, or call OverrideTargetPosition. Expansion eAIBase owns:
				  - target acquisition / aggro distance
				  - LOS and threat ramp
				  - Master/Fighting FSM transitions
				  - Flank / Positioning / Cover / Pathfinding movement points

				BQ stays only as weapon backend. It may prepare the weapon and mirror the
				current Expansion target for firebridge/aim pitch, but it may not feed data
				back into Expansion movement decisions.
			*/
			RefreshWeaponStateFromHands();
			m_Bot.BQBot_SetFireIntentMode(GetFireIntentModeId());
			FinalizeReloadState();
			EnsureWeaponReady(false);
			HandleReload();

			if (m_Bot.BQBot_ConsumePostReloadSyncNeeded())
				ArmPostReloadSync("expansion_fireweapon", true);

			if (TryHandleWeaponJam(now))
			{
				m_Bot.BQBot_SetVanillaTriggerHoldWanted(false);
				return;
			}

			eAITarget eaiTarget = m_Bot.GetTarget();
			PlayerBase target;
			if (eaiTarget)
				Class.CastTo(target, eaiTarget.GetEntity());

			m_CurrentTarget = target;

			if (!target)
			{
				m_Bot.BQBot_SetCombatTargetContext(null, 0.0, 0.0);
				m_Bot.BQBot_SetVanillaTriggerHoldWanted(false);
				m_WasInFireRadius = false;
				return;
			}

			float activeThreat = m_Bot.GetThreatToSelf();
			float absoluteThreat = m_Bot.GetThreatToSelf(true);
			m_Bot.BQBot_SetCombatTargetContext(target, activeThreat, absoluteThreat);

			vector aimPos = eaiTarget.GetPosition(false) + eaiTarget.GetAimOffset();
			m_Bot.BQBot_SetAimTarget(aimPos);
		}

		protected void UpdateStateNoTarget(int now, float detectRadius, PlayerBase previousTarget)
		{
			if (!m_AggroLatched)
			{
				SetTacticalState(TACTICAL_STATE_IDLE, now);
				ResetTargetSearchState();
				ResetTacticalMoveState();
				m_CurrentTarget = null;
				m_Bot.BQBot_SetVanillaTriggerHoldWanted(false);
				m_Bot.BQBot_ClearAimHeading();
				DisableCombatPose();
				StopChaseMovement(true);
				m_WasInFireRadius = false;
				m_FireEntrySyncUntilTime = 0;
				m_PostReloadSyncUntilTime = 0;
				m_BurstShotsRemaining = 0;
				m_NextBurstShotTime = 0;
				m_NextBurstStartTime = 0;
				m_NextSingleRequestTime = 0;
				m_LastTargetVisibleTime = 0;
				if (VERBOSE_LOGS)
					StageLog("IDLE_GUARD", "holding position until first target detection", true);
				return;
			}

			SearchDiag("no_target_branch", "hasLastKnown=" + BoolTo01(m_HasLastKnownTargetPos) + " searchActive=" + BoolTo01(m_TargetSearchActive));

			vector pursuitGoal;
			bool hasPursuitGoal = TryGetAggroPursuitGoal(now, pursuitGoal);
			if (hasPursuitGoal)
			{
				if (!m_TargetSearchActive || m_TargetSearchPosition == "0 0 0")
					StartTargetSearch(pursuitGoal, now);

				UpdateTargetSearchPosition(now);
				vector pursuitSeedGoal = pursuitGoal;
				if (CanContinueTargetSearch(now) && m_TargetSearchPosition != "0 0 0")
					pursuitSeedGoal = m_TargetSearchPosition;

				PlayerBase softPursuitTarget = null;
				if (IsCandidateHumanTarget(previousTarget))
					softPursuitTarget = previousTarget;
				else
					softPursuitTarget = GetNearestPlayerNoLos(detectRadius);

				float pursuitDist = vector.Distance(m_Bot.GetPosition(), pursuitSeedGoal);
				m_CurrentTarget = softPursuitTarget;
				eAI_OverrideSearchPosition(softPursuitTarget, pursuitSeedGoal, pursuitDist, now, "no_target_aggro_memory");
				m_WasInFireRadius = false;
				if (VERBOSE_LOGS)
					StageLog("PURSUIT_NO_LOS", "goal=" + pursuitSeedGoal.ToString() + " dist=" + pursuitDist.ToString(), true);
				return;
			}

			if (!m_HasLastKnownTargetPos && !m_TargetSearchActive)
			{
				PlayerBase noLosCandidate = GetNearestPlayerNoLos(detectRadius);
				if (IsCandidateHumanTarget(noLosCandidate))
				{
					vector fallbackGoal = ProjectToNavigablePosition(noLosCandidate.GetPosition(), 3.0);
					m_LastKnownTargetPos = fallbackGoal;
					m_HasLastKnownTargetPos = true;
					SyncTargetSearchStateLastKnownFromMemory();
					m_TargetSearchOnLOSLost = true;
					SearchPacketDiag("packet_los_lost", "fallbackGoal=" + fallbackGoal.ToString(), true);
					StartTargetSearch(fallbackGoal, now);

					UpdateTargetSearchPosition(now);
					vector fallbackSeedGoal = fallbackGoal;
					if (CanContinueTargetSearch(now) && m_TargetSearchPosition != "0 0 0")
						fallbackSeedGoal = m_TargetSearchPosition;

					float fallbackDist = vector.Distance(m_Bot.GetPosition(), fallbackSeedGoal);
					m_CurrentTarget = noLosCandidate;
					eAI_OverrideSearchPosition(noLosCandidate, fallbackSeedGoal, fallbackDist, now, "no_target_nearest_no_los");
					m_WasInFireRadius = false;
					if (VERBOSE_LOGS)
						StageLog("PURSUIT_NO_LOS_FALLBACK", "goal=" + fallbackSeedGoal.ToString() + " dist=" + fallbackDist.ToString(), true);
					return;
				}
			}

			if (CanContinueTargetSearch(now))
			{
				UpdateTargetSearchPosition(now);
				vector searchPos = m_TargetSearchPosition;
				float searchDist = vector.Distance(m_Bot.GetPosition(), searchPos);
				SearchDiag("search_only_move", "goal=" + searchPos.ToString() + " dist=" + searchDist.ToString());

				m_CurrentTarget = null;
				eAI_OverrideSearchPosition(null, searchPos, searchDist, now, "no_target_search_only");
				m_WasInFireRadius = false;

				if (VERBOSE_LOGS)
				{
					int searchAgeMs = now - m_TargetSearchStartTime;
					StageLog("SEARCH_ONLY", "goal=" + searchPos.ToString() + " dist=" + searchDist.ToString() + " ageMs=" + searchAgeMs.ToString(), true);
				}
				return;
			}

			SearchDiag("search_only_stop", "reason=no_target_and_cannot_continue", true);

			ResetTargetSearchState();
			ResetTacticalMoveState();
			SetTacticalState(TACTICAL_STATE_IDLE, now);
			MovementDiag("no_target_stop", "reason=no_target_and_search_timeout", true);
			m_AggroLatched = false;
			m_CurrentTarget = null;
			m_Bot.BQBot_SetVanillaTriggerHoldWanted(false);
			m_Bot.BQBot_ClearAimHeading();
			DisableCombatPose();
			StopChaseMovement(true);
			m_WasInFireRadius = false;
			m_FireEntrySyncUntilTime = 0;
			m_PostReloadSyncUntilTime = 0;
			m_BurstShotsRemaining = 0;
			m_NextBurstShotTime = 0;
			m_NextBurstStartTime = 0;
			m_NextSingleRequestTime = 0;
			m_LastTargetVisibleTime = 0;
			CombatDiagNoTarget(detectRadius);
			if (VERBOSE_LOGS)
			{
				float nearestDist = GetNearestPlayerDistanceCached();
				StageLog("NO_TARGET", "nearestDist=" + nearestDist.ToString() + " detectRadius=" + detectRadius.ToString() + " tick=" + m_UpdateCounter.ToString());
				DebugLog("no target in detect radius; nearestDist=" + nearestDist.ToString() + " detectRadius=" + detectRadius.ToString());
			}
		}

		// === Expansion-style movement helpers ===

		protected void eAI_OverrideSearchPosition(PlayerBase softTarget, vector searchGoal, float distToGoal, int now, string reason)
		{
			// Disabled. Search/memory movement belongs to Expansion target/FSM logic.
		}


		// Min firing distance: Expansion uses optic zeroing or weapon zeroing*0.3.
		protected float eAI_GetWeaponMinDistance()
		{
			if (!m_Bot)
				return 30.0;
			Weapon_Base weapon;
			if (Class.CastTo(weapon, m_Bot.GetHumanInventory().GetEntityInHands()))
			{
				ItemOptics optics = weapon.GetAttachedOptics();
				float md = 0.0;
				if (optics)
					md = optics.GetZeroingDistanceZoomMax();
				else
					md = weapon.GetZeroingDistanceZoomMax(weapon.GetCurrentMuzzle()) * 0.3;
				if (md > 1000.0)
					md = 1000.0;
				if (md < 5.0)
					md = 5.0;
				return md;
			}
			return 30.0;
		}

		protected bool eAI_IsInFlankRange(float dist)
		{
			if (!m_FlankingEnabled)
				return false;
			if (dist <= 5.0)
				return false;
			if (m_MaxFlankingDistance > 0.0 && dist > m_MaxFlankingDistance)
				return false;
			return true;
		}

		protected vector eAI_GetRandomPointInCircle(vector center, float radius)
		{
			float angle = Math.RandomFloat(0.0, Math.PI2);
			float r = Math.RandomFloat(radius * 0.4, radius);
			vector p = center;
			p[0] = p[0] + Math.Cos(angle) * r;
			p[2] = p[2] + Math.Sin(angle) * r;
			return p;
		}

		// Expansion eAIBase.OverrideTargetPosition port
		// cover (raised+LOS), flank (mid range), minDist hold, or direct chase.
		// Honors goal lock so positioning cannot rewrite cover/flank in motion.
		protected bool eAI_ShouldUpdatePosition(int now)
		{
			return true;
		}

		protected void eAI_SetPositioningOverride(vector goalPos, bool isFinal, int lockMs, bool allowJumpClimb, string reason, PlayerBase target, float distToTarget, bool hasLos, float threat, int now)
		{
			// Disabled. Cover/flank/min-distance points are selected by eAIBase.OverrideTargetPosition(eAITarget).
		}

		protected bool eAI_TrySelectCoverPosition(vector targetPos, int now, out vector coverPos)
		{
			coverPos = "0 0 0";
			if (!m_CoverEnabled)
			{
				MovementDiag("cover_select", "reason=disabled", true);
				return false;
			}
			if (now < m_NextCoverCheckTime)
				return false;

			coverPos = FindCoverPosition(targetPos);
			if (coverPos != "0 0 0")
			{
				m_CoverPosition = coverPos;
				m_CoverMoveActive = true;
				m_NextCoverCheckTime = now + m_CoverRecheckIntervalMs;
				string coverOkDiag = "reason=ok pos=" + coverPos.ToString();
				coverOkDiag = coverOkDiag + " target=" + targetPos.ToString();
				MovementDiag("cover_select", coverOkDiag, true);
				return true;
			}

			m_NextCoverCheckTime = now + 600;
			string coverFailDiag = "reason=no_position target=" + targetPos.ToString();
			coverFailDiag = coverFailDiag + " radius=" + m_CoverSearchRadius.ToString();
			MovementDiag("cover_select", coverFailDiag, true);
			return false;
		}
		protected bool eAI_TrySelectDirectionalCoverPosition(vector targetPos, vector direction, bool flank, float distToTarget, float minDist, bool keepMinDistToTarget, int now, out vector coverPos)
		{
			coverPos = "0 0 0";
			if (!m_Bot || !m_CoverEnabled)
				return false;
			if (now < m_NextCoverCheckTime)
				return false;

			vector botPos = m_Bot.GetPosition();
			UpdatePotentialCoverObjectsExpansionStyle(botPos);
			if (!m_PotentialCoverObjects || m_PotentialCoverObjects.Count() == 0)
			{
				m_NextCoverCheckTime = now + 600;
				return false;
			}

			direction[1] = 0.0;
			float dirLenSq = direction[0] * direction[0] + direction[2] * direction[2];
			if (dirLenSq <= 0.0001)
				return false;
			float invDirLen = 1.0 / Math.Sqrt(dirLenSq);
			direction[0] = direction[0] * invDirLen;
			direction[2] = direction[2] * invDirLen;

			float distSq = distToTarget * distToTarget;
			float minDistSq = minDist * minDist;
			float bestObjDistSq = 99999999.0;
			Object bestCoverObj = null;

			foreach (Object coverObj : m_PotentialCoverObjects)
			{
				if (!coverObj)
					continue;
				if (flank && coverObj == m_CurrentCoverObject)
					continue;
				if (IsCoverObjectReservedByOther(coverObj))
					continue;

				vector toObj = coverObj.GetPosition() - botPos;
				toObj[1] = 0.0;
				float objDistSq = toObj[0] * toObj[0] + toObj[2] * toObj[2];
				if (objDistSq <= 0.0001)
					continue;

				float invObjDist = 1.0 / Math.Sqrt(objDistSq);
				vector toObjNorm;
				toObjNorm[0] = toObj[0] * invObjDist;
				toObjNorm[1] = 0.0;
				toObjNorm[2] = toObj[2] * invObjDist;

				float dot = direction[0] * toObjNorm[0] + direction[2] * toObjNorm[2];
				if (flank)
				{
					if (dot < 0.75)
						continue;
				}
				else
				{
					if ((keepMinDistToTarget || distSq <= objDistSq) && dot >= 0.0)
						continue;
				}

				if (keepMinDistToTarget && vector.DistanceSq(coverObj.GetPosition(), targetPos) <= minDistSq)
					continue;

				if (objDistSq < bestObjDistSq)
				{
					bestObjDistSq = objDistSq;
					bestCoverObj = coverObj;
				}
			}

			if (!bestCoverObj)
			{
				m_NextCoverCheckTime = now + 600;
				return false;
			}

			coverPos = GetCoverPositionBehindObject(bestCoverObj, targetPos);
			if (coverPos == "0 0 0")
			{
				m_NextCoverCheckTime = now + 600;
				return false;
			}

			m_CurrentCoverObject = bestCoverObj;
			m_CoverPosition = coverPos;
			m_CoverMoveActive = true;
			m_NextCoverCheckTime = now + m_CoverRecheckIntervalMs;
			string coverDiag = "reason=directional_ok pos=" + coverPos.ToString();
			coverDiag = coverDiag + " obj=" + bestCoverObj.ToString();
			coverDiag = coverDiag + " flank=" + BoolTo01(flank);
			MovementDiag("cover_select", coverDiag, true);
			return true;
		}

		protected void eAI_OverrideTargetPosition(PlayerBase target, vector targetPos, float distToTarget, bool hasLos, float threat, int now)
		{
			// Disabled: only Expansion FSM states may call OverrideTargetPosition.
		}
		// Expansion owns close-range movement through positioning/flank. Keep this stub so old
		// call sites cannot reintroduce custom side-step goals.
		protected void eAI_DoEvadeStep(PlayerBase target, vector targetPos, int now)
		{
		}
		protected void UpdateStateEngageTarget(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target, int forcedTacticalState = -1)
		{
			// Disabled. BQ tactical state machine is not allowed to drive Expansion targets or movement.
		}

		protected PlayerBase GetNearestPlayer(float maxRadius)
		{
			array<Man> players = GetPlayersSnapshot();
			PerfCountTargetScan();

			PlayerBase best;
			float bestDistSq = maxRadius * maxRadius;
			float closeAwarenessSq = CLOSE_AWARENESS_RADIUS_M * CLOSE_AWARENESS_RADIUS_M;
			vector botPos = m_Bot.GetPosition();

			foreach (Man man : players)
			{
				PerfCountCandidate();
				PlayerBase player = PlayerBase.Cast(man);
				if (!IsCandidateHumanTarget(player))
					continue;

				float distSq = vector.DistanceSq(botPos, player.GetPosition());
				if (distSq > bestDistSq)
					continue;
				if (distSq > closeAwarenessSq && !IsInsideDynamicFov(player))
					continue;

				if (!HasLineOfSightToTarget(player))
					continue;

				if (distSq < bestDistSq)
				{
					bestDistSq = distSq;
					best = player;
				}
			}

			return best;
		}

		protected PlayerBase GetNearestPlayerNoLos(float maxRadius)
		{
			array<Man> players = GetPlayersSnapshot();
			PerfCountTargetScan();

			PlayerBase best;
			float bestDistSq = maxRadius * maxRadius;
			float closeAwarenessSq = CLOSE_AWARENESS_RADIUS_M * CLOSE_AWARENESS_RADIUS_M;
			bool allowFullAware = m_AggroLatched;
			vector botPos = m_Bot.GetPosition();

			foreach (Man man : players)
			{
				PerfCountCandidate();
				PlayerBase player = PlayerBase.Cast(man);
				if (!IsCandidateHumanTarget(player))
					continue;

				float distSq = vector.DistanceSq(botPos, player.GetPosition());
				if (distSq > bestDistSq)
					continue;
				if (!allowFullAware && distSq > closeAwarenessSq && !IsInsideDynamicFov(player))
					continue;

				if (distSq < bestDistSq)
				{
					bestDistSq = distSq;
					best = player;
				}
			}

			return best;
		}

		protected PlayerBase GetNearestRecentShotNoiseTarget(float maxRadius, int now, out vector heardPos, out int heardAgeMs)
		{
			heardPos = "0 0 0";
			heardAgeMs = 0;
			array<Man> players = GetPlayersSnapshot();
			if (!players)
				return null;

			vector botPos = m_Bot.GetPosition();
			float bestDistSq = maxRadius * maxRadius;
			PlayerBase best;

			foreach (Man man : players)
			{
				PlayerBase player = PlayerBase.Cast(man);
				if (!IsCandidateHumanTarget(player))
					continue;

				int shotTime;
				vector shotPos;
				if (!player.BQBot_GetPlayerShotNoise(shotTime, shotPos))
					continue;

				int ageMs = now - shotTime;
				if (ageMs < 0 || ageMs > BQBOT_SHOT_HEARING_MEMORY_MS)
					continue;

				float distSq = vector.DistanceSq(botPos, shotPos);
				if (distSq > bestDistSq)
					continue;

				bestDistSq = distSq;
				best = player;
				heardPos = shotPos;
				heardAgeMs = ageMs;
			}

			return best;
		}
		protected PlayerBase GetCachedTarget(float detectRadius)
		{
			int now = GetGame().GetTime();
			bool currentValid = false;
			if (m_CurrentTarget)
				currentValid = IsTargetValid(m_CurrentTarget, detectRadius);

			if (currentValid)
			{
				if (now < m_NextTargetScanTime)
				{
					if (VERBOSE_LOGS)
						StageLog("TARGET_CACHED", "nextScanInMs=" + (m_NextTargetScanTime - now).ToString() + " detectRadius=" + detectRadius.ToString());
					return m_CurrentTarget;
				}
			}

			if (!TryConsumeTargetScanBudget(now))
			{
				int deferMs = 80;
				if (m_TargetScanIntervalMs < deferMs)
					deferMs = m_TargetScanIntervalMs;
				m_NextTargetScanTime = now + deferMs;
				if (currentValid)
					return m_CurrentTarget;

				return null;
			}

			PlayerBase trackedTarget = m_CurrentTarget;
			PlayerBase scannedTarget = GetNearestPlayer(detectRadius);
			if (scannedTarget)
			{
				m_CurrentTarget = scannedTarget;
			}
			else if (currentValid)
			{
				// Keep remembered aggro target when scan misses due temporary LOS break.
				m_CurrentTarget = trackedTarget;
			}
			else if (CanContinueTargetSearch(now))
			{
				// Expansion-like fallback: while search is active, keep nearest candidate
				// even without direct LOS so chase/search behavior does not hard-stop.
				m_CurrentTarget = GetNearestPlayerNoLos(detectRadius);
				string candidate = "none";
				if (m_CurrentTarget)
					candidate = m_CurrentTarget.GetType();
				SearchDiag("cached_no_los_fallback", "candidate=" + candidate + " detectRadius=" + detectRadius.ToString());
			}
			else
			{
				m_CurrentTarget = null;
			}
			int nextScanMs = m_TargetScanIntervalMs;
			if (!m_CurrentTarget)
				nextScanMs = m_TargetScanNoTargetIntervalMs;
			m_NextTargetScanTime = now + nextScanMs;
			if (m_CurrentTarget)
			{
				if (scannedTarget)
					m_LastTargetVisibleTime = now;
				if (VERBOSE_LOGS)
				{
					float dist = vector.Distance(m_Bot.GetPosition(), m_CurrentTarget.GetPosition());
					StageLog("TARGET_SCAN_HIT", "dist=" + dist.ToString() + " nextScanInMs=" + nextScanMs.ToString(), true);
				}
			}
			else
			{
				if (VERBOSE_LOGS)
					StageLog("TARGET_SCAN_MISS", "detectRadius=" + detectRadius.ToString() + " nextScanInMs=" + nextScanMs.ToString());
			}
			return m_CurrentTarget;
		}

		protected bool IsTargetValid(PlayerBase target, float detectRadius)
		{
			if (!IsCandidateHumanTarget(target))
			{
				if (VERBOSE_LOGS)
					StageLog("TARGET_INVALID", "reason=null_or_dead_or_self");
				return false;
			}

			float distSq = vector.DistanceSq(m_Bot.GetPosition(), target.GetPosition());
			bool inside = distSq <= detectRadius * detectRadius;
			if (!inside)
			{
				if (VERBOSE_LOGS)
					StageLog("TARGET_INVALID", "reason=out_of_detect_radius dist=" + Math.Sqrt(distSq).ToString() + " detectRadius=" + detectRadius.ToString());
				return false;
			}

			if (!HasLineOfSightToTarget(target))
			{
				int now = GetGame().GetTime();

				// In raw Expansion FSM mode, do not let the legacy BQ scanner drop a target
				// while Expansion still has active target memory/search threat for it.
				if (IsExpansionMemoryTarget(target))
				{
					SearchDiag("is_valid_keep_expansion_memory", "target=" + target.GetType());
					return true;
				}

				// Keep aggro target for a short memory window so bot can path around hard cover.
				if (target == m_CurrentTarget && m_TargetMemoryMs > 0 && m_LastTargetVisibleTime > 0)
				{
					int lostLosMs = now - m_LastTargetVisibleTime;
					if (lostLosMs >= 0 && lostLosMs <= m_TargetMemoryMs)
					{
						if (VERBOSE_LOGS)
							StageLog("TARGET_MEMORY", "lostLosMs=" + lostLosMs.ToString() + " memoryMs=" + m_TargetMemoryMs.ToString());
						return true;
					}
				}

				// Expansion-like search behavior: even after LOS-memory expires, keep tracking
				// current target while search timer is active and continue moving to search positions.
				if (target == m_CurrentTarget && m_TargetSearchTimeoutMs > 0)
				{
					if (!m_TargetSearchActive && m_HasLastKnownTargetPos)
						StartTargetSearch(m_LastKnownTargetPos, now);

					if (CanContinueTargetSearch(now))
					{
						SearchDiag("is_valid_keep_search", "target=" + target.GetType() + " ageMs=" + (now - m_TargetSearchStartTime).ToString());
						if (VERBOSE_LOGS)
						{
							int searchAgeMs = now - m_TargetSearchStartTime;
							StageLog("TARGET_SEARCH_KEEP", "ageMs=" + searchAgeMs.ToString() + " timeoutMs=" + m_TargetSearchTimeoutMs.ToString());
						}
						return true;
					}
				}

				// Do not drop already tracked target only because global LOS budget was temporarily exhausted.
				// This prevents visible "part of bots stopped shooting" while keeping LOS checks throttled.
				if (target == m_CurrentTarget && m_LastLosBudgetDeferred)
					return true;

				if (VERBOSE_LOGS)
					StageLog("TARGET_INVALID", "reason=occluded_by_object");
				SearchDiag("is_valid_drop", "reason=occluded target=" + target.GetType());
				return false;
			}

			m_LastTargetVisibleTime = GetGame().GetTime();
			return inside;
		}

		protected bool HasLineOfSightToTarget(PlayerBase target)
		{
			m_LastLosBudgetDeferred = false;

			if (!m_Bot || !target)
			{
				LosDebug("check skipped: invalid bot/target refs", true);
				return false;
			}

			int now = GetGame().GetTime();
			if (m_LastLosResultKnown && m_LastLosTarget == target && now < m_NextLosCheckTime)
			{
				PerfCountLosCacheHit();
			// bqBots DIAG disabled
				return m_LastLosResult;
			}

			if (!TryConsumeLosBudget(now))
			{
				m_LastLosBudgetDeferred = true;
				if (m_LastLosResultKnown && m_LastLosTarget == target)
					return m_LastLosResult;
				return false;
			}

			PerfCountLosCheck();

			vector from = GetExpansionLosStartPoint(m_Bot);
			vector aimOffset = GetExpansionTargetAimOffset(target);
			vector targetAimPos = target.GetPosition() + aimOffset;
			// bqBots DIAG disabled

			vector dir = vector.Direction(from, targetAimPos);
			float targetDist = dir.Length();
			if (targetDist <= 0.001)
			{
			// bqBots DIAG disabled
				ReportLosResult(target, true, "self", 0.0, from, targetAimPos, false);
				return true;
			}

			if (!IsInsideDynamicFov(target))
			{
			// bqBots DIAG disabled
				ReportLosResult(target, false, "outside_fov", targetDist, from, targetAimPos, true);
				return false;
			}

			dir.Normalize();
			vector to = from + dir * targetDist + dir * 0.5;
			float dist = vector.Distance(from, to);

			vector contactPos;
			vector contactDir;
			int contactComponent;
			set<Object> results = new set<Object>;
			float radius = 0.05;
			// bqBots DIAG disabled
			
			bool rayHit = DayZPhysics.RaycastRV(from, to, contactPos, contactDir, contactComponent, results, null, m_Bot, false, false, ObjIntersectView, radius);
			// bqBots DIAG disabled
			
			if (!rayHit || results.Count() == 0)
			{
			// bqBots DIAG disabled
				ReportLosResult(target, false, "no_view_hit", dist, from, to, true);
				return false;
			}

			if (results[0] && results[0].IsBuilding())
			{
				results.Clear();
				DayZPhysics.RaycastRV(contactPos - dir * 0.1, to, contactPos, contactDir, contactComponent, results, null, m_Bot, false, false, ObjIntersectFire, radius);
			}

			float contactToTargetDistSq = vector.DistanceSq(contactPos, targetAimPos);
			
			Object hitObj = null;
			foreach (Object obj : results)
			{
				if (!obj)
					continue;

				hitObj = obj;
				Object testObj = obj;
				EntityAI hitEntity = EntityAI.Cast(obj);
				if (hitEntity)
				{
					EntityAI root = hitEntity.GetHierarchyRoot();
					if (root)
						testObj = root;
				}

				if (testObj == target || obj == target)
				{
					ReportLosResult(target, true, "target", dist, from, to, false);
					return true;
				}

				PlayerBase hitPlayer = ResolveHitPlayer(testObj);
				if (hitPlayer == target)
				{
					ReportLosResult(target, true, "target_player", dist, from, to, false);
					return true;
				}

				if (contactToTargetDistSq <= 0.04)
				{
					ReportLosResult(target, true, "near_target", dist, from, to, false);
					return true;
				}

				if (testObj.IsTree() || testObj.IsBush())
				{
					float targetDistSq = targetDist * targetDist;
					vector objPos = testObj.GetPosition();
					objPos[1] = contactPos[1];
					float objDistSq = vector.DistanceSq(from, objPos);
			// bqBots DIAG disabled
					if (targetDistSq < objDistSq || contactToTargetDistSq <= 16.0)
					{
			// bqBots DIAG disabled
						ReportLosResult(target, true, "foliage", dist, from, to, false);
						return true;
					}
				}

				PerfCountLosBlocked();
				string blockerType = testObj.GetType();
			// bqBots DIAG disabled
				ReportLosResult(target, false, blockerType, dist, from, to, true);
				return false;
			}
			// bqBots DIAG disabled
			ReportLosResult(target, false, "empty_results", dist, from, to, true);
			return false;
		}

		protected vector GetExpansionLosStartPoint(PlayerBase unit)
		{
			if (!unit)
				return "0 0 0";

			string boneName = "Head";
			if (unit.IsRaised())
				boneName = "Neck";

			vector p;
			if (TryGetUnitBonePoint(unit, boneName, p))
				return p;
			if (TryGetUnitBonePoint(unit, "neck", p) || TryGetUnitBonePoint(unit, "Neck", p))
				return p;
			if (TryGetUnitBonePoint(unit, "head", p) || TryGetUnitBonePoint(unit, "Head", p))
				return p;

			p = unit.GetPosition();
			p[1] = p[1] + 1.6;
			return p;
		}

		protected vector GetExpansionTargetAimOffset(PlayerBase target)
		{
			vector p;
			string boneName = "neck";
			Weapon_Base weapon = Weapon_Base.Cast(m_Bot.GetHumanInventory().GetEntityInHands());
			if (weapon && weapon.ShootsExplosiveAmmo())
				boneName = "spine3";

			if (!TryGetUnitBonePoint(target, boneName, p))
			{
				if (!TryGetUnitBonePoint(target, "Neck", p) && !TryGetUnitBonePoint(target, "neck", p))
				{
					p = target.GetPosition();
					p[1] = p[1] + 1.45;
				}
			}

			return p - target.GetPosition();
		}
		protected bool TryGetUnitBonePoint(PlayerBase unit, string boneName, out vector bonePos)
		{
			bonePos = "0 0 0";
			if (!unit)
				return false;

			int boneIndex = unit.GetBoneIndexByName(boneName);
			if (boneIndex < 0)
				return false;

			bonePos = unit.GetBonePositionWS(boneIndex);
			return bonePos != "0 0 0";
		}

		protected vector GetVisionPoint(PlayerBase unit)
		{
			if (!unit)
				return "0 0 0";

			vector p;
			if (TryGetUnitBonePoint(unit, "head", p) || TryGetUnitBonePoint(unit, "Head", p))
			{
				p[1] = p[1] + 0.03;
				return p;
			}

			if (TryGetUnitBonePoint(unit, "neck", p) || TryGetUnitBonePoint(unit, "Neck", p))
			{
				p[1] = p[1] + 0.10;
				return p;
			}

			p = unit.GetPosition();
			p[1] = p[1] + 1.60;
			return p;
		}
		protected PlayerBase ResolveHitPlayer(Object obj)
		{
			if (!obj)
				return null;

			PlayerBase player = PlayerBase.Cast(obj);
			if (player)
				return player;

			EntityAI ent = EntityAI.Cast(obj);
			if (!ent)
				return null;

			Man root = ent.GetHierarchyRootPlayer();
			if (!root)
				return null;

			return PlayerBase.Cast(root);
		}

		protected float Dot3(vector a, vector b)
		{
			return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
		}

		protected int BQBot_ModInt(int value, int divisor)
		{
			if (divisor == 0)
				return 0;

			int quotient = value / divisor;
			return value - quotient * divisor;
		}


		// Legacy manager-level local obstacle avoidance removed.
		// Movement ownership is in BQBotCommandMove (4_World), matching Expansion architecture.


		protected bool HasFriendlyBotNearFireSegment(vector from, vector to, PlayerBase target)
		{
			vector line = to - from;
			float lineLenSq = Dot3(line, line);
			if (lineLenSq <= 0.0001)
				return false;

			float radiusSq = FRIENDLY_FIRELINE_FALLBACK_RADIUS_M * FRIENDLY_FIRELINE_FALLBACK_RADIUS_M;
			array<Man> players = GetPlayersSnapshot();
			foreach (Man man : players)
			{
				PlayerBase botCandidate = PlayerBase.Cast(man);
				if (!botCandidate || botCandidate == m_Bot || botCandidate == target || !botCandidate.IsAlive() || !botCandidate.BQBot_IsBot())
					continue;

				vector p = GetVisionPoint(botCandidate);
				vector rel = p - from;
				float t = Dot3(rel, line) / lineLenSq;
				if (t <= 0.02 || t >= 0.98)
					continue;

				vector closest;
				closest[0] = from[0] + line[0] * t;
				closest[1] = from[1] + line[1] * t;
				closest[2] = from[2] + line[2] * t;

				vector d = p - closest;
				float distSq = Dot3(d, d);
				if (distSq <= radiusSq)
				{
					FriendlyFireDiag("fallback blocked by bot=" + botCandidate.GetType() + " t=" + t.ToString() + " distSq=" + distSq.ToString(), true);
					return true;
				}
			}

			return false;
		}

		protected bool HasFriendlyBotInFireCone(vector from, vector to, PlayerBase target)
		{
			vector line = to - from;
			float lineLenSq = Dot3(line, line);
			if (lineLenSq <= 0.0001)
				return false;

			float minCosSq = FRIENDLY_FIRE_CONE_MIN_COS * FRIENDLY_FIRE_CONE_MIN_COS;
			float maxDistSq = lineLenSq * 1.05;
			array<Man> players = GetPlayersSnapshot();
			foreach (Man man : players)
			{
				PlayerBase botCandidate = PlayerBase.Cast(man);
				if (!botCandidate || botCandidate == m_Bot || botCandidate == target || !botCandidate.IsAlive() || !botCandidate.BQBot_IsBot())
					continue;

				vector p = GetVisionPoint(botCandidate);
				vector rel = p - from;
				float relLenSq = Dot3(rel, rel);
				if (relLenSq <= 0.04 || relLenSq >= maxDistSq)
					continue;

				float dot = Dot3(rel, line);
				if (dot <= 0.0)
					continue;

				float cosSq = (dot * dot) / (relLenSq * lineLenSq);
				if (cosSq >= minCosSq)
				{
					FriendlyFireDiag("cone blocked by bot=" + botCandidate.GetType() + " cosSq=" + cosSq.ToString(), true);
					return true;
				}
			}

			return false;
		}

		protected bool HasFriendlyBotInFireLine(PlayerBase target, vector targetAimPos)
		{
			// Friendly-fire line raycast checks are intentionally disabled.
			// Bot-vs-bot damage is blocked in damage pipeline (EEOnDamageCalculated),
			// so this extra per-shot raycast path is unnecessary overhead.
			if (!FRIENDLY_FIRELINE_RAYCAST_ENABLED)
				return false;

			if (!m_Bot || !m_BotWeapon || !target)
				return false;

			int now = GetGame().GetTime();
			if (m_LastFriendlyFirelineKnown && m_LastFriendlyFirelineTarget == target && now < m_NextFriendlyFirelineCheckTime)
			{
				FriendlyFireDiag("cache target=" + target.GetType() + " blocked=" + BoolTo01(m_LastFriendlyFirelineBlocked));
				return m_LastFriendlyFirelineBlocked;
			}

			vector from = m_BotWeapon.GetPosition();

			vector to = targetAimPos;
			FriendlyFireDiag("ray begin from=" + from.ToString() + " to=" + to.ToString() + " target=" + target.GetType(), true);

			RaycastRVParams rayInput = new RaycastRVParams(from, to, m_Bot, 0.03);
			rayInput.flags = CollisionFlags.ALLOBJECTS;
			rayInput.type = ObjIntersectView;
			rayInput.sorted = true;

			array<ref RaycastRVResult> hits = new array<ref RaycastRVResult>();
			array<Object> excluded = { m_Bot };
			bool blockedByFriendly = false;

			if (DayZPhysics.RaycastRVProxy(rayInput, hits, excluded) && hits.Count() > 0)
			{
				FriendlyFireDiag("ray hits count=" + hits.Count().ToString(), true);
				foreach (RaycastRVResult hit : hits)
				{
					if (!hit)
						continue;

					Object blocker = hit.obj;
					if (!blocker)
						blocker = hit.parent;
					if (!blocker)
						continue;

					if (blocker == target)
						break;

					PlayerBase hitPlayer = ResolveHitPlayer(blocker);
					if (hitPlayer)
					{
						FriendlyFireDiag("hit player=" + hitPlayer.GetType() + " isBot=" + BoolTo01(hitPlayer.BQBot_IsBot()), true);
						if (hitPlayer == target)
							break;

						if (hitPlayer != m_Bot && hitPlayer.BQBot_IsBot())
						{
							blockedByFriendly = true;
							FriendlyFireDiag("blocked by friendly bot type=" + hitPlayer.GetType(), true);
							break;
						}

						continue;
					}

					FriendlyFireDiag("hit object=" + blocker.GetType() + " obstruct=" + BoolTo01(blocker.CanObstruct()), true);
					if (blocker.CanObstruct())
						break;
				}
			}
			else
			{
				FriendlyFireDiag("ray no hits", true);
			}

			if (!blockedByFriendly && HasFriendlyBotNearFireSegment(from, to, target))
				blockedByFriendly = true;
			if (!blockedByFriendly && HasFriendlyBotInFireCone(from, to, target))
				blockedByFriendly = true;

			m_LastFriendlyFirelineKnown = true;
			m_LastFriendlyFirelineBlocked = blockedByFriendly;
			m_LastFriendlyFirelineTarget = target;
			m_NextFriendlyFirelineCheckTime = now + FRIENDLY_FIRELINE_CHECK_INTERVAL_MS;
			FriendlyFireDiag("ray result blocked=" + BoolTo01(blockedByFriendly), true);
			return blockedByFriendly;
		}

		protected void RefreshWeaponStateFromHands()
		{
			if (!m_Bot)
				return;

			EntityAI hands = m_Bot.GetEntityInHands();
			if (hands == m_LastHandsEntity)
				return;

			m_LastHandsEntity = hands;
			m_BotWeapon = Weapon_Base.Cast(hands);
			if (!m_BotWeapon)
			{
				VerboseLog("weapon changed: no valid weapon in hands");
				return;
			}

			EnsureAutoFireMode(true);
		}

	protected void EnableCombatPose()
		{
			if (m_CombatPoseActive)
			{
				return;
			}

			m_CombatPoseActive = true;
			m_Bot.BQBot_SetCombatPoseWanted(true);
			
			StageLog("POSE_ON", "wantRaised=" + BoolTo01(m_Bot.BQBot_IsCombatPoseWanted()), true);
			PoseDebug("combat pose enabled (command-handler one-frame raise)", true);
		}

		protected void MaintainCombatPose()
	{
			if (!m_CombatPoseActive)
			{
				return;
			}

			// BQBot Phase 1: single pose source ------------------------------------------------------------------- raise is applied via OverrideRaise(ONE_FRAME)
			// inside BQBotCommandMove.PreAnimUpdate. ForceStance(RAISEDERECT) here would
			// conflict with the HIC override and cause stance-blend jitter.

			StageLog("POSE_TICK", "raised=" + BoolTo01(m_Bot.IsRaised()) + " raiseDone=" + BoolTo01(m_Bot.IsWeaponRaiseCompleted()));
			
			PoseDebug("maintain pose tick");
		}

		protected void DisableCombatPose()
		{
			if (!m_Bot)
				return;

			m_Bot.BQBot_ClearAimHeading();
			m_Bot.BQBot_SetCombatPoseWanted(false);

			if (!m_CombatPoseActive)
				return;

			m_CombatPoseActive = false;
			StageLog("POSE_OFF", "wantRaised=" + BoolTo01(m_Bot.BQBot_IsCombatPoseWanted()), true);
			PoseDebug("combat pose disabled", true);
		}

		protected void PoseDebug(string tag, bool force = false)
		{
			if (!VERBOSE_LOGS)
				return;

			if (!m_Bot)
				return;

			int now = GetGame().GetTime();
			if (!force && now < m_NextPoseDebugTime)
				return;

			m_NextPoseDebugTime = now + POSE_DEBUG_INTERVAL_MS;
			bool inRaisedErect = m_Bot.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDERECT);
			string msg = "pose: " + tag;
			msg = msg + " active=" + BoolTo01(m_CombatPoseActive);
			msg = msg + " wantRaised=" + BoolTo01(m_Bot.BQBot_IsCombatPoseWanted());
			msg = msg + " raised=" + BoolTo01(m_Bot.IsRaised());
			msg = msg + " raiseDone=" + BoolTo01(m_Bot.IsWeaponRaiseCompleted());
			msg = msg + " lift=" + BoolTo01(m_Bot.IsLiftWeapon());
			msg = msg + " inRaisedErect=" + BoolTo01(inRaisedErect);
			VerboseLog(msg);
		}

		protected void ForceStandPose()
		{
			if (!m_Bot)
				return;

			HumanCommandMove cm = m_Bot.GetCommand_Move();
			if (!cm)
			{
				m_Bot.StartCommand_Move();
				cm = m_Bot.GetCommand_Move();
			}

			if (!cm)
			{
				DebugLog("cannot get move command for stance control");
				return;
			}

			// BQBot Phase 1: stance transitions flow through command handler; avoid ForceStance here
			// to keep pose control centralized in BQBotCommandMove. Leaving ForceStanceUp for the
			// prone-------------------------------------------------------------------------------erect base stance only, but no combat raise here.
			cm.ForceStanceUp(DayZPlayerConstants.STANCEIDX_ERECT);
		}

		protected void ResetTacticalMoveState()
		{
			m_CoverMoveActive = false;
			m_CoverPosition = "0 0 0";
			m_CurrentCoverObject = null;
			m_IsInCover = false;
			if (m_PotentialCoverObjects)
				m_PotentialCoverObjects.Clear();
			m_NextCoverCheckTime = 0;
			ResetFlankState();
			m_PositioningMoveActive = false;
			m_PositioningMoveGoal = "0 0 0";
			m_PositioningMoveAnchorTargetPos = "0 0 0";
			m_PositioningMoveExpireTime = 0;
			m_GroupShouldTakeCover = false;
			m_GroupCombatRole = GROUP_COMBAT_ROLE_ANCHOR;
			m_LastTacticalGoalReason = "none";
			m_BreachFollowGoalCache = "0 0 0";
			m_BreachFollowAnchorCache = "0 0 0";
			m_BreachFollowGoalCacheUntilTime = 0;
			ResetStableMoveGoal();
		}

		protected void ClearTransientChaseDetourState(string reason)
		{
			m_CoverMoveActive = false;
			m_CoverPosition = "0 0 0";
			m_CurrentCoverObject = null;
			m_IsInCover = false;
			if (m_PotentialCoverObjects)
				m_PotentialCoverObjects.Clear();
			m_NextCoverCheckTime = 0;
			ResetFlankState();
			m_PositioningMoveActive = false;
			m_PositioningMoveGoal = "0 0 0";
			m_PositioningMoveAnchorTargetPos = "0 0 0";
			m_PositioningMoveExpireTime = 0;
			m_LastTacticalGoalReason = reason;
			ResetStableMoveGoal();
		}

		protected void SyncTargetSearchStateLastKnownFromMemory()
		{
			if (m_HasLastKnownTargetPos)
				m_TargetSearchLastKnownPosition = m_LastKnownTargetPos;
			else
				m_TargetSearchLastKnownPosition = "0 0 0";
		}

		protected void ResetTargetSearchState()
		{
			m_TargetSearchActive = false;
			m_TargetSearchPosition = "0 0 0";
			m_TargetSearchDirection = "0 0 1";
			m_TargetSearchPositionUpdateCount = 0;
			m_TargetSearchPositionUpdateTime = 0;
			m_TargetSearchStartTime = 0;
			m_TargetSearchOnLOSLost = false;
			SyncTargetSearchStateLastKnownFromMemory();
			m_TargetMemoryState = null;
			SearchPacketDiag("packet_reset", "", true);
		}

		protected void ResetNoLosGoalState()
		{
			m_NoLosChaseGoal = "0 0 0";
			m_NoLosChaseGoalUntilTime = 0;
			m_NoLosChaseGoalSource = "";
		}

		protected float GetPlanarDirectionDot(vector fromPos, vector goalA, vector goalB)
		{
			vector a = goalA - fromPos;
			a[1] = 0.0;
			vector b = goalB - fromPos;
			b[1] = 0.0;

			float aLenSq = a[0] * a[0] + a[2] * a[2];
			float bLenSq = b[0] * b[0] + b[2] * b[2];
			if (aLenSq <= 0.0001 || bLenSq <= 0.0001)
				return 1.0;

			float dot = Dot3(a, b);
			float invLen = 1.0 / Math.Sqrt(aLenSq * bLenSq);
			return dot * invLen;
		}

		protected vector ProjectToNavigablePosition(vector rawPos, float sampleRadius = 2.5)
		{
			vector projected = rawPos;
			if (m_ChaseAIWorld && m_ChaseSampleFilter)
			{
				vector sampled = rawPos;
				if (m_ChaseAIWorld.SampleNavmeshPosition(sampled, sampleRadius, m_ChaseSampleFilter, sampled))
					projected = sampled;
			}

			projected[1] = GetGame().SurfaceY(projected[0], projected[2]);
			return projected;
		}

		vector BQBot_ProjectTargetMemoryPosition(vector rawPos, float sampleRadius)
		{
			return ProjectToNavigablePosition(rawPos, sampleRadius);
		}

		vector BQBot_GetTargetMemoryAIPosition()
		{
			if (m_Bot)
				return m_Bot.GetPosition();

			return "0 0 0";
		}

		bool BQBot_IsTargetMemoryFlankActive()
		{
			return m_FlankActive;
		}

		bool BQBot_GetGroupTargetMemoryUpdateFlag()
		{
			return GetGroupSearchUpdateFlag();
		}

		void BQBot_SetGroupTargetMemoryUpdateFlag(bool value)
		{
			SetGroupSearchUpdateFlag(value);
		}

		float BQBot_CalculateTargetMemoryThreat(PlayerBase target, vector searchPosition, bool hasLos, int now)
		{
			float dist = 0.0;
			if (m_Bot)
				dist = vector.Distance(m_Bot.GetPosition(), searchPosition);
			return CalculateThreatLevel(target, dist, hasLos, now);
		}

		protected BQBotTargetMemoryState GetOrCreateTargetMemoryState(PlayerBase target)
		{
			if (!target)
				return null;

			if (!m_TargetMemoryState)
			{
				m_TargetMemoryState = new BQBotTargetMemoryState(this, target, true);
			}
			else
			{
				m_TargetMemoryState.SetTarget(target);
			}

			return m_TargetMemoryState;
		}

		protected vector GetTargetMemoryPosition(PlayerBase target, bool hasLos)
		{
			if (!target)
				return "0 0 0";

			BQBotTargetMemoryState state = GetOrCreateTargetMemoryState(target);
			if (!state)
				return target.GetPosition();

			state.SetLOS(hasLos);
			state.UpdatePosition(false);
			SyncLegacyTargetMemoryFromState(state);
			return state.GetPosition(false);
		}

		protected void SyncLegacyTargetMemoryFromState(BQBotTargetMemoryState state)
		{
			if (!state)
				return;

			m_LastKnownTargetPos = state.m_LastKnownPosition;
			m_HasLastKnownTargetPos = m_LastKnownTargetPos != "0 0 0";
			m_TargetSearchActive = m_HasLastKnownTargetPos;
			m_TargetSearchPosition = state.m_SearchPosition;
			m_TargetSearchDirection = state.m_SearchDirection;
			m_TargetSearchPositionUpdateCount = state.m_SearchPositionUpdateCount;
			m_TargetSearchPositionUpdateTime = state.m_SearchPositionUpdateTimestamp;
			if (m_TargetSearchStartTime <= 0 || m_TargetSearchPositionUpdateCount == 0)
				m_TargetSearchStartTime = state.m_SearchPositionUpdateTimestamp;
			m_TargetSearchOnLOSLost = state.m_SearchOnLOSLost;
			SyncTargetSearchStateLastKnownFromMemory();
		}

		protected void StartTargetSearch(vector seedPos, int now)
		{
			if (!m_Bot)
				return;

			m_TargetSearchActive = true;
			m_TargetSearchStartTime = now;
			m_TargetSearchPositionUpdateTime = now;
			m_TargetSearchPositionUpdateCount = 0;
			m_TargetSearchPosition = ProjectToNavigablePosition(seedPos, 3.0);

			vector dir = m_TargetSearchPosition - m_Bot.GetPosition();
			dir[1] = 0.0;
			if ((dir[0] * dir[0] + dir[2] * dir[2]) < 0.0001)
				dir = m_Bot.GetDirection();
			dir[1] = 0.0;
			m_TargetSearchDirection = dir;
			m_TargetSearchOnLOSLost = true;
			SyncTargetSearchStateLastKnownFromMemory();
			SetGroupSearchUpdateFlag(false);

			SearchPacketDiag("packet_start", "seed=" + seedPos.ToString() + " projected=" + m_TargetSearchPosition.ToString(), true);
		}

		protected bool CanContinueTargetSearch(int now)
		{
			if (!m_TargetSearchActive)
			{
				SearchPacketDiag("packet_stop", "reason=inactive", true);
				return false;
			}

			if (m_TargetSearchTimeoutMs <= 0)
			{
				SearchPacketDiag("packet_stop", "reason=timeout_disabled timeoutMs=" + m_TargetSearchTimeoutMs.ToString(), true);
				return false;
			}

			if (m_TargetSearchPosition == "0 0 0")
			{
				SearchPacketDiag("packet_stop", "reason=zero_search_pos", true);
				return false;
			}

			if (m_TargetSearchStartTime <= 0)
			{
				SearchPacketDiag("packet_stop", "reason=start_time_invalid", true);
				return false;
			}

			int ageMs = now - m_TargetSearchStartTime;
			if (ageMs < 0)
			{
				SearchPacketDiag("packet_stop", "reason=negative_age ageMs=" + ageMs.ToString(), true);
				return false;
			}

			if (ageMs > m_TargetSearchTimeoutMs)
			{
				SearchPacketDiag("packet_stop", "reason=timeout_expired ageMs=" + ageMs.ToString() + " timeoutMs=" + m_TargetSearchTimeoutMs.ToString(), true);
				return false;
			}

			return true;
		}

		protected void UpdateTargetSearchPosition(int now)
		{
			if (!m_TargetSearchActive || !m_Bot)
				return;

			if (!CanContinueTargetSearch(now))
			{
				SearchPacketDiag("packet_update_abort", "reason=cannot_continue", true);
				ResetTargetSearchState();
				return;
			}

			if (m_FlankActive)
			{
				SearchPacketDiag("packet_update_hold", "reason=flank_active");
				return;
			}

			int sinceUpdate = now - m_TargetSearchPositionUpdateTime;
			int minUpdateMs = m_TargetSearchUpdateIntervalMs;
			if (minUpdateMs < 450)
				minUpdateMs = 450;
			if (sinceUpdate < minUpdateMs)
				return;

			vector botPos = m_Bot.GetPosition();
			float searchDistSq = vector.DistanceSq(botPos, m_TargetSearchPosition);
			float reachM = m_TargetSearchReachM;
			if (reachM < 1.5)
				reachM = 1.5;
			float searchReachSq = reachM * reachM;
			bool groupWantsSearchUpdate = GetGroupSearchUpdateFlag();
			bool shouldUpdate = false;
			bool firstGroupStep = groupWantsSearchUpdate && m_TargetSearchPositionUpdateCount == 0;
			bool groupTimeout = groupWantsSearchUpdate && sinceUpdate > 10000;
			bool reachedSearchPoint = searchDistSq <= searchReachSq;
			if (firstGroupStep || groupTimeout || reachedSearchPoint)
				shouldUpdate = true;
			if (!shouldUpdate)
				return;

			m_TargetSearchPositionUpdateCount++;
			m_TargetSearchPositionUpdateTime = now;

			vector searchDir = m_TargetSearchDirection;
			searchDir[1] = 0.0;
			float searchDirLenSq = searchDir[0] * searchDir[0] + searchDir[2] * searchDir[2];
			if (searchDirLenSq < 0.0001)
			{
				searchDir = m_TargetSearchPosition - botPos;
				searchDir[1] = 0.0;
				searchDirLenSq = searchDir[0] * searchDir[0] + searchDir[2] * searchDir[2];
			}
			if (searchDirLenSq < 0.0001)
			{
				searchDir = m_Bot.GetDirection();
				searchDir[1] = 0.0;
				searchDirLenSq = searchDir[0] * searchDir[0] + searchDir[2] * searchDir[2];
			}

			vector searchDirN = "0 0 1";
			if (searchDirLenSq >= 0.0001)
			{
				float invSearchDirLen = 1.0 / Math.Sqrt(searchDirLenSq);
				searchDirN = Vector(searchDir[0] * invSearchDirLen, 0.0, searchDir[2] * invSearchDirLen);
			}

			vector nextPos = m_TargetSearchPosition;
			if (m_TargetSearchPositionUpdateCount <= 3)
			{
				// Expansion-like directional search: advance in remembered direction
				// with small yaw randomization for natural spread.
				vector angles = searchDirN.VectorToAngles();
				angles[0] = NormalizeYawAbs(NormalizeYawDelta(angles[0] + Math.RandomFloat(-33.75, 33.75)));
				vector dir = angles.AnglesToVector();
				float stepDist = Math.RandomFloat(5.0, 10.0);
				nextPos = m_TargetSearchPosition + dir * stepDist;
			}
			else
			{
				// Expansion-like radial search after directional passes.
				float randomAngle = Math.RandomFloat(0.0, 360.0) * Math.DEG2RAD;
				float randomRadius = 30.0 * Math.Sqrt(Math.RandomFloat(0.0, 1.0));
				vector radialOffset = Vector(Math.Sin(randomAngle), 0.0, Math.Cos(randomAngle)) * randomRadius;
				vector radialCenter = m_TargetSearchPosition + searchDirN;
				nextPos = radialCenter + radialOffset;
			}

			nextPos = ProjectToNavigablePosition(nextPos, 3.0);
			if (vector.DistanceSq(nextPos, m_TargetSearchPosition) <= 1.0)
			{
				vector fallbackStep = m_TargetSearchPosition + searchDirN * 4.0;
				nextPos = ProjectToNavigablePosition(fallbackStep, 2.5);
			}

			vector nextSearchDir = nextPos - m_TargetSearchPosition;
			nextSearchDir[1] = 0.0;
			float nextSearchDirLenSq = nextSearchDir[0] * nextSearchDir[0] + nextSearchDir[2] * nextSearchDir[2];
			if (nextSearchDirLenSq >= 0.0001)
				m_TargetSearchDirection = nextSearchDir;
			else
				m_TargetSearchDirection = searchDirN;

			m_TargetSearchPosition = nextPos;

			SetGroupSearchUpdateFlag(true);
			SearchPacketDiag("packet_update", "next=" + m_TargetSearchPosition.ToString() + " updCount=" + m_TargetSearchPositionUpdateCount.ToString() + " grpFlag=" + BoolTo01(groupWantsSearchUpdate), true);
		}
		protected float GetDynamicFovHalfAngle(float distanceToTarget)
		{
			if (distanceToTarget <= m_FOVNearDistThreshold)
				return m_FOVNearHalfAngleH;
			if (distanceToTarget >= m_FOVFarDistThreshold)
				return m_FOVFarHalfAngleH;

			float t = (distanceToTarget - m_FOVNearDistThreshold) / (m_FOVFarDistThreshold - m_FOVNearDistThreshold);
			return m_FOVNearHalfAngleH + (m_FOVFarHalfAngleH - m_FOVNearHalfAngleH) * t;
		}

		protected bool IsInsideDynamicFov(PlayerBase target)
		{
			if (!m_Bot || !target)
				return false;

			vector toTarget = target.GetPosition() - m_Bot.GetPosition();
			toTarget[1] = 0.0;
			float distSq = toTarget[0] * toTarget[0] + toTarget[2] * toTarget[2];
			if (distSq <= 9.0)
			{
			// bqBots DIAG disabled
				return true;
			}

			float dist = Math.Sqrt(distSq);
			float invDist = 1.0 / dist;
			toTarget[0] = toTarget[0] * invDist;
			toTarget[2] = toTarget[2] * invDist;

			vector forward = m_Bot.GetDirection();
			forward[1] = 0.0;
			float fwdLenSq = forward[0] * forward[0] + forward[2] * forward[2];
			if (fwdLenSq < 0.0001)
			{
			// bqBots DIAG disabled
				return true;
			}
			float invFwdLen = 1.0 / Math.Sqrt(fwdLenSq);
			forward[0] = forward[0] * invFwdLen;
			forward[2] = forward[2] * invFwdLen;

			float halfAngle = GetDynamicFovHalfAngle(dist);
			float minCos = Math.Cos(halfAngle * Math.DEG2RAD);
			float dot = forward[0] * toTarget[0] + forward[2] * toTarget[2];
			bool result = dot >= minCos;
			// bqBots DIAG disabled
			return result;
		}

		protected float GetTargetWeaponThreat(PlayerBase target)
		{
			if (!target)
				return 0.0;

			EntityAI inHands = target.GetEntityInHands();
			if (!inHands)
				return -0.05;

			if (Weapon_Base.Cast(inHands))
			{
				if (target.IsRaised())
					return 0.18;
				return 0.10;
			}

			if (inHands.IsKindOf("MeleeWeapon"))
				return 0.05;

			return 0.02;
		}

		protected float CalculateThreatLevel(PlayerBase target, float distanceToTarget, bool hasLos, int now)
		{
			float detectRadius = GetEffectiveDetectRadius();
			if (detectRadius < 1.0)
				detectRadius = 1.0;

			float distanceThreat = 1.0 - Math.Clamp(distanceToTarget / detectRadius, 0.0, 1.0);
			float losThreat = -0.15;
			if (hasLos)
				losThreat = 0.22;

			float movementThreat = 0.0;
			if (target && m_Bot)
			{
				vector threatTargetPos = target.GetPosition();
				if (!hasLos && m_TargetMemoryState)
					threatTargetPos = m_TargetMemoryState.GetPosition(false);
				vector toSelf = m_Bot.GetPosition() - threatTargetPos;
				toSelf[1] = 0.0;
				float toSelfLenSq = toSelf[0] * toSelf[0] + toSelf[2] * toSelf[2];
				if (toSelfLenSq > 0.0001)
				{
					float invToSelfLen = 1.0 / Math.Sqrt(toSelfLenSq);
					toSelf[0] = toSelf[0] * invToSelfLen;
					toSelf[2] = toSelf[2] * invToSelfLen;

					vector targetDir = target.GetDirection();
					targetDir[1] = 0.0;
					float targetDirLenSq = targetDir[0] * targetDir[0] + targetDir[2] * targetDir[2];
					if (targetDirLenSq > 0.0001)
					{
						float invTargetDirLen = 1.0 / Math.Sqrt(targetDirLenSq);
						targetDir[0] = targetDir[0] * invTargetDirLen;
						targetDir[2] = targetDir[2] * invTargetDirLen;
						float approachDot = targetDir[0] * toSelf[0] + targetDir[2] * toSelf[2];
						if (approachDot > 0.35)
							movementThreat = 0.10;
						else if (approachDot < -0.35)
							movementThreat = -0.06;
					}
				}
			}

			float visibilityPenalty = (1.0 - GetVisibilityMultiplier()) * 0.15;
			float weaponThreat = GetTargetWeaponThreat(target);
			float threat = distanceThreat + losThreat + weaponThreat + movementThreat - visibilityPenalty;
			return Math.Clamp(threat, 0.0, 1.0);
		}

		protected int GetCoverObjectPriority(Object obj)
		{
			if (!obj)
				return 0;

			if (obj.IsKindOf("House") || obj.IsKindOf("Building"))
				return 4;
			if (obj.IsKindOf("Transport") || obj.IsKindOf("CarScript"))
				return 3;
			if (obj.IsKindOf("RockBase"))
				return 2;
			if (obj.IsKindOf("TreeHard") || obj.IsKindOf("TreeSoft") || obj.IsKindOf("Tree"))
				return 1;

			string typeName = obj.GetType();
			if (typeName != "")
			{
				if (typeName.IndexOf("Rock") >= 0)
					return 2;
				if (typeName.IndexOf("Tree") >= 0)
					return 1;
			}

			return 0;
		}

		protected bool IsPotentialCoverObject(Object obj)
		{
			return GetCoverObjectPriority(obj) > 0;
		}

		protected Object GetFirstCoverBlocker(vector fromEye, vector threatEye)
		{
			RaycastRVParams rayInput = new RaycastRVParams(fromEye, threatEye, m_Bot, 0.05);
			rayInput.flags = CollisionFlags.ALLOBJECTS;
			rayInput.type = ObjIntersectView;
			rayInput.sorted = true;

			array<ref RaycastRVResult> hits = new array<ref RaycastRVResult>();
			array<Object> excluded = { m_Bot, m_CurrentTarget };
			if (!DayZPhysics.RaycastRVProxy(rayInput, hits, excluded) || hits.Count() == 0)
				return null;

			foreach (RaycastRVResult hit : hits)
			{
				if (!hit)
					continue;

				Object blocker = hit.obj;
				if (!blocker)
					blocker = hit.parent;
				if (!blocker)
					continue;
				if (!blocker.CanObstruct())
					continue;

				return blocker;
			}

			return null;
		}

		protected bool IsExpansionPotentialCoverObject(Object obj)
		{
			if (!obj)
				return false;

			if (obj.IsBuilding())
			{
				string typeName = obj.GetType();
				if ((typeName.IndexOf("Land_") == 0 || typeName.IndexOf("Wreck_") == 0) && typeName.IndexOf("Gate") == -1)
					return true;
			}
			else if (obj.IsRock())
			{
				return true;
			}
			else if (obj.IsTree())
			{
				return true;
			}
			else if (obj.IsTransport())
			{
				return true;
			}

			return IsPotentialCoverObject(obj);
		}

		protected void UpdatePotentialCoverObjectsExpansionStyle(vector center)
		{
			if (!m_PotentialCoverObjects)
				m_PotentialCoverObjects = new set<Object>();
			m_PotentialCoverObjects.Clear();

			array<EntityAI> candidates = new array<EntityAI>();
			vector min = Vector(center[0] - 30.0, center[1] - 30.0, center[2] - 30.0);
			vector max = Vector(center[0] + 30.0, center[1] + 30.0, center[2] + 30.0);
			DayZPlayerUtils.PhysicsGetEntitiesInBox(min, max, candidates);

			foreach (EntityAI candidate : candidates)
			{
				Object obj = Object.Cast(candidate);
				if (!obj || obj == m_Bot || obj == m_CurrentTarget)
					continue;
				if (!IsExpansionPotentialCoverObject(obj))
					continue;
				if (IsCoverObjectReservedByOther(obj))
					continue;

				m_PotentialCoverObjects.Insert(obj);
			}
		}

		protected vector GetCoverPositionBehindObject(Object coverObj, vector threatPos)
		{
			if (!coverObj)
				return "0 0 0";

			vector objPos = coverObj.GetPosition();
			vector objToThreat = threatPos - objPos;
			objToThreat[1] = 0.0;
			float lenSq = objToThreat[0] * objToThreat[0] + objToThreat[2] * objToThreat[2];
			if (lenSq < 0.0001)
				return "0 0 0";

			float invLen = 1.0 / Math.Sqrt(lenSq);
			objToThreat[0] = objToThreat[0] * invLen;
			objToThreat[2] = objToThreat[2] * invLen;

			float extension = 1.5;
			vector minMax[2];
			if (coverObj.IsTree())
			{
				extension = 2.0;
			}
			else if (coverObj.GetCollisionBox(minMax))
			{
				minMax[0][1] = 0.0;
				minMax[1][1] = 0.0;
				extension = vector.Distance(minMax[0], minMax[1]) * 0.5 + 1.0;
			}
			else
			{
				extension = coverObj.ClippingInfo(minMax);
				if (extension < 1.0)
					extension = 1.0;
			}

			vector coverPos = objPos - objToThreat * extension;
			return ProjectToNavigablePosition(coverPos, 3.5);
		}
		protected vector FindCoverPosition(vector threatPos)
		{
			if (!m_Bot)
				return "0 0 0";

			vector botPos = m_Bot.GetPosition();
			vector toThreat = threatPos - botPos;
			toThreat[1] = 0.0;
			float threatLenSq = toThreat[0] * toThreat[0] + toThreat[2] * toThreat[2];
			if (threatLenSq < 0.0001)
				return "0 0 0";
			float invThreatLen = 1.0 / Math.Sqrt(threatLenSq);
			toThreat[0] = toThreat[0] * invThreatLen;
			toThreat[2] = toThreat[2] * invThreatLen;

			UpdatePotentialCoverObjectsExpansionStyle(botPos);

			Object bestCoverObj = null;
			float bestObjDistSq = 99999999.0;
			float targetDistSq = vector.DistanceSq(botPos, threatPos);

			foreach (Object coverObj : m_PotentialCoverObjects)
			{
				if (!coverObj)
					continue;
				if (IsCoverObjectReservedByOther(coverObj))
					continue;

				vector toObj = coverObj.GetPosition() - botPos;
				toObj[1] = 0.0;
				float objDistSq = toObj[0] * toObj[0] + toObj[2] * toObj[2];
				if (objDistSq <= 0.0001)
					continue;

				float invObjDist = 1.0 / Math.Sqrt(objDistSq);
				vector toObjNorm;
				toObjNorm[0] = toObj[0] * invObjDist;
				toObjNorm[1] = 0.0;
				toObjNorm[2] = toObj[2] * invObjDist;
				float dot = toThreat[0] * toObjNorm[0] + toThreat[2] * toObjNorm[2];

				// Expansion: for normal cover, choose objects closer than the target or behind us
				// relative to target aim direction, then pick the closest suitable object.
				if (targetDistSq <= objDistSq && dot >= 0.0)
					continue;

				if (objDistSq < bestObjDistSq)
				{
					bestObjDistSq = objDistSq;
					bestCoverObj = coverObj;
				}
			}

			if (!bestCoverObj)
			{
				m_CurrentCoverObject = null;
				return "0 0 0";
			}

			vector bestCover = GetCoverPositionBehindObject(bestCoverObj, threatPos);
			if (bestCover == "0 0 0")
			{
				m_CurrentCoverObject = null;
				return "0 0 0";
			}

			m_CurrentCoverObject = bestCoverObj;
			return bestCover;
		}

		protected void UpdateCoverState()
		{
			m_IsInCover = false;
			if (!m_Bot)
				return;

			if (m_Bot.eAI_ShouldUpdatePotentialCoverObjects())
			{
				UpdatePotentialCoverObjectsExpansionStyle(m_Bot.GetPosition());
				m_Bot.eAI_ClearPotentialCoverObjectsUpdate();
			}

			if (m_CoverPosition == "0 0 0")
				return;

			float coverReachSq = 0.55 * 0.55;
			if (vector.DistanceSq(m_Bot.GetPosition(), m_CoverPosition) <= coverReachSq)
				m_IsInCover = true;
		}

		// Expansion-style group context only. Do not assign custom attacker/defender/panic roles here:
		// combat decisions are owned by the FSM states (Flank/FollowFormation/Fire/Positioning).
		protected void BQBot_UpdateGroupRoleCoordination(int now, PlayerBase target)
		{
			// Disabled: Expansion group/FSM controls movement roles, speed and cover.
		}

		protected int DetermineGroupCombatRole(BQBotShooterManager leader, int memberIndex, int memberCount, float distToTarget, float threat, bool hasLos, bool groupInCombat)
		{
			if (memberCount <= 1)
				return GROUP_COMBAT_ROLE_ANCHOR;

			if (!leader || leader == this || memberIndex <= 0)
				return GROUP_COMBAT_ROLE_ANCHOR;

			if (memberCount == 2)
				return GROUP_COMBAT_ROLE_ASSAULT;

			if (memberIndex == 1)
				return GROUP_COMBAT_ROLE_ASSAULT;
			if (memberIndex == 2)
				return GROUP_COMBAT_ROLE_SUPPORT;

			int role = GROUP_COMBAT_ROLE_ASSAULT;
			if ((BQBot_ModInt(memberIndex, 2)) == 0)
				role = GROUP_COMBAT_ROLE_SUPPORT;

			// In sustained no-LOS pursuit keep more members in assault role so
			// group keeps pressure instead of all collapsing to cover.
			if (!hasLos && groupInCombat && distToTarget >= 18.0)
			{
				if (role == GROUP_COMBAT_ROLE_SUPPORT && threat <= Math.Max(0.45, m_CoverThreatThreshold))
					role = GROUP_COMBAT_ROLE_ASSAULT;
			}

			return role;
		}

		protected bool HasOtherGroupCoverMover(array<BQBotShooterManager> members, BQBotShooterManager except, bool ignoreLeader = false)
		{
			if (!members || members.Count() <= 1)
				return false;

			BQBotShooterManager leader = members.Get(0);
			foreach (BQBotShooterManager member : members)
			{
				if (!member || member == except)
					continue;
				if (ignoreLeader && member == leader)
					continue;
				if (member.m_CoverMoveActive || member.m_IsInCover)
					return true;
			}

			return false;
		}

		protected bool IsCoverObjectReservedByOther(Object coverObj)
		{
			if (!coverObj || !s_AllManagers)
				return false;

			foreach (BQBotShooterManager manager : s_AllManagers)
			{
				if (!manager || manager == this)
					continue;
				if (!manager.m_Bot || !manager.m_Bot.IsAlive())
					continue;
				if (manager.m_CurrentCoverObject != coverObj)
					continue;
				if (manager.m_CoverMoveActive || manager.m_IsInCover)
					return true;
			}

			return false;
		}

		protected int ResolvePositioningSideSign(int now, int preferredSign = 0)
		{
			if (preferredSign != 0)
			{
				m_PositioningSideSign = preferredSign;
				return preferredSign;
			}

			if (m_PositioningSideSign == 0)
			{
				if ((BQBot_ModInt(m_ManagerOrder, 2)) == 0)
					m_PositioningSideSign = 1;
				else
					m_PositioningSideSign = -1;
				m_NextPositioningSideFlipTime = now + 3500 + Math.RandomIntInclusive(0, 2500);
			}

			if (now >= m_NextPositioningSideFlipTime)
			{
				m_PositioningSideSign = -m_PositioningSideSign;
				m_NextPositioningSideFlipTime = now + 3500 + Math.RandomIntInclusive(0, 2500);
			}

			return m_PositioningSideSign;
		}

		protected int ResolveEffectiveGroupSlot()
		{
			int slot = m_GroupSlot;
			if (slot >= 1 || m_GroupId == "")
				return slot;

			array<BQBotShooterManager> members = new array<BQBotShooterManager>();
			if (!CollectAliveGroupMembers(members))
				return slot;

			int memberIdx = members.Find(this);
			if (memberIdx >= 0)
				slot = memberIdx + 1;

			return slot;
		}

		protected vector ApplyExpansionGroupSteering(vector desiredMoveDir, vector botPos, float distToTarget, out float separationWeight, out float cohesionWeight, out int nearbyCount)
		{
			separationWeight = 0.0;
			cohesionWeight = 0.0;
			nearbyCount = 0;

			if (m_GroupId == "")
				return desiredMoveDir;

			float desiredLenSq = desiredMoveDir[0] * desiredMoveDir[0] + desiredMoveDir[2] * desiredMoveDir[2];
			if (desiredLenSq < 0.0001)
				return desiredMoveDir;
			float desiredLen = Math.Sqrt(desiredLenSq);

			array<BQBotShooterManager> members = new array<BQBotShooterManager>();
			if (!CollectAliveGroupMembers(members) || members.Count() <= 1)
				return desiredMoveDir;

			float separationRadius = 2.8;
			if (distToTarget > Math.Max(14.0, m_ChaseStopDistanceM + 8.0))
				separationRadius = 3.3;
			if (distToTarget > 30.0)
				separationRadius = 3.8;
			float separationRadiusSq = separationRadius * separationRadius;

			vector separationPush = "0 0 0";
			foreach (BQBotShooterManager member : members)
			{
				if (!member || member == this || !member.m_Bot || !member.m_Bot.IsAlive())
					continue;

				vector delta = botPos - member.m_Bot.GetPosition();
				delta[1] = 0.0;
				float deltaSq = delta[0] * delta[0] + delta[2] * delta[2];
				if (deltaSq < 0.01 || deltaSq > separationRadiusSq)
					continue;

				float deltaLen = Math.Sqrt(deltaSq);
				float weight = (separationRadius - deltaLen) / separationRadius;
				if (weight <= 0.0)
					continue;

				separationPush = separationPush + delta * (weight / deltaLen);
				nearbyCount++;
			}

			vector steer = desiredMoveDir;
			if (nearbyCount > 0)
			{
				separationPush = separationPush * (1.0 / nearbyCount);
				separationWeight = Math.Clamp(0.45 + (0.12 * nearbyCount), 0.45, 0.90);
				steer = steer + separationPush * separationWeight;
			}

			BQBotShooterManager leader = members.Get(0);
			if (leader && leader != this && leader.m_Bot && leader.m_Bot.IsAlive())
			{
				vector toLeader = leader.m_Bot.GetPosition() - botPos;
				toLeader[1] = 0.0;
				float toLeaderSq = toLeader[0] * toLeader[0] + toLeader[2] * toLeader[2];
				float regroupStart = Math.Max(6.0, m_GroupRegroupDistanceM * 0.85);
				if (toLeaderSq > regroupStart * regroupStart)
				{
					float toLeaderLen = Math.Sqrt(toLeaderSq);
					vector toLeaderN = Vector(toLeader[0] / toLeaderLen, 0.0, toLeader[2] / toLeaderLen);
					float regroupSpan = Math.Max(4.0, m_GroupBreakFormationDistanceM - m_GroupRegroupDistanceM);
					float t = (toLeaderLen - regroupStart) / regroupSpan;
					if (t < 0.0)
						t = 0.0;
					if (t > 1.0)
						t = 1.0;
					cohesionWeight = 0.35 * t;
					steer = steer + toLeaderN * cohesionWeight;
				}
			}

			float steerLenSq = steer[0] * steer[0] + steer[2] * steer[2];
			if (steerLenSq < 0.0001)
				return desiredMoveDir;
			float steerLen = Math.Sqrt(steerLenSq);
			vector steerNorm = Vector(steer[0] / steerLen, 0.0, steer[2] / steerLen);
			return steerNorm * desiredLen;
		}

		protected vector CalculateLeaderSupportPosition(BQBotShooterManager leader, vector targetPos, int memberIndex, int now)
		{
			if (!m_Bot || !leader || !leader.m_Bot)
				return "0 0 0";

			vector leaderPos = leader.m_Bot.GetPosition();
			vector forward = targetPos - leaderPos;
			forward[1] = 0.0;
			float fwdLenSq = forward[0] * forward[0] + forward[2] * forward[2];
			if (fwdLenSq < 0.0001)
				return "0 0 0";
			float invFwdLen = 1.0 / Math.Sqrt(fwdLenSq);
			forward[0] = forward[0] * invFwdLen;
			forward[2] = forward[2] * invFwdLen;

			vector right = forward.Perpend();
			right[1] = 0.0;
			int sideSign = GetGroupFlankSideSign(memberIndex);
			if (sideSign == 0)
				sideSign = ResolvePositioningSideSign(now);

			float sideDist = 2.5 + Math.Min(4.0, memberIndex * 1.3);
			float backDist = 1.8 + Math.Min(3.0, memberIndex * 0.8);
			float sideMul = 1.0;
			if (sideSign < 0)
				sideMul = -1.0;
			vector supportGoal = leaderPos + right * (sideDist * sideMul) - forward * backDist;
			return ProjectToNavigablePosition(supportGoal, 2.5);
		}

		protected bool TryGetGroupBreachFollowGoal(PlayerBase target, bool hasLos, int now, int memberIndex, out vector moveGoal)
		{
			moveGoal = "0 0 0";
			if (hasLos || !target || !m_Bot || m_GroupId == "")
				return false;

			array<BQBotShooterManager> members = new array<BQBotShooterManager>();
			if (!CollectAliveGroupMembers(members) || members.Count() <= 1)
				return false;

			vector botPos = m_Bot.GetPosition();
			vector targetPos = GetTargetMemoryPosition(target, false);
			float ownDistToTarget = vector.Distance(botPos, targetPos);

			BQBotShooterManager bestMember = null;
			float bestScore = 999999.0;
			foreach (BQBotShooterManager member : members)
			{
				if (!member || member == this || !member.m_Bot || !member.m_Bot.IsAlive())
					continue;

				int seenAge = 999999;
				if (member.m_LastTargetVisibleTime > 0)
					seenAge = now - member.m_LastTargetVisibleTime;
				if (seenAge < 0)
					seenAge = 999999;

				bool memberHasFreshLos = seenAge <= BQBOT_BREACH_FOLLOW_SEEN_AGE_MS;
				bool memberIsFiring = member.m_TacticalState == TACTICAL_STATE_FIGHTING_FIREWEAPON;
				if (!memberHasFreshLos && !memberIsFiring)
					continue;

				vector memberPos = member.m_Bot.GetPosition();
				float memberDistToTarget = vector.Distance(memberPos, targetPos);
				if (memberDistToTarget > ownDistToTarget + 10.0 && !memberIsFiring)
					continue;

				if (member.m_ChaseBlockedAccumMs > (m_ChaseStuckRepathMs * 4) && !memberIsFiring)
					continue;

				float score = memberDistToTarget + vector.Distance(botPos, memberPos) * 0.08 + member.m_ChaseBlockedAccumMs * 0.002;
				if (memberIsFiring)
					score = score - 12.0;
				if (memberHasFreshLos)
					score = score - 8.0;

				if (score < bestScore)
				{
					bestScore = score;
					bestMember = member;
				}
			}

			if (!bestMember || !bestMember.m_Bot)
				return false;

			vector anchorPos = bestMember.m_Bot.GetPosition();

			// Hysteresis: reuse the cached goal while the anchor has not drifted far.
			// Without this, the breach goal jitters every tick the squadmate moves.
			bool hasCachedBreachGoal = (m_BreachFollowGoalCache[0] != 0.0 || m_BreachFollowGoalCache[2] != 0.0);
			if (hasCachedBreachGoal && now < m_BreachFollowGoalCacheUntilTime && vector.DistanceSq(anchorPos, m_BreachFollowAnchorCache) <= 9.0)
			{
				moveGoal = m_BreachFollowGoalCache;
				return true;
			}

			vector forward = targetPos - anchorPos;
			forward[1] = 0.0;
			float fwdLenSq = forward[0] * forward[0] + forward[2] * forward[2];
			if (fwdLenSq < 0.0001)
				return false;

			float invFwdLen = 1.0 / Math.Sqrt(fwdLenSq);
			forward[0] = forward[0] * invFwdLen;
			forward[2] = forward[2] * invFwdLen;
			vector right = forward.Perpend();
			right[1] = 0.0;

			int sideSign = GetGroupFlankSideSign(memberIndex);
			if (sideSign == 0)
				sideSign = ResolvePositioningSideSign(now);
			float sideMul = 1.0;
			if (sideSign < 0)
				sideMul = -1.0;

			float sideDist = Math.Clamp(1.6 + memberIndex * 0.7, 1.6, 4.2);
			float backDist = Math.Clamp(2.2 + memberIndex * 0.5, 2.2, 4.6);
			vector rawGoal = anchorPos - forward * backDist + right * (sideDist * sideMul);
			vector projectedGoal = ProjectToNavigablePosition(rawGoal, 3.5);
			if (vector.DistanceSq(botPos, projectedGoal) < (BQBOT_BREACH_FOLLOW_MIN_DIST_M * BQBOT_BREACH_FOLLOW_MIN_DIST_M))
				projectedGoal = ProjectToNavigablePosition(anchorPos, 3.0);
			if (vector.DistanceSq(botPos, projectedGoal) < 1.44)
				return false;

			moveGoal = projectedGoal;
			// Cache for ~1.6s; refresh sooner if anchor drifts >3m (DistanceSq>9).
			m_BreachFollowGoalCache = projectedGoal;
			m_BreachFollowAnchorCache = anchorPos;
			m_BreachFollowGoalCacheUntilTime = now + 1600;
			MovementDiag("tactical_pick", "reason=breach_follow goal=" + moveGoal.ToString() + " anchor=" + anchorPos.ToString() + " ownDist=" + ownDistToTarget.ToString() + " anchorDist=" + vector.Distance(anchorPos, targetPos).ToString(), true);
			return true;
		}
		protected bool TryGetDefenderSupportMoveGoal(vector targetPos, float distToTarget, bool hasLos, int now, int memberIndex, int memberCount, out vector moveGoal)
		{
			moveGoal = "0 0 0";
			if (!BQBot_IsDefenderSupportPace() || !m_Bot)
				return false;
			// No LOS/search phase should stay a direct push to the remembered/player direction.
			// Support pacing only starts once the bot can actually see/fight.
			if (!hasLos)
				return false;
			if (m_CoverMoveActive || m_FlankActive)
				return false;

			vector supportGoal = CalculateDefenderSupportGoal(targetPos, distToTarget, hasLos, memberIndex, memberCount);
			if (supportGoal == "0 0 0")
				return false;
			if (vector.DistanceSq(m_Bot.GetPosition(), supportGoal) <= 1.44)
			{
				m_LastTacticalGoalReason = "support_hold";
				return false;
			}

			m_PositioningMoveActive = true;
			m_PositioningMoveGoal = supportGoal;
			m_PositioningMoveAnchorTargetPos = targetPos;
			m_PositioningMoveExpireTime = now + BQBOT_DEFENDER_SUPPORT_HOLD_MS;
			moveGoal = supportGoal;
			m_LastTacticalGoalReason = "support_step_new";
			string tacticalPickLog = "reason=" + m_LastTacticalGoalReason;
		tacticalPickLog = tacticalPickLog + " goal=" + moveGoal.ToString();
		tacticalPickLog = tacticalPickLog + " distTarget=" + distToTarget.ToString();
		tacticalPickLog = tacticalPickLog + " los=" + BoolTo01(hasLos);
		tacticalPickLog = tacticalPickLog + " member=" + memberIndex.ToString();
		MovementDiag("tactical_pick", tacticalPickLog, true);
			return true;
		}

		protected vector CalculateDefenderSupportGoal(vector targetPos, float distToTarget, bool hasLos, int memberIndex, int memberCount)
		{
			if (!m_Bot)
				return "0 0 0";

			vector botPos = m_Bot.GetPosition();
			vector toTarget = targetPos - botPos;
			toTarget[1] = 0.0;
			float toTargetLenSq = toTarget[0] * toTarget[0] + toTarget[2] * toTarget[2];
			if (toTargetLenSq < 0.0001)
				return botPos;

			float toTargetLen = Math.Sqrt(toTargetLenSq);
			toTarget[0] = toTarget[0] / toTargetLen;
			toTarget[2] = toTarget[2] / toTargetLen;

			float desiredDist = BQBOT_DEFENDER_SUPPORT_DESIRED_LOS_M;
			if (!hasLos)
				desiredDist = BQBOT_DEFENDER_SUPPORT_DESIRED_NOLOS_M;

			vector goal = botPos;
			if (distToTarget > desiredDist + 5.0)
			{
				float advanceDist = Math.Clamp(distToTarget - desiredDist, 2.5, BQBOT_DEFENDER_SUPPORT_MAX_STEP_M);
				goal = botPos + toTarget * advanceDist;

				if (memberCount > 1 && memberIndex > 0)
				{
					vector right = toTarget.Perpend();
					right[1] = 0.0;
					int sideSign = GetGroupFlankSideSign(memberIndex);
					float sideMul = 1.0;
					if (sideSign < 0)
						sideMul = -1.0;
					float laneOffset = Math.Clamp(1.6 + memberIndex * 0.55, 1.6, 4.2);
					goal = goal + right * (sideMul * laneOffset);
				}
			}
			else if (distToTarget < desiredDist - 13.0)
			{
				float retreatDist = Math.Clamp((desiredDist - 13.0) - distToTarget, 1.5, 4.0);
				goal = botPos - toTarget * retreatDist;
			}
			else
			{
				// Already in a useful support band. Do not emit current position as a goal,
				// otherwise UpdateChaseMovement will spam move_goal_reached and freeze pursuit.
				return "0 0 0";
			}

			return ProjectToNavigablePosition(goal, 2.5);
		}

		protected vector CalculateCombatPositioningGoal(vector targetPos, float distToTarget, float threat, int now, int memberIndex = -1, int memberCount = 1)
		{
			if (!m_Bot)
				return "0 0 0";

			vector botPos = m_Bot.GetPosition();
			vector toTarget = targetPos - botPos;
			toTarget[1] = 0.0;
			float toTargetLenSq = toTarget[0] * toTarget[0] + toTarget[2] * toTarget[2];
			if (toTargetLenSq < 0.0001)
				return "0 0 0";

			float invToTargetLen = 1.0 / Math.Sqrt(toTargetLenSq);
			toTarget[0] = toTarget[0] * invToTargetLen;
			toTarget[2] = toTarget[2] * invToTargetLen;

			// Keep tactical spacing independent from very large fire radius values.
			// This prevents unnatural constant retreat when fire radius is configured high (e.g. 300).
			float tacticalFireRadius = Math.Clamp(GetFireRadius(), 20.0, 120.0);
			float desiredDistance = Math.Clamp(tacticalFireRadius * 0.28, 10.0, 34.0);
			float minDistance = desiredDistance * 0.72;
			float maxDistance = desiredDistance * 1.15;
			if (threat >= m_ThreatCombatEnter)
			{
				minDistance = minDistance + 4.0;
				maxDistance = maxDistance + 2.0;
			}

			vector right = toTarget.Perpend();
			right[1] = 0.0;
			int preferredSign = 0;
			if (memberCount > 1 && memberIndex > 0)
				preferredSign = GetGroupFlankSideSign(memberIndex);
			int sideSign = ResolvePositioningSideSign(now, preferredSign);
			float sideMul = 1.0;
			if (sideSign < 0)
				sideMul = -1.0;
			int slotRow = 1;
			if (memberIndex > 0)
				slotRow = Math.Max(1, (memberIndex + 1) / 2);

			vector goal = "0 0 0";
			if (distToTarget < minDistance)
			{
				float retreatDist = Math.Clamp((minDistance - distToTarget) + 1.5, 2.0, 8.0);
				goal = botPos - toTarget * retreatDist;
				if (memberCount > 1 && memberIndex > 0)
				{
					float retreatSide = Math.Clamp(1.2 + slotRow * 0.65, 1.2, 4.5);
					goal = goal + right * (sideMul * retreatSide);
				}
			}
			else if (distToTarget > maxDistance)
			{
				float advanceDist = Math.Clamp((distToTarget - maxDistance) + 1.5, 2.0, 8.0);
				goal = botPos + toTarget * advanceDist;
				if (memberCount > 1 && memberIndex > 0)
				{
					// Keep followers in lanes while advancing to prevent snake-like stacking.
					float laneOffset = Math.Clamp(2.0 + slotRow * 1.1, 2.0, 6.5);
					float rowLag = Math.Clamp(0.4 + slotRow * 0.45, 0.4, 2.4);
					goal = goal + right * (sideMul * laneOffset) - toTarget * rowLag;
				}
			}
			else
			{
				float lateralDist = Math.Clamp(2.2 + threat * 3.0, 2.0, 6.0);
				goal = botPos + right * (sideMul * lateralDist);
			}

			return ProjectToNavigablePosition(goal, 2.5);
		}

		protected vector CalculateCloseBackpedalGoal(vector targetPos, int now)
		{
			if (!m_Bot)
				return "0 0 0";

			vector botPos = m_Bot.GetPosition();
			vector away = botPos - targetPos;
			away[1] = 0.0;
			float awayLenSq = away[0] * away[0] + away[2] * away[2];
			if (awayLenSq < 0.0001)
				return "0 0 0";

			float awayLen = Math.Sqrt(awayLenSq);
			away = Vector(away[0] / awayLen, 0.0, away[2] / awayLen);
			vector side = away.Perpend();
			side[1] = 0.0;

			int sideSign = ResolvePositioningSideSign(now);
			float sideMul = 1.0;
			if (sideSign < 0)
				sideMul = -1.0;

			float backDist = Math.RandomFloat(4.5, 7.5);
			float sideDist = Math.RandomFloat(1.5, 3.5);
			vector goal = botPos + away * backDist + side * (sideMul * sideDist);
			return ProjectToNavigablePosition(goal, 3.0);
		}

		protected bool IsFlankCandidateValid(vector targetPos, vector anchorDir, vector candidatePos, int sideSign, float minAngleDeg)
		{
			if (!m_Bot)
				return false;

			vector botPos = m_Bot.GetPosition();
			vector toCandidate = candidatePos - targetPos;
			toCandidate[1] = 0.0;
			float toCandidateLenSq = toCandidate[0] * toCandidate[0] + toCandidate[2] * toCandidate[2];
			if (toCandidateLenSq <= 0.0001)
				return false;
			float toCandidateLen = Math.Sqrt(toCandidateLenSq);
			toCandidate[0] = toCandidate[0] / toCandidateLen;
			toCandidate[2] = toCandidate[2] / toCandidateLen;

			float minTargetDist = Math.Max(m_ChaseStopDistanceM + 3.0, 6.0);
			if (toCandidateLen < minTargetDist)
				return false;

			float botTravelDist = vector.Distance(botPos, candidatePos);
			if (botTravelDist < FLANK_MIN_TRAVEL_M)
				return false;

			float minAngleCos = Math.Cos(minAngleDeg * Math.DEG2RAD);
			float angleDot = anchorDir[0] * toCandidate[0] + anchorDir[2] * toCandidate[2];
			if (angleDot > minAngleCos)
				return false;

			if (sideSign != 0)
			{
				vector right = anchorDir.Perpend();
				right[1] = 0.0;
				float sideDot = toCandidate[0] * right[0] + toCandidate[2] * right[2];
				if (sideSign > 0 && sideDot < 0.12)
					return false;
				if (sideSign < 0 && sideDot > -0.12)
					return false;
			}

			return true;
		}

		protected bool TryBuildGuaranteedFlankSpreadGoal(vector targetPos, float distToTarget, int now, int sideSign, int memberIndex, int memberCount, out vector flankGoal)
		{
			flankGoal = "0 0 0";
			if (!m_Bot)
				return false;

			vector botPos = m_Bot.GetPosition();
			vector anchorDir = botPos - targetPos;
			anchorDir[1] = 0.0;
			float anchorLenSq = anchorDir[0] * anchorDir[0] + anchorDir[2] * anchorDir[2];
			if (anchorLenSq <= 0.0001)
				return false;
			float invAnchorLen = 1.0 / Math.Sqrt(anchorLenSq);
			anchorDir[0] = anchorDir[0] * invAnchorLen;
			anchorDir[2] = anchorDir[2] * invAnchorLen;

			if (sideSign == 0)
				sideSign = ResolvePositioningSideSign(now);

			vector right = anchorDir.Perpend();
			right[1] = 0.0;
			float sideMul = 1.0;
			if (sideSign < 0)
				sideMul = -1.0;

			int slotRow = 1;
			if (memberCount > 1 && memberIndex > 0)
				slotRow = Math.Max(1, (memberIndex + 1) / 2);

			float ringDist = Math.Clamp(distToTarget * 0.70, 6.0, m_MaxFlankingDistance);
			float sideDist = Math.Clamp(4.0 + slotRow * 1.6, 4.0, 10.5);
			float rearBias = Math.Clamp(0.9 + slotRow * 0.5, 0.9, 3.4);
			vector candidateRaw = targetPos + anchorDir * ringDist + right * (sideMul * sideDist) - anchorDir * rearBias;
			vector candidate = ProjectToNavigablePosition(candidateRaw, 4.0);
			if (candidate == "0 0 0")
				return false;

			if (!IsFlankCandidateValid(targetPos, anchorDir, candidate, sideSign, 26.0))
				return false;

			flankGoal = candidate;
			return true;
		}

		protected void ResetFlankState()
		{
			m_FlankActive = false;
			m_FlankPosition = "0 0 0";
			m_FlankExpireTime = 0;
			m_FlankAnchorTargetPos = "0 0 0";
			m_FlankSideSign = 0;
			m_FlankNextReplanTime = 0;
			m_FlankLastProgressPos = "0 0 0";
			m_FlankNextProgressCheckTime = 0;
			m_FlankNoProgressAccumMs = 0;
		}

		protected void ActivateFlankGoal(vector flankPos, vector targetPos, int sideSign, int now)
		{
			m_FlankActive = true;
			m_FlankPosition = flankPos;
			m_FlankAnchorTargetPos = targetPos;
			m_FlankSideSign = sideSign;
			m_FlankNextReplanTime = now + FLANK_REEVAL_INTERVAL_MS;
			m_FlankLastProgressPos = m_Bot.GetPosition();
			m_FlankNextProgressCheckTime = now + FLANK_PROGRESS_CHECK_MS;
			m_FlankNoProgressAccumMs = 0;

			float flankSpan = vector.Distance(m_Bot.GetPosition(), targetPos);
			int flankTimeoutMin = Math.Round(flankSpan * 300.0);
			int flankTimeoutMax = Math.Round(flankSpan * 600.0);
			if (flankTimeoutMin < 3000)
				flankTimeoutMin = 3000;
			if (flankTimeoutMax < flankTimeoutMin)
				flankTimeoutMax = flankTimeoutMin;
			m_FlankExpireTime = now + Math.RandomIntInclusive(flankTimeoutMin, flankTimeoutMax);
		}

		protected void InitChaseNavigation()
		{
			if (m_ChaseAIWorld)
				return;

			if (!GetGame() || !GetGame().GetWorld())
				return;

			m_ChaseAIWorld = GetGame().GetWorld().GetAIWorld();
			if (!m_ChaseAIWorld)
				return;

			m_ChaseSampleFilter = new PGFilter();

			// Sampling filter mirrors Expansion all-filter: sample broad navmesh, let path handler decide unreachable.
			int sampleInclude = PGPolyFlags.ALL & ~(PGPolyFlags.CRAWL | PGPolyFlags.CROUCH);
			int sampleExclude = PGPolyFlags.CRAWL | PGPolyFlags.CROUCH;
			m_ChaseSampleFilter.SetFlags(sampleInclude, sampleExclude, PGPolyFlags.NONE);

		}

		protected void ClearChasePath()
		{
			// Disabled. BQ manager must not own movement/path state.
		}

		protected void StopChaseMovement(bool clearPath = false)
		{
			// Disabled for Variant A. Expansion FSM/path handler owns movement stop/target clearing.
			m_ChaseMoveActive = false;
			m_GroupFormationGoalActive = false;
			m_GroupFormationDistance = 0.0;
			m_LastTurnUpdateTimeMs = 0;
			m_TurnSmoothVel[0] = 0.0;
			m_ChaseOverrideUntilTime = 0;
			m_ChaseOverrideMovementAngle = 0.0;
			m_ChaseOverrideReason = "";
		}

		protected bool TryBuildLocalEscapeGoal(vector towardGoal, out vector escapeGoal)
		{
			escapeGoal = "0 0 0";
			if (!m_Bot)
				return false;

			InitChaseNavigation();
			if (!m_ChaseAIWorld || !m_ChaseSampleFilter)
				return false;

			vector botPos = m_Bot.GetPosition();
			vector toGoal = towardGoal - botPos;
			toGoal[1] = 0.0;
			float toGoalLenSq = toGoal[0] * toGoal[0] + toGoal[2] * toGoal[2];
			float baseYaw = 0.0;
			if (toGoalLenSq >= 0.04)
				baseYaw = toGoal.VectorToAngles()[0];
			else
				baseYaw = m_Bot.GetOrientation()[0];

			array<float> yawOffsets = new array<float>;
			yawOffsets.Insert(-150.0);
			yawOffsets.Insert(150.0);
			yawOffsets.Insert(-110.0);
			yawOffsets.Insert(110.0);
			yawOffsets.Insert(-70.0);
			yawOffsets.Insert(70.0);
			yawOffsets.Insert(180.0);
			yawOffsets.Insert(-35.0);
			yawOffsets.Insert(35.0);
			yawOffsets.Insert(0.0);

			array<float> probeRadii = new array<float>;
			probeRadii.Insert(2.0);
			probeRadii.Insert(3.0);
			probeRadii.Insert(4.0);
			probeRadii.Insert(5.5);
			probeRadii.Insert(7.0);

			float bestScore = -99999.0;
			vector bestGoal = "0 0 0";
			bool found = false;
			float currentGoalDist = vector.Distance(botPos, towardGoal);
			float minEscapeDist = Math.Max(m_ChaseStopDistanceM + 1.2, 3.5);

			foreach (float radius : probeRadii)
			{
				foreach (float yawOffset : yawOffsets)
				{
					float yaw = baseYaw + yawOffset;
					float yawRad = yaw * Math.DEG2RAD;
					vector dir = Vector(Math.Sin(yawRad), 0.0, Math.Cos(yawRad));
					vector probe = botPos + dir * radius;
					vector sampled = probe;
					if (!m_ChaseAIWorld.SampleNavmeshPosition(sampled, 1.8, m_ChaseSampleFilter, sampled))
						continue;

					float moveDist = vector.Distance(botPos, sampled);
					if (moveDist < minEscapeDist)
						continue;

					float towardDist = vector.Distance(sampled, towardGoal);
					float progress = currentGoalDist - towardDist;
					float score = (progress * 0.75) + (moveDist * 0.65);
					if (!found || score > bestScore)
					{
						found = true;
						bestScore = score;
						bestGoal = sampled;
					}
				}
			}

			if (!found)
				return false;

			escapeGoal = bestGoal;
			return true;
		}

		protected void ResetStableMoveGoal()
		{
			m_StableMoveGoalActive = false;
			m_StableMoveGoal = "0 0 0";
			m_StableMoveAnchorTargetPos = "0 0 0";
			m_StableMoveGoalUntilTime = 0;
			m_StableMoveGoalSource = "";
			m_StableMoveGoalKind = "";
		}

		protected string GetStableMoveGoalKind(string goalSource)
		{
			if (goalSource.Contains("cover"))
				return "cover";
			if (goalSource.Contains("flank"))
				return "flank";
			if (goalSource.Contains("support"))
				return "support";
			if (goalSource.Contains("positioning"))
				return "positioning";
			if (goalSource.Contains("formation"))
				return "formation";
			if (goalSource.Contains("no_los") || goalSource.Contains("search") || goalSource.Contains("aggro_memory"))
				return "no_los";
			return "direct";
		}

		protected int GetStableMoveGoalHoldMs(string kind)
		{
			if (kind == "cover")
				return 7000;
			if (kind == "flank")
				return 6500;
			if (kind == "support")
				return 5200;
			if (kind == "positioning")
				return 4800 + Math.RandomIntInclusive(0, 1800);
			if (kind == "formation")
				return 3800;
			if (kind == "no_los")
				return 4200;
			return MOVE_GOAL_HOLD_MIN_MS;
		}

		protected bool IsStableMoveGoalAllowed(string goalSource, bool hasLos)
		{
			if (goalSource.Contains("+evade"))
				return false;

			if (!hasLos)
				return true;

			if (goalSource.Contains("tactical") || goalSource.Contains("formation") || goalSource.Contains("cover") || goalSource.Contains("flank") || goalSource.Contains("positioning") || goalSource.Contains("support"))
				return true;

			return false;
		}

		protected bool ShouldKeepStableMoveGoal(string newKind, vector candidateGoal, vector botPos, vector targetPos, bool hasLos, int now)
		{
			if (!m_StableMoveGoalActive || m_StableMoveGoal == "0 0 0")
				return false;
			if (now >= m_StableMoveGoalUntilTime)
				return false;

			float reachedDist = Math.Max(2.2, m_ChaseStopDistanceM + MOVE_GOAL_REACHED_EXTRA_M);
			if (vector.Distance(botPos, m_StableMoveGoal) <= reachedDist)
				return false;


			float candidateDelta = vector.Distance(candidateGoal, m_StableMoveGoal);
			if (candidateDelta <= MOVE_GOAL_FORCE_REPLACE_DIST_M)
				return true;

			if (m_StableMoveGoalKind == "cover")
				return newKind == "cover";
			if (m_StableMoveGoalKind == "flank")
				return newKind == "flank" || newKind == "positioning" || newKind == "formation" || newKind == "no_los";
			if (m_StableMoveGoalKind == "support")
				return newKind == "support" || newKind == "formation" || newKind == "no_los";
			if (m_StableMoveGoalKind == "positioning")
				return newKind == "positioning" || newKind == "formation" || newKind == "no_los";
			if (m_StableMoveGoalKind == "formation")
				return newKind == "formation" || newKind == "no_los";
			if (m_StableMoveGoalKind == "no_los")
				return !hasLos && newKind != "cover" && newKind != "flank";

			return false;
		}

		protected vector ResolveStableMoveGoal(vector candidateGoal, vector targetPos, bool hasLos, int now, string goalSource, out string resolvedGoalSource)
		{
			resolvedGoalSource = goalSource;
			if (candidateGoal == "0 0 0")
			{
				ResetStableMoveGoal();
				return candidateGoal;
			}

			if (!IsStableMoveGoalAllowed(goalSource, hasLos))
			{
				ResetStableMoveGoal();
				return candidateGoal;
			}

			vector botPos = m_Bot.GetPosition();
			string newKind = GetStableMoveGoalKind(goalSource);
			if (ShouldKeepStableMoveGoal(newKind, candidateGoal, botPos, targetPos, hasLos, now))
			{
				resolvedGoalSource = goalSource + "+stable:" + m_StableMoveGoalKind;
				return m_StableMoveGoal;
			}

			int holdMs = GetStableMoveGoalHoldMs(newKind);
			holdMs = Math.Clamp(holdMs, MOVE_GOAL_HOLD_MIN_MS, MOVE_GOAL_HOLD_MAX_MS);
			m_StableMoveGoalActive = true;
			m_StableMoveGoal = candidateGoal;
			m_StableMoveAnchorTargetPos = targetPos;
			m_StableMoveGoalUntilTime = now + holdMs;
			m_StableMoveGoalSource = goalSource;
			m_StableMoveGoalKind = newKind;
			resolvedGoalSource = goalSource + "+stable_new:" + newKind;
			return candidateGoal;
		}

		protected float GetChaseSpeed(float distToGoal)
		{
			if (distToGoal >= m_ChaseSprintDistanceM)
				return 3.0;

			if (distToGoal >= m_ChaseJogDistanceM)
				return 2.0;

			// Allow running (2.0) while in firing range for faster flanking
			if (distToGoal <= m_ChaseStopDistanceM)
				return 0.0;

			return 1.5;
		}

		protected float ClampSpeedByLimit(float speed, int speedLimit)
		{
			if (speedLimit <= 1)
			{
				if (speed > 1.0)
					return 1.0;
				return speed;
			}

			if (speedLimit == 2)
			{
				if (speed > 2.0)
					return 2.0;
				return speed;
			}

			if (speed > 3.0)
				return 3.0;
			return speed;
		}

		protected float ApplyMovementSpeedLimits(float desiredSpeed, bool underThreat, bool hasTarget)
		{
			int activeLimit = m_SpeedLimitPreference;
			if (activeLimit <= 0)
				activeLimit = 3;

			if (!hasTarget)
				activeLimit = m_RoamingMovementSpeedLimit;
			if (underThreat)
				activeLimit = m_MovementSpeedLimitUnderThreat;
			else if (m_MovementSpeedLimit > 0 && activeLimit > m_MovementSpeedLimit)
				activeLimit = m_MovementSpeedLimit;

			return ClampSpeedByLimit(desiredSpeed, activeLimit);
		}

		protected float ApplyCombatPaceCap(float desiredSpeed, float distToTarget, bool hasLos, bool forceMediumPursuitSpeed)
		{
			if (desiredSpeed <= 0.0)
				return desiredSpeed;

			// Expansion-like cadence:
			// close = jog, medium = run, far = fast run.
			if (hasLos)
			{
				float closeDist = Math.Max(m_ChaseStopDistanceM + 3.0, m_ChaseJogDistanceM * 1.6);
				float mediumDist = Math.Max(closeDist + 8.0, m_ChaseSprintDistanceM * 0.85);

				if (distToTarget <= closeDist)
				{
					if (desiredSpeed > 1.5)
						desiredSpeed = 1.5;
				}
				else if (distToTarget <= mediumDist)
				{
					if (desiredSpeed > 2.0)
						desiredSpeed = 2.0;
				}
				else if (desiredSpeed > 2.5)
				{
					desiredSpeed = 2.5;
				}
			}
			else if (distToTarget <= Math.Max(10.0, m_ChaseJogDistanceM * 1.2))
			{
				// Lost-LOS pursuit remains aggressive, but avoid sprint in near-contact.
				if (desiredSpeed > 2.0)
					desiredSpeed = 2.0;
			}

			if (forceMediumPursuitSpeed && !hasLos)
			{
				// Lost-LOS chase should stay at medium tempo (run/jog), not full sprint.
				if (distToTarget > (m_ChaseStopDistanceM + 24.0))
				{
					if (desiredSpeed < 2.0)
						desiredSpeed = 2.0;
				}
				else if (distToTarget > (m_ChaseStopDistanceM + 10.0))
				{
					if (desiredSpeed < 1.75)
						desiredSpeed = 1.75;
				}
				else if (distToTarget > (m_ChaseStopDistanceM + 4.0))
				{
					if (desiredSpeed < 1.5)
						desiredSpeed = 1.5;
				}
				if (desiredSpeed > 2.0)
					desiredSpeed = 2.0;
			}

			return desiredSpeed;
		}

		// Build a strong perpendicular escape goal (~7m sideways) that forces a fresh
		// route when the bot is hard-stuck against geometry the navmesh can't see.
		// Probes at GROUND level (terrain Y) when bot is elevated above terrain - this
		// is what knocks the bot off a beam/structure top onto walkable ground.
		protected void ForceFaceTargetForFire(vector targetPos)
		{
			if (!m_Bot)
				return;

			vector toTarget = targetPos - m_Bot.GetPosition();
			toTarget[1] = 0.0;
			float lenSq = toTarget[0] * toTarget[0] + toTarget[2] * toTarget[2];
			if (lenSq < 0.0001)
				return;

			float yawAbs = NormalizeYawAbs(toTarget.VectorToAngles()[0]);
			m_Bot.BQBot_SetAimHeading(NormalizeYawDelta(yawAbs), false);
			AimToTargetPitchOnly(targetPos);
		}

		protected void AimToTarget(vector targetPos)
		{
			vector botPos = m_Bot.GetPosition();
			vector yawDelta = targetPos - botPos;
			vector yawAngles = yawDelta.VectorToAngles();
			vector aimFrom = botPos;
			int neckBone = m_Bot.GetBoneIndexByName("neck");
			if (neckBone >= 0)
				aimFrom = m_Bot.GetBonePositionWS(neckBone);
			else if (m_BotWeapon)
				aimFrom = m_BotWeapon.GetPosition();
			vector pitchDelta = targetPos - aimFrom;
			vector pitchAngles = pitchDelta.VectorToAngles();
			float yawDesired = yawAngles[0];
			float rawPitch = pitchAngles[1];
			float pitch = NormalizeAimPitchDeg(rawPitch);

			vector oriBefore = m_Bot.GetOrientation();
			float oriPitchBefore = oriBefore[1];
			float yaw = SmoothTurnToYaw(yawDesired, AIM_TURN_STEP_DEG, "aim");
			vector oriAfter = m_Bot.GetOrientation();
			m_Bot.BQBot_SetAimHeading(yaw, false);
			m_Bot.BQBot_SetAimPitch(pitch);
			m_Bot.BQBot_SetAimTarget(targetPos);

			string aimDiagMsg = "aim_update";
			aimDiagMsg = aimDiagMsg + " botPos=" + botPos.ToString();
			aimDiagMsg = aimDiagMsg + " aimFrom=" + aimFrom.ToString();
			aimDiagMsg = aimDiagMsg + " targetPos=" + targetPos.ToString();
			aimDiagMsg = aimDiagMsg + " yawDelta=" + yawDelta.ToString();
			aimDiagMsg = aimDiagMsg + " pitchDelta=" + pitchDelta.ToString();
			aimDiagMsg = aimDiagMsg + " rawPitch=" + rawPitch.ToString();
			aimDiagMsg = aimDiagMsg + " desiredYaw=" + yawDesired.ToString();
			aimDiagMsg = aimDiagMsg + " appliedYaw=" + yaw.ToString();
			aimDiagMsg = aimDiagMsg + " desiredPitch=" + pitch.ToString();
			aimDiagMsg = aimDiagMsg + " botOriBeforePitch=" + oriPitchBefore.ToString();
			aimDiagMsg = aimDiagMsg + " botOriAfter=" + oriAfter.ToString();
			AimDiag(aimDiagMsg);

			string aimDbgMsg = "AIM:";
			aimDbgMsg = aimDbgMsg + " botPos=" + botPos.ToString();
			aimDbgMsg = aimDbgMsg + " targetPos=" + targetPos.ToString();
			aimDbgMsg = aimDbgMsg + " yawDelta=" + yawDelta.ToString();
			aimDbgMsg = aimDbgMsg + " pitchDelta=" + pitchDelta.ToString();
			aimDbgMsg = aimDbgMsg + " yaw=" + yaw.ToString();
			aimDbgMsg = aimDbgMsg + " pitch=" + pitch.ToString();
			aimDbgMsg = aimDbgMsg + " botOri=" + oriAfter.ToString();
			DebugLog(aimDbgMsg);
		}

		protected void AimToTargetPitchOnly(vector targetPos)
		{
			if (!m_Bot)
				return;

			vector aimFrom = m_Bot.GetPosition();
			int neckBone = m_Bot.GetBoneIndexByName("neck");
			if (neckBone >= 0)
				aimFrom = m_Bot.GetBonePositionWS(neckBone);
			else if (m_BotWeapon)
				aimFrom = m_BotWeapon.GetPosition();

			vector pitchDelta = targetPos - aimFrom;
			vector pitchAngles = pitchDelta.VectorToAngles();
			float pitch = NormalizeAimPitchDeg(pitchAngles[1]);

			m_Bot.BQBot_SetAimPitch(pitch);
			m_Bot.BQBot_SetAimTarget(targetPos);
		}

		protected float NormalizeAimPitchDeg(float pitchDeg)
		{
			while (pitchDeg > 180.0)
				pitchDeg = pitchDeg - 360.0;

			while (pitchDeg < -180.0)
				pitchDeg = pitchDeg + 360.0;

			return Math.Clamp(pitchDeg, -85.0, 85.0);
		}

		protected vector GetAimPoint(PlayerBase target)
		{
			if (!target)
				return "0 0 0";

			vector aim;
			if (TryGetUnitBonePoint(target, "spine3", aim) || TryGetUnitBonePoint(target, "Spine3", aim))
				return aim;

			if (TryGetUnitBonePoint(target, "spine2", aim) || TryGetUnitBonePoint(target, "Spine2", aim))
				return aim;

			if (TryGetUnitBonePoint(target, "chest", aim) || TryGetUnitBonePoint(target, "Chest", aim))
				return aim;

			aim = target.GetPosition();
			aim[1] = aim[1] + 1.22;
			return aim;
		}
		protected void AimToTargetThrottled(vector targetPos, bool force = false)
		{
			int now = GetGame().GetTime();
			if (!force && now < m_NextAimUpdateTime)
				return;

			AimToTarget(targetPos);
			m_NextAimUpdateTime = now + m_AimUpdateIntervalMs;
		}













	protected void AutoFireHold(vector targetPos)
		{
			string mode = GetFireMode();
			if (mode == "single")
			{
				AutoFireSingle(targetPos);
				return;
			}

			if (mode == "burst")
			{
				AutoFireBurst(targetPos);
				return;
			}

			int muzzle = m_BotWeapon.GetCurrentMuzzle();
			if (!m_BotWeapon.GetCurrentModeAutoFire(muzzle))
				EnsureAutoFireMode(false);
			EnsureRoundChambered("autofire");
			string holdReason = "";
			bool holdWanted = BotShouldHoldTriggerReason(holdReason);
			WeaponTrace("autofire_hold_eval", "holdWanted=" + BoolTo01(holdWanted) + " reason=" + holdReason + " targetPos=" + targetPos.ToString());
			// Full-auto must keep the trigger held through short vanilla weapon-cycle gaps.
			// Real no-ammo/reload/chamber blockers are returned before this reason.
			if (!holdWanted && holdReason == "weapon_manager_cannot_fire")
				holdWanted = true;
			m_Bot.BQBot_SetVanillaTriggerHoldWanted(holdWanted);
			string triggerStateDetails = "holdWanted=" + BoolTo01(holdWanted);
			triggerStateDetails = triggerStateDetails + " chamberEmpty=" + BoolTo01(m_BotWeapon.IsChamberEmpty(muzzle));
			triggerStateDetails = triggerStateDetails + " chamberFiredOut=" + BoolTo01(m_BotWeapon.IsChamberFiredOut(muzzle));
			if (VERBOSE_LOGS)
				StageLog("TRIGGER_STATE", triggerStateDetails);

			if (AIM_DIAG_LOGS)
			{
				string aimTriggerMsg = "trigger_state";
				aimTriggerMsg = aimTriggerMsg + " holdWanted=" + BoolTo01(holdWanted);
				aimTriggerMsg = aimTriggerMsg + " botPos=" + m_Bot.GetPosition().ToString();
				aimTriggerMsg = aimTriggerMsg + " targetPos=" + targetPos.ToString();
				aimTriggerMsg = aimTriggerMsg + " ori=" + m_Bot.GetOrientation().ToString();
				aimTriggerMsg = aimTriggerMsg + " chamberEmpty=" + BoolTo01(m_BotWeapon.IsChamberEmpty(muzzle));
				aimTriggerMsg = aimTriggerMsg + " chamberFiredOut=" + BoolTo01(m_BotWeapon.IsChamberFiredOut(muzzle));
				AimDiag(aimTriggerMsg);
			}

			if (FIRE_DIAG_LOGS)
			{
				int currentMode = m_BotWeapon.GetCurrentMode(muzzle);
				bool currentAuto = m_BotWeapon.GetCurrentModeAutoFire(muzzle);
				int currentBurstSize = m_BotWeapon.GetCurrentModeBurstSize(muzzle);
				Magazine curMag = Magazine.Cast(m_BotWeapon.GetMagazine(muzzle));
				int magAmmo = -1;
				if (curMag)
					magAmmo = curMag.GetAmmoCount();

				string holdEvalMsg = "hold_eval intent=auto";
				holdEvalMsg = holdEvalMsg + " holdWanted=" + BoolTo01(holdWanted);
				holdEvalMsg = holdEvalMsg + " reason=" + holdReason;
				holdEvalMsg = holdEvalMsg + " muzzle=" + muzzle.ToString();
				holdEvalMsg = holdEvalMsg + " mode=" + currentMode.ToString();
				holdEvalMsg = holdEvalMsg + " modeName=" + m_BotWeapon.GetCurrentModeName(muzzle);
				holdEvalMsg = holdEvalMsg + " modeAuto=" + BoolTo01(currentAuto);
				holdEvalMsg = holdEvalMsg + " burstSize=" + currentBurstSize.ToString();
				holdEvalMsg = holdEvalMsg + " serverShots=" + m_Bot.BQBot_GetServerShotCount().ToString();
				holdEvalMsg = holdEvalMsg + " magAmmo=" + magAmmo.ToString();
				holdEvalMsg = holdEvalMsg + " botPos=" + m_Bot.GetPosition().ToString();
				holdEvalMsg = holdEvalMsg + " targetPos=" + targetPos.ToString();
				holdEvalMsg = holdEvalMsg + " ori=" + m_Bot.GetOrientation().ToString();
				holdEvalMsg = holdEvalMsg + " raised=" + BoolTo01(m_Bot.IsRaised());
				holdEvalMsg = holdEvalMsg + " raiseDone=" + BoolTo01(m_Bot.IsWeaponRaiseCompleted());
				FireDiag(holdEvalMsg);
			}

			// Return aim to target after shot for believable tracking.
			AimToTargetThrottled(targetPos);

			if (!holdWanted)
			{
				WeaponManager wm = m_Bot.GetWeaponManager();
				bool mgrCanNow = false;
				if (wm)
					mgrCanNow = wm.CanFire(m_BotWeapon);
				bool canFireNow = BotCanFireNow();

				string dbg = "fire blocked:";
				dbg = dbg + " raised=" + BoolTo01(m_Bot.IsRaised());
				dbg = dbg + " raiseDone=" + BoolTo01(m_Bot.IsWeaponRaiseCompleted());
				dbg = dbg + " mgrCanFire=" + BoolTo01(mgrCanNow);
				dbg = dbg + " botCanFireNow=" + BoolTo01(canFireNow);
				dbg = dbg + " inErect=" + BoolTo01(m_Bot.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_RAISEDERECT));
				dbg = dbg + " inProne=" + BoolTo01(m_Bot.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_RAISEDPRONE));
				dbg = dbg + " dist=" + vector.Distance(m_Bot.GetPosition(), targetPos).ToString();
				dbg = dbg + " chamberEmpty=" + BoolTo01(m_BotWeapon.IsChamberEmpty(muzzle));
				dbg = dbg + " chamberFiredOut=" + BoolTo01(m_BotWeapon.IsChamberFiredOut(muzzle));
				dbg = dbg + " jammed=" + BoolTo01(m_BotWeapon.IsJammed());
				if (VERBOSE_LOGS)
					DebugLog(dbg);
			}
			else
			{
				if (VERBOSE_LOGS)
					DebugLog("hold fire active");
			}
		}

		protected void AutoFireSingle(vector targetPos)
		{
			int now = GetGame().GetTime();
			m_Bot.BQBot_SetVanillaTriggerHoldWanted(false);
			AimToTargetThrottled(targetPos);
			EnsureRoundChambered("single");

			if (now < m_NextSingleRequestTime)
				return;

			string reason = "";
			if (!BotShouldHoldTriggerReason(reason))
				return;

			BQExpansionDiag.Log("BQ_FIRE_BACKEND", "RequestVanillaFire bot=" + m_Bot.ToString() + " targetPos=" + targetPos.ToString());
			m_Bot.BQBot_RequestVanillaFire();
			m_NextSingleRequestTime = now + SINGLE_SHOT_INTERVAL_MS;
		}

		protected void AutoFireBurst(vector targetPos)
		{
			EnsureRoundChambered("burst");
			AimToTargetThrottled(targetPos);

			string reason = "";
			bool holdWanted = BotShouldHoldTriggerReason(reason);
			// Keep burst hold stable through brief CanFire gaps, but release for real
			// no-ammo/reload/chamber-not-ready states so reload can start immediately.
			if (!holdWanted && reason == "weapon_manager_cannot_fire")
				holdWanted = true;

			m_Bot.BQBot_SetVanillaTriggerHoldWanted(holdWanted);

			if (FIRE_DIAG_LOGS)
			{
				int muzzle = m_BotWeapon.GetCurrentMuzzle();
				Magazine curMag = Magazine.Cast(m_BotWeapon.GetMagazine(muzzle));
				int magAmmo = -1;
				if (curMag)
					magAmmo = curMag.GetAmmoCount();
				string burstEvalMsg = "hold_eval intent=burst";
				burstEvalMsg = burstEvalMsg + " holdWanted=" + BoolTo01(holdWanted);
				burstEvalMsg = burstEvalMsg + " reason=" + reason;
				burstEvalMsg = burstEvalMsg + " muzzle=" + muzzle.ToString();
				burstEvalMsg = burstEvalMsg + " mode=" + m_BotWeapon.GetCurrentMode(muzzle).ToString();
				burstEvalMsg = burstEvalMsg + " modeName=" + m_BotWeapon.GetCurrentModeName(muzzle);
				burstEvalMsg = burstEvalMsg + " modeAuto=" + BoolTo01(m_BotWeapon.GetCurrentModeAutoFire(muzzle));
				burstEvalMsg = burstEvalMsg + " burstSize=" + m_BotWeapon.GetCurrentModeBurstSize(muzzle).ToString();
				burstEvalMsg = burstEvalMsg + " serverShots=" + m_Bot.BQBot_GetServerShotCount().ToString();
				burstEvalMsg = burstEvalMsg + " magAmmo=" + magAmmo.ToString();
				burstEvalMsg = burstEvalMsg + " targetPos=" + targetPos.ToString();
				FireDiag(burstEvalMsg);
			}
		}

		protected bool BotShouldHoldTrigger()
		{
			string unusedReason;
			return BotShouldHoldTriggerReason(unusedReason);
		}

	protected bool IsBotRaisedReadyForCombat()
		{
			if (!m_Bot)
				return false;

			bool inErect = m_Bot.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT);
			bool inRaisedErect = m_Bot.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDERECT);
			bool inCrouch = m_Bot.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH);
			bool inRaisedCrouch = m_Bot.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDCROUCH);
			bool raised = m_Bot.IsRaised();
			bool raiseDone = m_Bot.IsWeaponRaiseCompleted();
			bool lifting = m_Bot.IsLiftWeapon();
			bool wantPose = m_Bot.BQBot_IsCombatPoseWanted();

			// Fire if: raiseDone done OR wantPose is set (not waiting for animation)
			if ((inErect || inRaisedErect || inCrouch || inRaisedCrouch) && (raiseDone || wantPose))
				return true;

			// Also accept if still lifting/raising
			if (lifting)
				return true;

			return false;
		}

		protected bool BotShouldHoldTriggerReason(out string reason)
		{
			reason = "ok";

			if (!m_Bot || !m_BotWeapon)
			{
				reason = "no_bot_or_weapon";
				StageLog("HOLD_BLOCK", "reason=no_bot_or_weapon");
				HoldDiag(reason);
				WeaponTrace("hold_block", "reason=" + reason, true);
				return false;
			}

			if (!IsBotRaisedReadyForCombat())
			{
				reason = "not_raised_ready";
				StageLog("HOLD_BLOCK", "reason=not_raised_ready");
				HoldDiag(reason, "raised=" + BoolTo01(m_Bot.IsRaised()) + " raiseDone=" + BoolTo01(m_Bot.IsWeaponRaiseCompleted()) + " inRaisedErect=" + BoolTo01(m_Bot.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDERECT)));
				WeaponTrace("hold_block", "reason=" + reason, true);
				return false;
			}

	if (m_IsReloading)
			{
				// Auto-clear reload flag if stuck too long (3 sec)
				int now = GetGame().GetTime();
				if (m_ReloadStartTime > 0 && (now - m_ReloadStartTime) > 3000)
				{
					m_IsReloading = false;
					m_ReloadStartTime = 0;
				}
				else
				{
					reason = "reloading_flag";
					StageLog("HOLD_BLOCK", "reason=reloading_flag");
					HoldDiag(reason);
					WeaponTrace("hold_block", "reason=" + reason, true);
					return false;
				}
			}

			if (m_Bot.GetEntityInHands() != m_BotWeapon)
			{
				reason = "weapon_not_in_hands";
				StageLog("HOLD_BLOCK", "reason=weapon_not_in_hands");
				HoldDiag(reason);
				WeaponTrace("hold_block", "reason=" + reason, true);
				return false;
			}

			if (m_BotWeapon.IsDamageDestroyed())
			{
				reason = "destroyed";
				StageLog("HOLD_BLOCK", "reason=destroyed");
				HoldDiag(reason);
				WeaponTrace("hold_block", "reason=" + reason, true);
				return false;
			}

			if (m_BotWeapon.IsJammed())
			{
				// Bots run with jams disabled.
				m_BotWeapon.SetJammed(false);
			}

			DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
			if (inv && inv.IsProcessing())
			{
				reason = "inventory_processing";
				StageLog("HOLD_BLOCK", "reason=inventory_processing");
				HoldDiag(reason);
				WeaponTrace("hold_block", "reason=" + reason, true);
				return false;
			}

			int muzzle = m_BotWeapon.GetCurrentMuzzle();
			Magazine mag = Magazine.Cast(m_BotWeapon.GetMagazine(muzzle));
			int magAmmo = 0;
			if (mag)
				magAmmo = mag.GetAmmoCount();

			bool chamberReady = !m_BotWeapon.IsChamberEmpty(muzzle) && !m_BotWeapon.IsChamberFiredOut(muzzle);
			if (!chamberReady && magAmmo > 0)
			{
				reason = "needs_chamber_round";
				StageLog("HOLD_BLOCK", "reason=needs_chamber_round magAmmo=" + magAmmo.ToString());
				HoldDiag(reason, "magAmmo=" + magAmmo.ToString());
				WeaponTrace("hold_block", "reason=" + reason + " magAmmo=" + magAmmo.ToString(), true);
				return false;
			}
			if (!chamberReady && magAmmo <= 0)
			{
				reason = "no_ammo";
				StageLog("HOLD_BLOCK", "reason=no_ammo");
				HoldDiag(reason);
				WeaponTrace("hold_block", "reason=" + reason, true);
				return false;
			}

			PlayerBase fireTarget = m_CurrentTarget;
			if (fireTarget && fireTarget.IsAlive())
			{
				if (!HasLineOfSightToTarget(fireTarget))
				{
					reason = "no_los";
					StageLog("HOLD_BLOCK", "reason=no_los");
					HoldDiag(reason);
					return false;
				}

				vector fireTargetPos = GetAimPoint(fireTarget);
				if (HasFriendlyBotInFireLine(fireTarget, fireTargetPos))
				{
					reason = "friendly_bot_in_line";
					FriendlyFireDiag("hold blocked by friendly line target=" + fireTarget.GetType() + " targetPos=" + fireTargetPos.ToString(), true);
					StageLog("HOLD_BLOCK", "reason=friendly_bot_in_line");
					HoldDiag(reason);
					return false;
				}
			}

	WeaponManager wm = m_Bot.GetWeaponManager();
			bool mgrCanFire = false;
			if (wm)
				mgrCanFire = wm.CanFire(m_BotWeapon);
			if (!wm || !mgrCanFire)
			{
				reason = "weapon_manager_cannot_fire";
				StageLog("HOLD_BLOCK", "reason=weapon_manager_cannot_fire");
				HoldDiag(reason);
				WeaponTrace("hold_block", "reason=" + reason, true);
				return false;
			}

			StageLog("HOLD_READY", "magAmmo=" + magAmmo.ToString(), true);
			WeaponTrace("hold_ready", "magAmmo=" + magAmmo.ToString(), true);
			HoldDiag("ok", "magAmmo=" + magAmmo.ToString());
			return true;
		}

		protected void EnsureRoundChambered(string sourceTag, bool forceLog = false)
		{
			if (!m_Bot || !m_BotWeapon)
				return;

			if (m_IsReloading)
				return;

			if (m_Bot.GetEntityInHands() != m_BotWeapon)
				return;

			DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
			if (inv && inv.IsProcessing())
				return;

			if (!m_BotWeapon.IsIdle() || m_BotWeapon.IsWaitingForActionFinish())
				return;

			int muzzle = m_BotWeapon.GetCurrentMuzzle();
			bool chamberEmpty = m_BotWeapon.IsChamberEmpty(muzzle);
			bool chamberFiredOut = m_BotWeapon.IsChamberFiredOut(muzzle);
			if (!chamberEmpty && !chamberFiredOut)
			{
				m_ChamberEventInFlight = false;
				return;
			}

			Magazine mag = Magazine.Cast(m_BotWeapon.GetMagazine(muzzle));
			if (!mag || mag.GetAmmoCount() <= 0)
				return;

			int now = GetGame().GetTime();
			if (now < m_NextChamberAttemptTime)
				return;

			if (m_ChamberEventInFlight)
			{
				if (now - m_LastChamberPostTime < CHAMBER_INFLIGHT_GUARD_MS)
					return;

				m_ChamberEventInFlight = false;
			}

			if (!CanPostWeaponEvent())
			{
				m_NextChamberAttemptTime = now + CHAMBER_RETRY_WHEN_BUSY_MS;
				return;
			}

			WeaponTrace("post_event_mechanism", "source=" + sourceTag + " magAmmo=" + mag.GetAmmoCount().ToString(), true);
			inv.PostWeaponEvent(new WeaponEventMechanism(m_Bot, NULL));
			m_ChamberEventInFlight = true;
			m_LastChamberPostTime = now;
			m_NextChamberAttemptTime = now + 350;

			if (forceLog)
			{
				string dbg = "rechamber: source=" + sourceTag;
				dbg = dbg + " mode=mechanism_event";
				dbg = dbg + " magAmmo=" + mag.GetAmmoCount().ToString();
				dbg = dbg + " inFlight=" + BoolTo01(m_ChamberEventInFlight);
				VerboseLog(dbg);
			}
		}

		protected void EnsureAutoFireMode(bool forceLog = false)
		{
			if (!m_BotWeapon)
				return;

			int muzzle = m_BotWeapon.GetCurrentMuzzle();
			int modeCount = m_BotWeapon.GetMuzzleModeCount(muzzle);
			if (modeCount <= 1)
				return;

			string requestedMode = GetFireMode();
			int prevMode = m_BotWeapon.GetCurrentMode(muzzle);
			int selectedMode = -1;
			int fallbackAutoMode = -1;
			int fallbackSemiMode = -1;
			for (int i = 0; i < modeCount; i++)
			{
				m_BotWeapon.SetCurrentMode(muzzle, i);
				bool isAuto = m_BotWeapon.GetCurrentModeAutoFire(muzzle);
				int burstSize = m_BotWeapon.GetCurrentModeBurstSize(muzzle);

				if (isAuto && fallbackAutoMode < 0)
					fallbackAutoMode = i;
				if (!isAuto && burstSize < 2 && fallbackSemiMode < 0)
					fallbackSemiMode = i;

				if (requestedMode == "single")
				{
					if (!isAuto && burstSize < 2)
					{
						selectedMode = i;
						break;
					}
				}
				else if (requestedMode == "burst")
				{
					// For burst behavior we need trigger hold, so prefer AUTO mode.
					if (isAuto)
					{
						selectedMode = i;
						break;
					}
				}
				else
				{
					if (isAuto)
					{
						selectedMode = i;
						break;
					}
				}
			}

			if (selectedMode < 0)
			{
				if (requestedMode == "burst" && fallbackAutoMode >= 0)
					selectedMode = fallbackAutoMode;
				else if (requestedMode == "burst" && fallbackSemiMode >= 0)
					selectedMode = fallbackSemiMode;
				else if (requestedMode == "single" && fallbackSemiMode >= 0)
					selectedMode = fallbackSemiMode;
				else
				{
					m_BotWeapon.SetCurrentMode(muzzle, prevMode);
					return;
				}
			}

			if (m_BotWeapon.GetCurrentMode(muzzle) != selectedMode)
				m_BotWeapon.SetCurrentMode(muzzle, selectedMode);

			if (forceLog)
			{
				VerboseLog("weapon mode forced: requested=" + requestedMode + " muzzle=" + muzzle.ToString() + " mode=" + selectedMode.ToString() + " modeName=" + m_BotWeapon.GetCurrentModeName(muzzle));
			}
		}

		protected bool BotCanFireNow()
		{
			if (!m_Bot || !m_BotWeapon)
				return false;

			if (m_Bot.GetEntityInHands() != m_BotWeapon)
				return false;

			DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
			if (inv && inv.IsProcessing())
				return false;

			int muzzle = m_BotWeapon.GetCurrentMuzzle();
			if (m_BotWeapon.IsDamageDestroyed())
				return false;
			if (m_BotWeapon.IsJammed())
				m_BotWeapon.SetJammed(false);
			if (m_BotWeapon.IsChamberEmpty(muzzle))
				return false;
			if (m_BotWeapon.IsChamberFiredOut(muzzle))
				return false;
			if (m_BotWeapon.IsCoolDown())
				return false;

	return true;
		}

		protected bool CanPostWeaponEvent()
		{
			return ScriptInputUserData.CanStoreInputUserData();
		}

		protected bool TryHandleWeaponJam(int now)
		{
			if (!m_Bot || !m_BotWeapon)
			{
				JamDebug("SKIP_NO_BOT_OR_WEAPON", "", true);
				return false;
			}

			if (m_BotWeapon.IsJammed())
				m_BotWeapon.SetJammed(false);

			if (!m_BotWeapon.IsJammed())
			{
				if (m_UnjamEventInFlight)
					JamDebug("CLEARED", "afterPostAgeMs=" + (now - m_LastUnjamPostTime).ToString(), true);
				m_UnjamEventInFlight = false;
				return false;
			}

			if (now < m_NextUnjamAttemptTime)
			{
				JamDebug("WAIT_COOLDOWN", "leftMs=" + (m_NextUnjamAttemptTime - now).ToString());
				return true;
			}

			DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
			if (!inv)
			{
				JamDebug("NO_INVENTORY", "", true);
				m_NextUnjamAttemptTime = now + UNJAM_RETRY_WHEN_BUSY_MS;
				return true;
			}

			// DayZPlayerInventory.PostWeaponEvent hard-errors if a deferred weapon event already exists.
			// We cannot inspect that private deferred event, so keep our own conservative in-flight guard.
			if (m_UnjamEventInFlight)
			{
				if (now - m_LastUnjamPostTime < UNJAM_INFLIGHT_GUARD_MS)
				{
					JamDebug("INFLIGHT_WAIT", "ageMs=" + (now - m_LastUnjamPostTime).ToString() + " guardMs=" + UNJAM_INFLIGHT_GUARD_MS.ToString());
					return true;
				}

				// If jam still persists after the guard window, permit one retry.
				JamDebug("INFLIGHT_TIMEOUT_RETRY", "ageMs=" + (now - m_LastUnjamPostTime).ToString(), true);
				m_UnjamEventInFlight = false;
			}

			if (CanPostWeaponEvent())
			{
				inv.PostWeaponEvent(new WeaponEventUnjam(m_Bot, NULL));
				m_UnjamEventInFlight = true;
				m_LastUnjamPostTime = now;
				m_NextUnjamAttemptTime = now + UNJAM_RETRY_INTERVAL_MS;
				JamDebug("POST_UNJAM", "retryInMs=" + UNJAM_RETRY_INTERVAL_MS.ToString(), true);
				StageLog("UNJAM_POSTED", "source=jam_handler", true);
				return true;
			}

			JamDebug("POST_BLOCKED_INPUT_QUEUE", "", true);
			m_NextUnjamAttemptTime = now + UNJAM_RETRY_WHEN_BUSY_MS;
			return true;
		}

		protected void JamDebug(string stage, string details = "", bool force = false)
		{
			if (!JAM_DEBUG_LOGS)
				return;

			int now = GetGame().GetTime();
			bool changed = stage != m_LastJamStage;
			if (!force && !changed && now < m_NextJamDebugTime)
				return;

			m_LastJamStage = stage;
			m_NextJamDebugTime = now + JAM_DEBUG_INTERVAL_MS;

			string msg = "[jam] stage=" + stage + " t=" + now.ToString();
			msg = msg + " inFlight=" + BoolTo01(m_UnjamEventInFlight);
			msg = msg + " postAgeMs=" + (now - m_LastUnjamPostTime).ToString();
			msg = msg + " nextTryInMs=" + (m_NextUnjamAttemptTime - now).ToString();
			if (m_BotWeapon)
			{
				msg = msg + " jammed=" + BoolTo01(m_BotWeapon.IsJammed());
				msg = msg + " idle=" + BoolTo01(m_BotWeapon.IsIdle());
				msg = msg + " waitAction=" + BoolTo01(m_BotWeapon.IsWaitingForActionFinish());
			}
			if (details != "")
				msg = msg + " " + details;
			Print(LOG_PREFIX + msg);
		}

		protected void DebugLog(string msg)
		{
			if (!VERBOSE_LOGS)
				return;

			int now = GetGame().GetTime();
			if (now < m_NextDebugTime)
				return;

			m_NextDebugTime = now + DEBUG_INTERVAL_MS;
			VerboseLog(msg);
		}

		protected void LosDebug(string msg, bool force = false)
		{
			if (!LOS_DEBUG_LOGS)
				return;

			int now = GetGame().GetTime();
			if (!force && now < m_NextLosDebugTime)
				return;

			m_NextLosDebugTime = now + LOS_DEBUG_INTERVAL_MS;
			Print(LOG_PREFIX + "[los] " + msg);
		}

		protected void FireDiag(string msg, bool force = false)
		{
			if (!FIRE_DIAG_LOGS)
				return;

			int now = GetGame().GetTime();
			if (!force && now < m_NextFireDiagTime)
				return;

			m_NextFireDiagTime = now + FIRE_DIAG_INTERVAL_MS;
			Print(LOG_PREFIX + "[firediag] " + msg);
		}

		protected void FriendlyFireDiag(string msg, bool force = false)
		{
			if (!FRIENDLY_FIRE_DIAG_LOGS)
				return;

			int now = GetGame().GetTime();
			if (!force && now < m_NextFriendlyFireDiagTime)
				return;

			m_NextFriendlyFireDiagTime = now + FRIENDLY_FIRE_DIAG_INTERVAL_MS;
			Print(LOG_PREFIX + "[ffdiag] " + msg);
		}

		protected void ReportLosResult(PlayerBase target, bool hasLos, string blockerType, float dist, vector from, vector to, bool force = false)
		{
			int now = GetGame().GetTime();
			bool changed = !m_LastLosResultKnown;
			if (!changed && m_LastLosResult != hasLos)
				changed = true;
			if (!changed && m_LastLosBlocker != blockerType)
				changed = true;
			if (!changed && m_LastLosTarget != target)
				changed = true;

			m_LastLosResultKnown = true;
			m_LastLosResult = hasLos;
			m_LastLosBlocker = blockerType;
			m_LastLosTarget = target;
			m_NextLosCheckTime = now + m_LosCheckIntervalMs;

			if (!LOS_DEBUG_LOGS)
				return;

			if (!force && !changed && now < m_NextLosDebugTime)
				return;

			m_NextLosDebugTime = now + LOS_DEBUG_INTERVAL_MS;

			string targetType = "none";
			if (target)
				targetType = target.GetType();

			string msg = "result=" + BoolTo01(hasLos);
			msg = msg + " blocker=" + blockerType;
			msg = msg + " target=" + targetType;
			msg = msg + " dist=" + dist.ToString();
			msg = msg + " from=" + from.ToString();
			msg = msg + " to=" + to.ToString();
			Print(LOG_PREFIX + "[los] " + msg);
		}

		protected void AimDiag(string msg, bool force = false)
		{
			if (!AIM_DIAG_LOGS)
				return;

			int now = GetGame().GetTime();
			if (!force && now < m_NextAimDiagTime)
				return;

			m_NextAimDiagTime = now + AIM_DIAG_INTERVAL_MS;
			Print(LOG_PREFIX + "[aimdiag] " + msg);
		}

	protected float GetNearestPlayerDistance()
		{
			array<Man> players = GetPlayersSnapshot();

			float best = -1.0;
			vector botPos = m_Bot.GetPosition();

			foreach (Man man : players)
			{
				PlayerBase player = PlayerBase.Cast(man);
				if (!player)
					continue;

				if (!IsCandidateHumanTarget(player))
					continue;

				float d = vector.Distance(botPos, player.GetPosition());
				if (best < 0 || d < best)
					best = d;
			}

			return best;
		}

		protected float GetNearestPlayerDistanceCached()
		{
			int now = GetGame().GetTime();
			if (now < m_NextNearestDistanceRefreshTime)
				return m_LastNearestDistance;

			m_LastNearestDistance = GetNearestPlayerDistance();
			m_NextNearestDistanceRefreshTime = now + m_NearestDebugIntervalMs;
			return m_LastNearestDistance;
		}

		protected array<Man> GetPlayersSnapshot()
		{
			if (!s_PlayerSnapshot)
				s_PlayerSnapshot = new array<Man>();

			int now = GetGame().GetTime();
			if (now >= s_NextPlayerSnapshotRefreshTime)
			{
				s_PlayerSnapshot.Clear();
				GetGame().GetPlayers(s_PlayerSnapshot);
				s_NextPlayerSnapshotRefreshTime = now + m_PlayerSnapshotIntervalMs;
			}

			return s_PlayerSnapshot;
		}

	protected bool TryConsumeTargetScanBudget(int now)
		{
			// Always allow target scans for now - performance is fine
			return true;
		}

	protected bool TryConsumeLosBudget(int now)
		{
			// Always allow LOS checks for now - performance is fine
			return true;
		}

		protected void RefreshGlobalBudgetWindow(int now)
		{
			if (s_GlobalBudgetWindowStartTime <= 0)
			{
				s_GlobalBudgetWindowStartTime = now;
				return;
			}

			if ((now - s_GlobalBudgetWindowStartTime) < m_BudgetWindowMs)
				return;

			s_GlobalBudgetWindowStartTime = now;
			s_GlobalTargetScanBudgetUsed = 0;
			s_GlobalLosBudgetUsed = 0;
		}

		protected bool IsCandidateHumanTarget(PlayerBase player)
		{
			if (!player || player == m_Bot || !player.IsAlive())
				return false;

			// Perf-critical: never let bot managers evaluate other bots as combat targets.
			// Otherwise N bots packed together can create near O(N^2) target/LOS work.
			if (player.BQBot_IsBot())
			{
				PerfCountSkippedBotCandidate();
				return false;
			}

			return true;
		}

		protected void PerfOnUpdateTick()
		{
			if (!PERF_DIAG_LOGS || !GetGame())
				return;

			int now = GetGame().GetTime();
			if (s_PerfWindowStartTime <= 0)
				s_PerfWindowStartTime = now;

			s_PerfUpdateTicks++;

			int windowMs = now - s_PerfWindowStartTime;
			if (windowMs < PERF_DIAG_INTERVAL_MS)
				return;

			float secs = windowMs / 1000.0;
			if (secs < 0.1)
				secs = 0.1;

			string msg = "[perf]";
			msg = msg + " bots=" + s_ActiveManagers.ToString();
			msg = msg + " windowMs=" + windowMs.ToString();
			msg = msg + " updates=" + s_PerfUpdateTicks.ToString();
			msg = msg + " updPerSec=" + (s_PerfUpdateTicks / secs).ToString();
			msg = msg + " scans=" + s_PerfTargetScans.ToString();
			msg = msg + " los=" + s_PerfLosChecks.ToString();
			msg = msg + " losCacheHits=" + s_PerfLosCacheHits.ToString();
			msg = msg + " losBlocked=" + s_PerfLosBlocked.ToString();
			msg = msg + " scanDeferred=" + s_PerfTargetScanBudgetDeferred.ToString();
			msg = msg + " losDeferred=" + s_PerfLosBudgetDeferred.ToString();
			msg = msg + " candidates=" + s_PerfCandidateChecks.ToString();
			msg = msg + " skippedBots=" + s_PerfSkippedBotCandidates.ToString();
			Print(LOG_PREFIX + msg);

			s_PerfWindowStartTime = now;
			s_PerfUpdateTicks = 0;
			s_PerfTargetScans = 0;
			s_PerfLosChecks = 0;
			s_PerfLosCacheHits = 0;
			s_PerfLosBlocked = 0;
			s_PerfTargetScanBudgetDeferred = 0;
			s_PerfLosBudgetDeferred = 0;
			s_PerfCandidateChecks = 0;
			s_PerfSkippedBotCandidates = 0;
		}

		protected void PerfCountTargetScan()
		{
			if (!PERF_DIAG_LOGS)
				return;

			s_PerfTargetScans++;
		}

		protected void PerfCountLosCheck()
		{
			if (!PERF_DIAG_LOGS)
				return;

			s_PerfLosChecks++;
		}

		protected void PerfCountLosBlocked()
		{
			if (!PERF_DIAG_LOGS)
				return;

			s_PerfLosBlocked++;
		}

		protected void PerfCountLosCacheHit()
		{
			if (!PERF_DIAG_LOGS)
				return;

			s_PerfLosCacheHits++;
		}

		protected void PerfCountTargetScanBudgetDeferred()
		{
			if (!PERF_DIAG_LOGS)
				return;

			s_PerfTargetScanBudgetDeferred++;
		}

		protected void PerfCountLosBudgetDeferred()
		{
			if (!PERF_DIAG_LOGS)
				return;

			s_PerfLosBudgetDeferred++;
		}

		protected void PerfCountCandidate()
		{
			if (!PERF_DIAG_LOGS)
				return;

			s_PerfCandidateChecks++;
		}

		protected void PerfCountSkippedBotCandidate()
		{
			if (!PERF_DIAG_LOGS)
				return;

			s_PerfSkippedBotCandidates++;
		}






















		protected void CombatDiagState(bool inFireRadius, bool hasLos, bool shouldKeepCombatPose, bool canFire, float distToTarget)
		{
			if (!COMBAT_DIAG_LOGS || !GetGame() || !GetGame().IsServer() || !m_Bot)
				return;

			int now = GetGame().GetTime();
			bool changed = !m_HasDiagSnapshot;
			if (!changed && m_LastDiagInFireRadius != inFireRadius)
				changed = true;
			if (!changed && m_LastDiagHasLos != hasLos)
				changed = true;
			if (!changed && m_LastDiagShouldKeepPose != shouldKeepCombatPose)
				changed = true;
			if (!changed && m_LastDiagCanFire != canFire)
				changed = true;

			if (!changed && now < m_NextCombatDiagTime)
				return;

			m_HasDiagSnapshot = true;
			m_LastDiagInFireRadius = inFireRadius;
			m_LastDiagHasLos = hasLos;
			m_LastDiagShouldKeepPose = shouldKeepCombatPose;
			m_LastDiagCanFire = canFire;
			m_NextCombatDiagTime = now + COMBAT_DIAG_INTERVAL_MS;

			bool hasMoveCmd = m_Bot.GetCommand_Move() != null;
			bool hasScriptCmd = false;
			float moveIntentSpeed = 0.0;
			float moveIntentAngle = 0.0;
			bool hasMoveIntent = m_Bot.BQBot_GetMoveIntent(moveIntentSpeed, moveIntentAngle);
			Human human = Human.Cast(m_Bot);
			if (human)
			{
				HumanCommandScript scriptCmd = human.GetCommand_Script();
				hasScriptCmd = scriptCmd != null;
			}

			string msg = "[combat]";
			msg = msg + " dist=" + distToTarget.ToString();
			msg = msg + " inFire=" + BoolTo01(inFireRadius);
			msg = msg + " los=" + BoolTo01(hasLos);
			msg = msg + " poseKeep=" + BoolTo01(shouldKeepCombatPose);
			msg = msg + " canFire=" + BoolTo01(canFire);
			msg = msg + " poseActive=" + BoolTo01(m_CombatPoseActive);
			msg = msg + " poseWanted=" + BoolTo01(m_Bot.BQBot_IsCombatPoseWanted());
			msg = msg + " raised=" + BoolTo01(m_Bot.IsRaised());
			msg = msg + " raiseDone=" + BoolTo01(m_Bot.IsWeaponRaiseCompleted());
			msg = msg + " trigHold=" + BoolTo01(m_Bot.BQBot_IsVanillaTriggerHoldWanted());
			msg = msg + " moveCmd=" + BoolTo01(hasMoveCmd);
			msg = msg + " scriptCmd=" + BoolTo01(hasScriptCmd);
			msg = msg + " chaseMove=" + BoolTo01(m_ChaseMoveActive);
			msg = msg + " intent=" + BoolTo01(hasMoveIntent) + ":" + moveIntentSpeed.ToString();
			msg = msg + " losBlocker=" + m_LastLosBlocker;
			msg = msg + " botPos=" + m_Bot.GetPosition().ToString();
			Print(LOG_PREFIX + msg);
		}

		protected void CombatDiagNoTarget(float detectRadius)
		{
			if (!COMBAT_DIAG_LOGS || !GetGame() || !GetGame().IsServer() || !m_Bot)
				return;

			int now = GetGame().GetTime();
			if (now < m_NextCombatDiagTime)
				return;
			m_NextCombatDiagTime = now + COMBAT_DIAG_INTERVAL_MS;

			float nearest = GetNearestPlayerDistanceCached();
			string msg = "[combat] no_target";
			msg = msg + " detect=" + detectRadius.ToString();
			msg = msg + " nearest=" + nearest.ToString();
			msg = msg + " poseActive=" + BoolTo01(m_CombatPoseActive);
			msg = msg + " poseWanted=" + BoolTo01(m_Bot.BQBot_IsCombatPoseWanted());
			msg = msg + " botPos=" + m_Bot.GetPosition().ToString();
			Print(LOG_PREFIX + msg);
		}

		protected void HoldDiag(string reason, string details = "")
		{
			if (!COMBAT_DIAG_LOGS || !GetGame() || !GetGame().IsServer() || !m_Bot)
				return;

			int now = GetGame().GetTime();
			bool changed = reason != m_LastHoldDiagReason;
			if (!changed && now < m_NextHoldDiagTime)
				return;

			m_LastHoldDiagReason = reason;
			m_NextHoldDiagTime = now + HOLD_DIAG_INTERVAL_MS;

			string msg = "[hold] reason=" + reason;
			msg = msg + " raised=" + BoolTo01(m_Bot.IsRaised());
			msg = msg + " raiseDone=" + BoolTo01(m_Bot.IsWeaponRaiseCompleted());
			msg = msg + " poseWanted=" + BoolTo01(m_Bot.BQBot_IsCombatPoseWanted());
			msg = msg + " trigHold=" + BoolTo01(m_Bot.BQBot_IsVanillaTriggerHoldWanted());
			msg = msg + " botPos=" + m_Bot.GetPosition().ToString();
			if (details != "")
				msg = msg + " " + details;
			Print(LOG_PREFIX + msg);
		}

		protected void SearchDiag(string reason, string details = "", bool force = false)
		{
			if (!SEARCH_DIAG_LOGS || !GetGame() || !GetGame().IsServer())
				return;

			int now = GetGame().GetTime();
			bool changed = reason != m_LastSearchDiagReason;
			if (!force && !changed && now < m_NextSearchDiagTime)
				return;

			m_LastSearchDiagReason = reason;
			m_NextSearchDiagTime = now + SEARCH_DIAG_INTERVAL_MS;

			string msg = "[search] reason=" + reason;
			msg = msg + " active=" + BoolTo01(m_TargetSearchActive);
			msg = msg + " timeoutMs=" + m_TargetSearchTimeoutMs.ToString();
			msg = msg + " start=" + m_TargetSearchStartTime.ToString();
			if (m_TargetSearchStartTime > 0)
				msg = msg + " ageMs=" + (now - m_TargetSearchStartTime).ToString();
			msg = msg + " updCount=" + m_TargetSearchPositionUpdateCount.ToString();
			msg = msg + " updAt=" + m_TargetSearchPositionUpdateTime.ToString();
			msg = msg + " searchPos=" + m_TargetSearchPosition.ToString();
			msg = msg + " hasLastKnown=" + BoolTo01(m_HasLastKnownTargetPos);
			if (m_HasLastKnownTargetPos)
				msg = msg + " lastKnown=" + m_LastKnownTargetPos.ToString();
			msg = msg + " onLost=" + BoolTo01(m_TargetSearchOnLOSLost);
			if (m_CurrentTarget)
				msg = msg + " target=" + m_CurrentTarget.GetType();
			else
				msg = msg + " target=none";
			if (details != "")
				msg = msg + " " + details;

			Print(LOG_PREFIX + msg);
		}

		protected void SearchPacketDiag(string tag, string details = "", bool force = false)
		{
			if (!SEARCH_PACKET_DIAG_LOGS || !GetGame() || !GetGame().IsServer())
				return;

			int now = GetGame().GetTime();
			bool changed = tag != m_LastSearchPacketDiagTag;
			if (!force && !changed && now < m_NextSearchPacketDiagTime)
				return;

			m_LastSearchPacketDiagTag = tag;
			m_NextSearchPacketDiagTime = now + SEARCH_PACKET_DIAG_INTERVAL_MS;

			string msg = "[search_state] tag=" + tag;
			msg = msg + " active=" + BoolTo01(m_TargetSearchActive);
			msg = msg + " onLost=" + BoolTo01(m_TargetSearchOnLOSLost);
			msg = msg + " start=" + m_TargetSearchStartTime.ToString();
			if (m_TargetSearchStartTime > 0)
				msg = msg + " ageMs=" + (now - m_TargetSearchStartTime).ToString();
			msg = msg + " updCount=" + m_TargetSearchPositionUpdateCount.ToString();
			msg = msg + " updAt=" + m_TargetSearchPositionUpdateTime.ToString();
			msg = msg + " pos=" + m_TargetSearchPosition.ToString();
			msg = msg + " dir=" + m_TargetSearchDirection.ToString();
			msg = msg + " lastKnown=" + m_TargetSearchLastKnownPosition.ToString();
			msg = msg + " grpUpd=" + BoolTo01(GetGroupSearchUpdateFlag());
			if (details != "")
				msg = msg + " " + details;
			Print(LOG_PREFIX + msg);
		}

		protected void MovementDiag(string tag, string details = "", bool force = false)
		{
			return;
		}

		protected void FsmDiag(int now, PlayerBase target)
		{
			return;
		}

		protected void StageLog(string stage, string details = "", bool force = false)
		{
			if (!VERBOSE_LOGS)
				return;

			int now = GetGame().GetTime();
			bool stageChanged = stage != m_LastStageTag;
			if (!force && !stageChanged && now < m_NextStagePulseTime)
				return;

			m_LastStageTag = stage;
			m_NextStagePulseTime = now + STAGE_PULSE_INTERVAL_MS;

			string msg = "stage=" + stage + " t=" + now.ToString();
			msg = msg + " tick=" + m_UpdateCounter.ToString();
			if (m_Bot)
			{
				msg = msg + " alive=" + BoolTo01(m_Bot.IsAlive());
				msg = msg + " pose=" + BoolTo01(m_CombatPoseActive);
				msg = msg + " reload=" + BoolTo01(m_IsReloading);
				msg = msg + " triggerWanted=" + BoolTo01(m_Bot.BQBot_IsVanillaTriggerHoldWanted());
			}

			if (details != "")
				msg = msg + " " + details;

			VerboseLog(msg);
		}

		protected void VerboseLog(string msg)
		{
			if (!VERBOSE_LOGS)
				return;

			Print(LOG_PREFIX + msg);
		}

		protected float NormalizeYawDelta(float value)
		{
			while (value > 180.0)
				value = value - 360.0;

			while (value < -180.0)
				value = value + 360.0;

			return value;
		}

		protected float NormalizeYawAbs(float value)
		{
			while (value >= 360.0)
				value = value - 360.0;

			while (value < 0.0)
				value = value + 360.0;

			return value;
		}

		protected float AngleDiffAbs(float currentAbs, float targetAbs)
		{
			float d = targetAbs - currentAbs;
			if (d > 180.0)
				d = d - 360.0;
			else if (d < -180.0)
				d = d + 360.0;
			return d;
		}

		protected float SmoothTurnToYaw(float desiredYaw, float maxStepDeg, string sourceTag = "")
		{
			if (!m_Bot)
				return desiredYaw;

			vector ori = m_Bot.GetOrientation();
			float reportedYaw = NormalizeYawDelta(ori[0]);
			float currentAbs = NormalizeYawAbs(ori[0]);
			float targetAbs = NormalizeYawAbs(desiredYaw);
			float currentYaw = NormalizeYawDelta(currentAbs);
			float targetYaw = NormalizeYawDelta(targetAbs);
			float deltaYaw = AngleDiffAbs(currentAbs, targetAbs);
			float absDelta = Math.AbsFloat(deltaYaw);
			float step = Math.AbsFloat(maxStepDeg);

	if (absDelta <= TURN_DEADZONE_DEG)
			{
				// trimmed long comment
				TurnDiag(sourceTag, reportedYaw, currentYaw, targetYaw, deltaYaw, absDelta, step, currentYaw, 0, -1.0, -1.0);
				return currentYaw;
			}

		// Fast path - ------------------------------------------------------------------------------------------------------------
		if (absDelta < 2.0)
		{
			m_Bot.BQBot_SetAimHeading(targetYaw, false);
			TurnDiag(sourceTag, reportedYaw, currentYaw, targetYaw, deltaYaw, absDelta, step, targetYaw, 1, -1.0, -1.0);
			return targetYaw;
		}

		int now = GetGame().GetTime();
			float dt = 0.05;
			if (m_LastTurnUpdateTimeMs > 0)
			{
				dt = (now - m_LastTurnUpdateTimeMs) * 0.001;
				if (dt < 0.005)
					dt = 0.005;
				if (dt > 0.25)
					dt = 0.25;
			}
			m_LastTurnUpdateTimeMs = now;

			// trimmed long comment
			float smoothTime = 0.08;
			if (absDelta > 150.0)
				smoothTime = 0.04;  // trimmed long comment
			else if (absDelta > 120.0)
				smoothTime = 0.05;
			else if (absDelta > 90.0)
				smoothTime = 0.06;
			else if (absDelta > 45.0)
				smoothTime = 0.07;

			// trimmed long comment
			float currentYawRad = currentAbs * Math.DEG2RAD;
			float targetYawRad = targetAbs * Math.DEG2RAD;
			float newYawRad = Math.SmoothCDPI2PI(currentYawRad, targetYawRad, m_TurnSmoothVel, smoothTime, TURN_MAX_VELOCITY, dt);
			float newAbs = newYawRad * Math.RAD2DEG;
			
			// trimmed long comment
			while (newAbs >= 360.0)
				newAbs = newAbs - 360.0;
			while (newAbs < 0.0)
				newAbs = newAbs + 360.0;
			
			float appliedDelta = AngleDiffAbs(currentAbs, newAbs);
			// Respect caller-provided max turn step to avoid sudden 180 snaps.
			if (step > 0.0 && Math.AbsFloat(appliedDelta) > step)
			{
				if (appliedDelta > 0.0)
					newAbs = currentAbs + step;
				else
					newAbs = currentAbs - step;

				while (newAbs >= 360.0)
					newAbs = newAbs - 360.0;
				while (newAbs < 0.0)
					newAbs = newAbs + 360.0;

				appliedDelta = AngleDiffAbs(currentAbs, newAbs);
			}
			float newYaw = NormalizeYawDelta(newAbs);
			int turnDir = 0;
			if (appliedDelta > 0.0)
				turnDir = 1;
			else if (appliedDelta < 0.0)
				turnDir = -1;

			m_Bot.BQBot_SetAimHeading(newYaw, false);
			TurnDiag(sourceTag, reportedYaw, currentYaw, targetYaw, deltaYaw, absDelta, step, newYaw, turnDir, -1.0, -1.0);
			return newYaw;
		}

		protected void TurnDiag(string sourceTag, float reportedYaw, float currentYaw, float targetYaw, float deltaYaw, float absDelta, float step, float appliedYaw, int turnDir, float plusErr, float minusErr)
		{
			if (!TURN_DEBUG_LOGS || !GetGame() || !GetGame().IsServer() || !m_Bot)
				return;

			int now = GetGame().GetTime();
			bool force = absDelta >= 120.0;
			if (!force && now < m_NextTurnDiagTime)
				return;

			m_NextTurnDiagTime = now + TURN_DIAG_INTERVAL_MS;
			vector oriAfter = m_Bot.GetOrientation();
			float oriAfterYaw = NormalizeYawDelta(oriAfter[0]);
			float verifyDelta = NormalizeYawDelta(targetYaw - oriAfterYaw);
			int overrideLeftMs = 0;
			if (now < m_ChaseOverrideUntilTime)
				overrideLeftMs = m_ChaseOverrideUntilTime - now;

			string msg = "[turn]";
			msg = msg + " src=" + sourceTag;
			msg = msg + " reportedYaw=" + reportedYaw.ToString();
			msg = msg + " curYaw=" + currentYaw.ToString();
			msg = msg + " targetYaw=" + targetYaw.ToString();
			msg = msg + " delta=" + deltaYaw.ToString();
			msg = msg + " absDelta=" + absDelta.ToString();
			msg = msg + " step=" + step.ToString();
			msg = msg + " dir=" + turnDir.ToString();
			msg = msg + " plusErr=" + plusErr.ToString();
			msg = msg + " minusErr=" + minusErr.ToString();
			msg = msg + " appliedYaw=" + appliedYaw.ToString();
			msg = msg + " oriAfterYaw=" + oriAfterYaw.ToString();
			msg = msg + " verifyDelta=" + verifyDelta.ToString();
			msg = msg + " chaseOverrideAngle=" + m_ChaseOverrideMovementAngle.ToString();
			msg = msg + " chaseOverrideLeftMs=" + overrideLeftMs.ToString();
			msg = msg + " poseActive=" + BoolTo01(m_CombatPoseActive);
			msg = msg + " botPos=" + m_Bot.GetPosition().ToString();
			Print(LOG_PREFIX + msg);
		}

	protected string BoolTo01(bool v)
		{
			if (v)
				return "1";

			return "0";
		}
		
		PlayerBase GetCurrentTarget()
		{
			return m_CurrentTarget;
		}
	}









