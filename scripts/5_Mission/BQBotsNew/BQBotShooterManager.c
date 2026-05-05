/*
    BQBotShooterManager

    Slim backend version.
    Expansion/eAI owns movement, target acquisition, flank, cover, formation FSM and pathfinding.
    This manager only does:
      - spawn/config/loadout wiring;
      - bot weapon/equipment preparation;
      - magazine reserve + vanilla reload events;
      - weapon mode/chamber/jam maintenance;
      - mirroring current Expansion target into BQ firebridge context.
*/
class BQBotShooterManager
{
    protected const string LOG_PREFIX = "[BQBots] [ShooterBackend] ";
    protected const bool VERBOSE_LOGS = false;
    protected const bool RELOAD_DEBUG_LOGS = false;
    protected const bool WEAPON_TRACE_LOGS = false;

    protected const int BOT_INIT_STAGGER_MS = 500;
    protected const int MAX_CONCURRENT_RELOADS = 100;
    protected const int RELOAD_QUEUE_CHECK_MS = 300;
    protected const int POST_RELOAD_SYNC_DELAY_MS = 100;
    protected const int UNJAM_RETRY_INTERVAL_MS = 450;
    protected const int UNJAM_RETRY_WHEN_BUSY_MS = 150;
    protected const int CHAMBER_INFLIGHT_GUARD_MS = 1800;
    protected const int CHAMBER_RETRY_WHEN_BUSY_MS = 120;

    protected const int FIRE_INTENT_MODE_AUTO = 0;
    protected const int FIRE_INTENT_MODE_BURST = 1;
    protected const int FIRE_INTENT_MODE_SINGLE = 2;

    protected static ref array<ref BQBotSpawnConfig> s_QueuedSpawnOverrides;
    protected static int s_NextQueuedSpawnOverrideIndex;
    protected static ref array<ref BQBotSettings> s_QueuedSettingsOverrides;
    protected static int s_NextQueuedSettingsOverrideIndex;
    protected static int s_BotInitStagger;
    protected static int s_ActiveReloadOperations;

    // Only spawn-time grouping. No tactical steering here.
    protected static ref array<string> s_ExpansionGroupIds;
    protected static ref array<ref eAIGroup> s_ExpansionGroups;

    protected ref BQBotSettings m_Settings;
    protected bqBots_Boris m_Bot;
    protected Weapon_Base m_BotWeapon;
    protected EntityAI m_LastHandsEntity;
    protected Magazine m_ReserveMag;
    protected PlayerBase m_CurrentTarget;

    protected int m_UpdateIntervalMs;
    protected int m_MaintenanceIntervalMs;
    protected int m_NextMaintenanceTime;
    protected int m_NextReloadAttemptTime;
    protected int m_NextReloadDebugTime;
    protected int m_NextWeaponTraceTime;
    protected int m_PostReloadSyncUntilTime;
    protected int m_NextUnjamAttemptTime;
    protected int m_NextChamberAttemptTime;
    protected int m_LastChamberPostTime;

    protected bool m_IsReloading;
    protected int m_ReloadStartTime;
    protected Magazine m_ReloadPrevCurrentMag;
    protected Magazine m_ReloadIncomingMag;
    protected bool m_ChamberEventInFlight;

    protected float m_MaxFlankingDistance;
    protected bool m_EnableFlankingOutsideCombat;

    void BQBotShooterManager()
    {
        if (!GetGame().IsServer())
            return;

        LoadSettings();

        int staggerDelay = s_BotInitStagger;
        s_BotInitStagger = s_BotInitStagger + BOT_INIT_STAGGER_MS;
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpawnBotDelayed, staggerDelay, false);
    }

    void ~BQBotShooterManager()
    {
        if (GetGame())
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Update);
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
        s_NextQueuedSpawnOverrideIndex = 0;
        s_NextQueuedSettingsOverrideIndex = 0;
        s_ExpansionGroupIds = new array<string>();
        s_ExpansionGroups = new array<ref eAIGroup>();
    }

    protected void SpawnBotDelayed()
    {
        SpawnBotInternal();
    }

    protected void SpawnBotInternal()
    {
        SpawnBot();
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
            Print(LOG_PREFIX + "spawn failed: object is not bqBots_Boris, class=" + botClass);
            return;
        }

        m_Bot.SetPosition(spawnPos);
        m_Bot.SetOrientation(spawnOri);
        m_Bot.SetAllowDamage(GetSpawnAllowDamage());
        m_Bot.BQBot_SetIsBot(true);
        m_Bot.BQBot_SetFireIntentMode(GetFireIntentModeId());
        m_Bot.BQBot_ResetSurvivalInitFlag();
        m_Bot.BQBot_ApplySurvivalDisabledOnce();

        ApplyAccuracyProfile();
        ApplyDamageProfile();
        SyncExpansionRuntimeToBot();
        ConfigureExpansionGroupOnSpawn();
        ForceStandPose();
        ApplyConfiguredClothing();
        EnsureBotStorage();

        EntityAI inHands = m_Bot.GetHumanInventory().CreateInHands(GetWeaponType());
        m_BotWeapon = Weapon_Base.Cast(inHands);
        m_LastHandsEntity = inHands;

        m_CurrentTarget = null;
        m_ReserveMag = null;
        m_IsReloading = false;
        m_ReloadStartTime = 0;
        m_ReloadPrevCurrentMag = null;
        m_ReloadIncomingMag = null;
        m_ChamberEventInFlight = false;
        m_NextMaintenanceTime = 0;
        m_NextReloadAttemptTime = 0;
        m_PostReloadSyncUntilTime = 0;
        m_NextChamberAttemptTime = 0;

        EnsureWeaponReady(true);
        EnsureAutoFireMode(true);

        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Update, m_UpdateIntervalMs, true);
    }

    protected void LoadSettings()
    {
        m_Settings = new BQBotSettings();
        ApplyQueuedSettingsOverride();

        if (!m_Settings.m_Spawn)
            m_Settings.m_Spawn = new BQBotSpawnConfig();
        if (!m_Settings.m_Weapon)
            m_Settings.m_Weapon = new BQBotWeaponConfig();
        if (!m_Settings.m_Clothing)
            m_Settings.m_Clothing = new BQBotClothingConfig();
        if (!m_Settings.m_Runtime)
            m_Settings.m_Runtime = new BQBotRuntimeConfig();

        if (m_Settings.m_Spawn.m_Classname == "" || m_Settings.m_Spawn.m_Classname == "SurvivorM_Mirek" || m_Settings.m_Spawn.m_Classname == "BQBots_Mirek" || m_Settings.m_Spawn.m_Classname == "bqBots_Mirek")
            m_Settings.m_Spawn.m_Classname = "bqBots_Boris_AI";

        BQBotRuntimeConfig rt = m_Settings.m_Runtime;
        m_UpdateIntervalMs = rt.m_UpdateIntervalMs;
        if (m_UpdateIntervalMs < 50)
            m_UpdateIntervalMs = 50;
        if (m_UpdateIntervalMs > 250)
            m_UpdateIntervalMs = 100;

        m_MaintenanceIntervalMs = rt.m_MaintenanceIntervalMs;
        if (m_MaintenanceIntervalMs < 100)
            m_MaintenanceIntervalMs = 100;
        if (m_MaintenanceIntervalMs > 5000)
            m_MaintenanceIntervalMs = 800;

        m_MaxFlankingDistance = rt.m_MaxFlankingDistance;
        if (m_MaxFlankingDistance <= 0.0)
            m_MaxFlankingDistance = 200.0;
        m_MaxFlankingDistance = Math.Clamp(m_MaxFlankingDistance, 6.0, 200.0);

        m_EnableFlankingOutsideCombat = rt.m_EnableFlankingOutsideCombat;

        if (m_Settings.m_Runtime.m_AccuracyBase < 0.0)
            m_Settings.m_Runtime.m_AccuracyBase = 0.0;
        if (m_Settings.m_Runtime.m_AccuracyBase > 1.0)
            m_Settings.m_Runtime.m_AccuracyBase = 1.0;
        if (m_Settings.m_Runtime.m_AccuracyNearDistance < 1.0)
            m_Settings.m_Runtime.m_AccuracyNearDistance = 1.0;
        if (m_Settings.m_Runtime.m_AccuracyFarDistance < (m_Settings.m_Runtime.m_AccuracyNearDistance + 1.0))
            m_Settings.m_Runtime.m_AccuracyFarDistance = m_Settings.m_Runtime.m_AccuracyNearDistance + 1.0;
        m_Settings.m_Runtime.m_AccuracyNearFactor = Math.Clamp(m_Settings.m_Runtime.m_AccuracyNearFactor, 0.0, 1.0);
        m_Settings.m_Runtime.m_AccuracyFarFactor = Math.Clamp(m_Settings.m_Runtime.m_AccuracyFarFactor, 0.0, 1.0);
        m_Settings.m_Runtime.m_AccuracyMaxSpreadDeg = Math.Clamp(m_Settings.m_Runtime.m_AccuracyMaxSpreadDeg, 0.0, 45.0);
        m_Settings.m_DamageMultiplier = Math.Clamp(m_Settings.m_DamageMultiplier, 0.0, 10.0);

        ApplyQueuedSpawnOverride();
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
        if (queued)
            m_Settings = queued;
    }

    protected vector ResolveGroundSpawnPosition(vector requestedPos)
    {
        float surfaceY = GetGame().SurfaceY(requestedPos[0], requestedPos[2]);
        return Vector(requestedPos[0], surfaceY, requestedPos[2]);
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
        if (m_Bot && m_Settings)
            m_Bot.BQBot_SetDamageMultiplier(m_Settings.m_DamageMultiplier);
    }

    protected void SyncExpansionRuntimeToBot()
    {
        if (!m_Bot)
            return;

        m_Bot.eAI_SetMaxFlankingDistance(m_MaxFlankingDistance);
        if (m_EnableFlankingOutsideCombat)
            m_Bot.eAI_SetEnableFlankingOutsideCombat(1);
        else
            m_Bot.eAI_SetEnableFlankingOutsideCombat(0);
    }

    protected void ConfigureExpansionGroupOnSpawn()
    {
        if (!m_Bot || !m_Settings || !m_Settings.m_Spawn)
            return;

        string groupId = m_Settings.m_Spawn.m_GroupId;
        if (groupId == "")
            groupId = TryGetGroupIdFromZoneId(m_Settings.m_Spawn.m_ZoneId);

        eAIGroup group;
        if (groupId != "")
            group = GetOrCreateExpansionSpawnGroup(groupId);
        else
            group = eAIGroup.GetGroupByLeader(m_Bot, true);

        if (!group)
            return;

        if (m_Bot.GetGroup() != group)
            m_Bot.SetGroup(group);

        ConfigureExpansionFormation(group);
    }

    protected string TryGetGroupIdFromZoneId(string zoneId)
    {
        if (zoneId == "")
            return "";

        TStringArray parts = {};
        zoneId.Split(".", parts);
        if (parts.Count() >= 2)
            return parts.Get(0);

        return "";
    }

    protected static eAIGroup GetOrCreateExpansionSpawnGroup(string groupId)
    {
        if (groupId == "")
            return null;

        if (!s_ExpansionGroupIds)
            s_ExpansionGroupIds = new array<string>();
        if (!s_ExpansionGroups)
            s_ExpansionGroups = new array<ref eAIGroup>();

        int idx = s_ExpansionGroupIds.Find(groupId);
        if (idx >= 0 && idx < s_ExpansionGroups.Count())
        {
            eAIGroup existing = s_ExpansionGroups.Get(idx);
            if (existing)
                return existing;
        }

        eAIGroup created = eAIGroup.CreateGroup();
        if (created)
        {
            created.SetName(groupId);
            s_ExpansionGroupIds.Insert(groupId);
            s_ExpansionGroups.Insert(created);
        }

        return created;
    }

    protected void ConfigureExpansionFormation(eAIGroup group)
    {
        if (!group || !m_Settings || !m_Settings.m_Spawn)
            return;

        string formation = m_Settings.m_Spawn.m_Formation;
        formation.ToLower();

        eAIFormation form;
        if (formation == "column")
            form = new eAIFormationColumn(group);
        else if (formation == "invcolumn")
            form = new eAIFormationInvColumn(group);
        else if (formation == "file")
            form = new eAIFormationFile(group);
        else if (formation == "invfile")
            form = new eAIFormationInvFile(group);
        else if (formation == "invvee")
            form = new eAIFormationInvVee(group);
        else if (formation == "wall" || formation == "line")
            form = new eAIFormationWall(group);
        else if (formation == "circle")
            form = new eAIFormationCircle(group);
        else if (formation == "circledot")
            form = new eAIFormationCircleDot(group);
        else if (formation == "star")
            form = new eAIFormationStar(group);
        else if (formation == "stardot")
            form = new eAIFormationStarDot(group);
        else
            form = new eAIFormationVee(group);

        float scale = m_Settings.m_Spawn.m_FormationScale;
        if (scale <= 0.0)
            scale = 1.0;
        form.SetScale(Math.Clamp(scale, 0.5, 4.0));

        float looseness = m_Settings.m_Spawn.m_FormationLooseness;
        if (formation == "loose" && looseness < 2.0)
            looseness = 2.0;
        form.SetLooseness(Math.Clamp(looseness, 0.0, 3.0));

        group.SetFormation(form);
    }

    protected void Update()
    {
        if (!m_Bot)
            return;

        if (!m_Bot.IsAlive())
        {
            m_Bot.BQBot_SetCombatTargetContext(null, 0.0, 0.0);
            m_Bot.BQBot_SetVanillaTriggerHoldWanted(false);
            m_Bot.BQBot_SetCombatPoseWanted(false);
            m_CurrentTarget = null;
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Update);
            return;
        }

        RefreshWeaponStateFromHands();
        if (!m_BotWeapon)
            return;

        m_Bot.BQBot_SetFireIntentMode(GetFireIntentModeId());
        FinalizeReloadState();
        EnsureWeaponReady(false);
        HandleReload();

        if (m_Bot.BQBot_ConsumePostReloadSyncNeeded())
            ArmPostReloadSync("expansion_reload_state", true);

        int now = GetGame().GetTime();
        if (TryHandleWeaponJam(now))
        {
            m_Bot.BQBot_SetVanillaTriggerHoldWanted(false);
            return;
        }

        eAITarget eaiTarget = m_Bot.GetTarget();
        EntityAI targetEntity;
        PlayerBase playerTarget;
        if (eaiTarget)
        {
            targetEntity = eaiTarget.GetEntity();
            Class.CastTo(playerTarget, targetEntity);
        }

        m_CurrentTarget = playerTarget;

        if (!eaiTarget || !targetEntity)
        {
            m_Bot.BQBot_SetCombatTargetContext(null, 0.0, 0.0);
            m_Bot.BQBot_SetVanillaTriggerHoldWanted(false);
            return;
        }

        if (playerTarget)
            m_Bot.BQBot_SetCombatTargetContext(playerTarget, m_Bot.GetThreatToSelf(false), m_Bot.GetThreatToSelf(true));
        else
            m_Bot.BQBot_SetCombatTargetContext(null, m_Bot.GetThreatToSelf(false), m_Bot.GetThreatToSelf(true));

        vector aimPos = eaiTarget.GetPosition(false) + eaiTarget.GetAimOffset();
        m_Bot.BQBot_SetAimTarget(aimPos);
        AimPitchOnly(aimPos);
        EnsureRoundChambered("target_context");
    }

    protected void RefreshWeaponStateFromHands()
    {
        EntityAI hands = m_Bot.GetEntityInHands();
        if (hands == m_LastHandsEntity)
            return;

        m_LastHandsEntity = hands;
        m_BotWeapon = Weapon_Base.Cast(hands);
        if (m_BotWeapon)
            EnsureAutoFireMode(true);
    }

    protected void EnsureWeaponReady(bool force)
    {
        if (!m_Bot || !m_BotWeapon)
            return;

        int now = GetGame().GetTime();
        if (!force && now < m_NextMaintenanceTime)
            return;

        int muzzle = m_BotWeapon.GetCurrentMuzzle();
        Magazine currentMag = Magazine.Cast(m_BotWeapon.GetMagazine(muzzle));

        EnsureReserveMagazine(currentMag);
        if (m_ReserveMag && m_ReserveMag.GetAmmoCount() < GetReserveMagAmmo())
            m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());

        if (!currentMag)
        {
            TryAttachMagazineFromReserve("ready_no_mag");
            m_NextMaintenanceTime = now + m_MaintenanceIntervalMs;
            return;
        }

        if (force && (m_BotWeapon.IsChamberEmpty(muzzle) || m_BotWeapon.IsChamberFiredOut(muzzle)) && currentMag.GetAmmoCount() > 0)
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
                m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());
        }

        PruneExtraReserveMagazines(m_ReserveMag, currentMag);
    }

    protected void HandleReload()
    {
        if (!m_Bot || !m_BotWeapon)
            return;

        int now = GetGame().GetTime();
        if (now < m_NextReloadAttemptTime)
            return;

        if (m_IsReloading)
            return;

        if (s_ActiveReloadOperations >= MAX_CONCURRENT_RELOADS)
        {
            m_NextReloadAttemptTime = now + RELOAD_QUEUE_CHECK_MS;
            return;
        }

        int muzzle = m_BotWeapon.GetCurrentMuzzle();
        Magazine currentMag = Magazine.Cast(m_BotWeapon.GetMagazine(muzzle));
        if (currentMag && currentMag.GetAmmoCount() > 0)
            return;

        EnsureReserveMagazine(currentMag);
        if (!m_ReserveMag)
            return;

        if (m_ReserveMag.GetAmmoCount() < GetReserveMagAmmo())
            m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());

        DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
        if (!inv || inv.IsProcessing())
            return;

        if (!m_BotWeapon.IsIdle() || m_BotWeapon.IsWaitingForActionFinish())
            return;

        if (currentMag)
            TrySwapMagazineFromReserve(currentMag);
        else
            TryAttachMagazineFromReserve("reload_no_mag");
    }

    protected void TryAttachMagazineFromReserve(string source)
    {
        if (!m_Bot || !m_BotWeapon || !m_ReserveMag)
            return;

        DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
        WeaponManager wm = m_Bot.GetWeaponManager();
        if (!inv || !wm || inv.IsProcessing())
            return;

        if (!m_BotWeapon.IsIdle() || m_BotWeapon.IsWaitingForActionFinish())
            return;

        if (!wm.CanAttachMagazine(m_BotWeapon, m_ReserveMag, false))
            return;

        if (!CanPostWeaponEvent())
        {
            m_NextReloadAttemptTime = GetGame().GetTime() + 200;
            return;
        }

        inv.PostWeaponEvent(new WeaponEventAttachMagazine(m_Bot, m_ReserveMag));
        m_IsReloading = true;
        s_ActiveReloadOperations++;
        m_ReloadStartTime = GetGame().GetTime();
        m_ReloadPrevCurrentMag = null;
        m_ReloadIncomingMag = m_ReserveMag;
        m_NextReloadAttemptTime = m_ReloadStartTime + 1400;
        WeaponTrace("attach_mag", source + " reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag), true);
    }

    protected void TrySwapMagazineFromReserve(Magazine currentMag)
    {
        if (!m_Bot || !m_BotWeapon || !currentMag || !m_ReserveMag)
            return;

        DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
        WeaponManager wm = m_Bot.GetWeaponManager();
        if (!inv || !wm || inv.IsProcessing())
            return;

        bool canSwap = wm.CanSwapMagazine(m_BotWeapon, m_ReserveMag, false);
        if (!canSwap)
        {
            if (RebuildReserveMagazine(currentMag))
                canSwap = wm.CanSwapMagazine(m_BotWeapon, m_ReserveMag, false);
        }

        if (!canSwap)
            return;

        InventoryLocation reserveSrc = new InventoryLocation();
        if (!m_ReserveMag.GetInventory().GetCurrentInventoryLocation(reserveSrc))
            return;

        if (!CanPostWeaponEvent())
        {
            m_NextReloadAttemptTime = GetGame().GetTime() + 200;
            return;
        }

        inv.PostWeaponEvent(new WeaponEventSwapMagazine(m_Bot, m_ReserveMag, reserveSrc));
        m_IsReloading = true;
        s_ActiveReloadOperations++;
        m_ReloadStartTime = GetGame().GetTime();
        m_ReloadPrevCurrentMag = currentMag;
        m_ReloadIncomingMag = m_ReserveMag;
        m_NextReloadAttemptTime = m_ReloadStartTime + 1400;
        WeaponTrace("swap_mag", "currentAmmo=" + GetMagAmmoSafe(currentMag) + " reserveAmmo=" + GetMagAmmoSafe(m_ReserveMag), true);
    }

    protected void FinalizeReloadState()
    {
        if (!m_IsReloading || !m_BotWeapon)
            return;

        int now = GetGame().GetTime();
        if (m_ReloadStartTime <= 0)
            m_ReloadStartTime = now;

        int muzzle = m_BotWeapon.GetCurrentMuzzle();
        Magazine currentMag = Magazine.Cast(m_BotWeapon.GetMagazine(muzzle));
        DayZPlayerInventory inv = m_Bot.GetDayZPlayerInventory();
        bool invBusy = inv && inv.IsProcessing();
        bool weaponBusy = !m_BotWeapon.IsIdle() || m_BotWeapon.IsWaitingForActionFinish();
        bool minReloadTimePassed = (now - m_ReloadStartTime) >= 900;

        bool finished = false;
        if (currentMag && m_ReloadIncomingMag && currentMag == m_ReloadIncomingMag)
            finished = true;

        if ((now - m_ReloadStartTime) > 2500)
        {
            weaponBusy = false;
            minReloadTimePassed = true;
        }

        if (finished)
        {
            if (!minReloadTimePassed || weaponBusy)
                return;

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
            ArmPostReloadSync("reload_done", true);
            ClearReloadState();
            EnsureRoundChambered("reload_finish", true);
            return;
        }

        if ((now - m_ReloadStartTime) > 5000 && !invBusy && !weaponBusy)
        {
            if (!currentMag && m_ReloadIncomingMag)
                return;

            if (currentMag)
                currentMag.ServerSetAmmoCount(GetMagazineAmmo());

            EnsureReserveMagazine(currentMag);
            if (m_ReserveMag && m_ReserveMag.GetAmmoCount() < GetReserveMagAmmo())
                m_ReserveMag.ServerSetAmmoCount(GetReserveMagAmmo());

            ArmPostReloadSync("reload_timeout_fallback", true);
            ClearReloadState();
        }
    }

    protected void ClearReloadState()
    {
        m_IsReloading = false;
        if (s_ActiveReloadOperations > 0)
            s_ActiveReloadOperations--;
        if (s_ActiveReloadOperations < 0)
            s_ActiveReloadOperations = 0;

        m_ReloadStartTime = 0;
        m_ReloadPrevCurrentMag = null;
        m_ReloadIncomingMag = null;
    }

    protected bool RebuildReserveMagazine(Magazine currentMag)
    {
        if (!m_Bot)
            return false;

        Magazine rebuilt = Magazine.Cast(m_Bot.GetInventory().CreateInInventory(GetMagazineType()));
        if (!rebuilt)
            return false;

        rebuilt.ServerSetAmmoCount(GetReserveMagAmmo());
        m_ReserveMag = rebuilt;
        PruneExtraReserveMagazines(m_ReserveMag, currentMag);
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

    protected void EnsureRoundChambered(string sourceTag, bool forceLog = false)
    {
        if (!m_Bot || !m_BotWeapon || m_IsReloading)
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

        inv.PostWeaponEvent(new WeaponEventMechanism(m_Bot, NULL));
        m_ChamberEventInFlight = true;
        m_LastChamberPostTime = now;
        m_NextChamberAttemptTime = now + 350;
        WeaponTrace("chamber", "source=" + sourceTag + " magAmmo=" + mag.GetAmmoCount().ToString(), forceLog);
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
            WeaponTrace("firemode", "requested=" + requestedMode + " mode=" + selectedMode.ToString(), true);
    }

    protected bool TryHandleWeaponJam(int now)
    {
        if (!m_BotWeapon)
            return false;

        if (!m_BotWeapon.IsJammed())
            return false;

        // Bots run with no-jam semantics. Do not let a jam keep the Expansion FSM stuck.
        m_BotWeapon.SetJammed(false);
        m_NextUnjamAttemptTime = now + UNJAM_RETRY_INTERVAL_MS;
        return false;
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
        WeaponTrace("post_reload_sync", reason, force);
    }

    protected bool CanPostWeaponEvent()
    {
        return ScriptInputUserData.CanStoreInputUserData();
    }

    protected void AimPitchOnly(vector targetPos)
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
        float pitch = NormalizeAimPitchDeg(pitchDelta.VectorToAngles()[1]);
        m_Bot.BQBot_SetAimPitch(pitch);
    }

    protected float NormalizeAimPitchDeg(float pitchDeg)
    {
        while (pitchDeg > 180.0)
            pitchDeg = pitchDeg - 360.0;
        while (pitchDeg < -180.0)
            pitchDeg = pitchDeg + 360.0;

        return Math.Clamp(pitchDeg, -85.0, 85.0);
    }

    protected void EnsureBotStorage()
    {
        if (!m_Bot)
            return;

        string vestType = GetVestType();
        if (vestType != "" && !m_Bot.GetInventory().FindAttachment(InventorySlots.VEST))
            m_Bot.GetInventory().CreateAttachment(vestType);

        string backpackType = GetBackpackType();
        if (backpackType != "" && !m_Bot.GetInventory().FindAttachment(InventorySlots.BACK))
            m_Bot.GetInventory().CreateAttachment(backpackType);
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

        if (cm)
            cm.ForceStanceUp(DayZPlayerConstants.STANCEIDX_ERECT);
    }

    protected string GetSpawnClassname()
    {
        string classname = "bqBots_Boris_AI";
        if (m_Settings && m_Settings.m_Spawn && m_Settings.m_Spawn.m_Classname != "")
            classname = m_Settings.m_Spawn.m_Classname;

        if (classname == "bqBots_Boris")
            classname = "bqBots_Boris_AI";

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
            mode.ToLower();
            if (mode == "semi")
                return "single";
            if (mode == "single" || mode == "burst" || mode == "auto")
                return mode;
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

    protected string GetMagAmmoSafe(Magazine mag)
    {
        if (!mag)
            return "none";
        return mag.GetAmmoCount().ToString();
    }

    protected void WeaponTrace(string tag, string details = "", bool force = false)
    {
        if (!WEAPON_TRACE_LOGS && !force)
            return;

        int now = GetGame().GetTime();
        if (!force && now < m_NextWeaponTraceTime)
            return;

        m_NextWeaponTraceTime = now + 500;
        Print(LOG_PREFIX + "[weapon] " + tag + " " + details);
    }

    protected void ReloadDebug(string msg, bool force = false)
    {
        if (!RELOAD_DEBUG_LOGS && !force)
            return;

        int now = GetGame().GetTime();
        if (!force && now < m_NextReloadDebugTime)
            return;

        m_NextReloadDebugTime = now + 500;
        Print(LOG_PREFIX + "[reload] " + msg);
    }

    protected void VerboseLog(string msg)
    {
        if (VERBOSE_LOGS)
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

    // Compatibility for the old BQ target-memory helper. The real target memory is now Expansion/eAI target state.
    vector BQBot_ProjectTargetMemoryPosition(vector rawPos, float sampleRadius)
    {
        return rawPos;
    }

    vector BQBot_GetTargetMemoryAIPosition()
    {
        if (m_Bot)
            return m_Bot.GetPosition();
        return "0 0 0";
    }

    bool BQBot_IsTargetMemoryFlankActive()
    {
        return false;
    }

    bool BQBot_GetGroupTargetMemoryUpdateFlag()
    {
        return false;
    }

    void BQBot_SetGroupTargetMemoryUpdateFlag(bool value)
    {
    }

    float BQBot_CalculateTargetMemoryThreat(PlayerBase target, vector searchPosition, bool hasLos, int now)
    {
        if (!target || !m_Bot)
            return 0.0;

        float dist = vector.Distance(m_Bot.GetPosition(), target.GetPosition());
        if (hasLos)
            return Math.Clamp(1.0 - (dist / 500.0), 0.15, 1.0);
        return Math.Clamp(1.0 - (dist / 250.0), 0.0, 0.35);
    }
}
