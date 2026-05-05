# Expansion AI: Полная логика передвижения и боя

> Документ составлен на основе анализа логов рабочего Expansion (`script_2026-04-27_16-58-45Expansion.log`) и исходников порта `ExpansionRawPort` в проекте bqBots.

---

## 1. Обзор архитектуры

Expansion AI построена вокруг **иерархического конечного автомата (FSM)**. Каждый бот (`eAIBase`) владеет экземпляром `eAIFSM`, загружаемым из XML-описания (`DayZExpansion/AI/scripts/FSM/Master`). FSM определяет, в каком состоянии находится бот, и какие переходы (`Guard` / `GuardIdle`) возможны между состояниями.

### 1.1. Два уровня FSM

```
Main FSM (Master)
├── eAIState_Idle
├── eAIState_TraversingWaypoints      ← патруль
├── eAIState_Flank                    ← фланк (до входа в бой)
├── eAIState_Fighting                 ← контейнер для Sub-FSM
│   └── Sub-FSM (Fighting)
│       ├── eAIState_Fighting_Positioning
│       ├── eAIState_Fighting_FireWeapon
│       ├── eAIState_Fighting_Melee
│       └── eAIState_Fighting_Evade
├── eAIState_FollowFormation
└── ... (другие состояния)
```

**Ключевой момент:** `eAIState_Flank` — это **отдельное состояние Main FSM**, а не часть Fighting Sub-FSM. Бот может находиться во фланке, НЕ находясь в состоянии `Fighting`.

### 1.2. Кто управляет движением

Движением управляет **не FSM напрямую**, а цепочка:

```
FSM State (OnUpdate)
  → eAIBase.OverrideTargetPosition(target / vector)
    → ExpansionPathHandler.SetTarget()
      → eAICommandMove (каждый тик Engine)
        → Физическое перемещение бота
```

FSM-состояния лишь решают, **какую целевую позицию** установить. Вся "мозговая" логика позиционирования сосредоточена в `eAIBase.OverrideTargetPosition(eAITarget)`.

---

## 2. Инициализация бота

### 2.1. Создание и старт

```csharp
// eAIBase.c
override void Init()
{
    super.Init();
    
    // ... инициализация переменных ...
    
    m_AimingProfile = new eAIAimingProfile(this);
    m_eMeleeCombat = new eAIMeleeCombat(this);
    m_MeleeCombat = m_eMeleeCombat;
    
    m_eAI_MeleeFightLogic = new eAIMeleeFightLogic_LightHeavy(this);
    m_MeleeFightLogic = m_eAI_MeleeFightLogic;
    
#ifndef EAI_USE_LEGACY_PATHFINDING
    m_PathFinding = new ExpansionPathHandler(this);
#else
    m_PathFinding = new eAIPathFinding(this);
#endif

    LoadFSM();  // ← Загрузка конечного автомата
    
    // ...
    m_eAI_CommandMove = new eAICommandMove(this, m_ExpansionST, DayZPlayerConstants.STANCEIDX_ERECT);
}
```

### 2.2. Загрузка FSM

```csharp
// eAIBase.c
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
```

XML `Master` определяет:
- Все состояния (`eAIState_Idle`, `eAIState_Flank`, ...)
- Переходы между ними через `Guard()` / `GuardIdle()` методы состояний
- Sub-FSM для `eAIState_Fighting`

### 2.3. Группа и фракция

```csharp
// eAIBase.c
override void Expansion_Init()
{
    if (g_Game.IsServer() && !m_eAI_FactionType)
    {
        m_eAI_FactionType = eAIFactionRaiders;
    }
    super.Expansion_Init();
    // ...
}
```

Бот всегда принадлежит `eAIGroup`. Группа управляет:
- Фракцией (`GetFaction()`)
- Формацией (`eAIFormationVee` и др.)
- Поведением вейпоинтов (`ROAMING`, `LOOP`, `ALTERNATE`, `HALT`)
- Состоянием формации (`IN`, `FLANK`)

---

## 3. Патрулирование — `eAIState_TraversingWaypoints`

### 3.1. Вход в состояние

```csharp
override void OnEntry(string Event, ExpansionState From)
{
    unit.eAI_LeaveCover();  // ← выход из укрытия
    
    eAIWaypointBehavior behaviour = unit.GetGroup().GetWaypointBehaviour();
    if (behaviour == eAIWaypointBehavior.ROAMING && m_Waypoint == vector.Zero)
    {
        m_Waypoint = unit.GetGroup().FindClosestRoamingLocationPosition();
        m_LeaveThreshold = Math.RandomFloat(5.0, 15.0);
    }
    
    if (unit.m_eAI_DefaultStance > eAIStance.STANDING)
    {
        unit.Expansion_GetUp(true);
        m_GotUp = true;
    }
}
```

### 3.2. Обновление — движение к вейпоинту

```csharp
override int OnUpdate(float DeltaTime, int SimulationPrecision)
{
    // 1. Получаем текущий waypoint
    eAIWaypointBehavior behaviour = unit.GetGroup().GetWaypointBehaviour();
    if (behaviour != eAIWaypointBehavior.ROAMING)
    {
        path = unit.GetGroup().GetWaypoints();
        m_Index = unit.GetGroup().m_CurrentWaypointIndex;
        m_Waypoint = path[m_Index];
    }
    
    // 2. Проверяем достижение
    float distance = vector.DistanceSq(position, targetPosition);
    bool waypointReached = distance < m_Threshold;  // m_Threshold = 1.0 (по умолчанию)
    
    // 3. Если достигли — advance waypoint
    if (waypointReached)
    {
        if (behaviour == eAIWaypointBehavior.ROAMING)
        {
            // Выбираем новую точку роуминга
            m_Waypoint = unit.GetGroup().FindClosestRoamingLocationPosition();
        }
        else
        {
            // Сдвигаем индекс (с учётом LOOP/ALTERNATE)
            m_Index++;
            unit.GetGroup().m_CurrentWaypointIndex = m_Index;
        }
    }
    
    // 4. Устанавливаем цель движения
    unit.OverrideTargetPosition(m_Waypoint, isFinal);
    unit.OverrideMovementDirection(false, 0);
    unit.OverrideMovementSpeed(false, 0);
    
    return EXIT;
}
```

### 3.3. Guard — условия выхода из патруля

```csharp
int Guard()
{
    auto group = unit.GetGroup();
    if (!group) return eAITransition.FAIL;
    
    // Если угроза высокая — выходим в бой или фланк
    if (unit.GetThreatToSelf() >= 0.4) return eAITransition.FAIL;
    
    // Если группа в бою и цель серьёзная
    if (group.IsInCombat())
    {
        eAITarget target = unit.GetTarget();
        if (target && !target.IsItem() && unit.GetThreatToSelf(true) >= 0.4)
            return eAITransition.FAIL;
    }
    
    // Только лидер патрулирует
    auto leader = group.GetFormationLeader();
    if (leader && leader != unit) return eAITransition.FAIL;
    
    return eAITransition.SUCCESS;
}
```

**Важно:** `GetThreatToSelf() >= 0.4` — это порог для выхода из патруля. При меньшей угрозе (0.152–0.4) бот может войти в `eAIState_Flank`.

---

## 4. Обнаружение цели — `target_acquired`

### 4.1. Как бот "видит" цель

```
[EAI] [target_acquired] bot=eAI_SurvivorM_Seth<...> 
  target=SurvivorBase<...> SurvivorM_Mirek:12131 
  targetPos=<5531.672852, 107.555008, 3131.899170> 
  distSq=21359.3 
  totalTargets=1
```

Цель обнаружена на расстоянии ~146м (`sqrt(21359.3)`). Сразу после этого:

```
[EAI] [state_exit] eAIState_TraversingWaypoints aborted=false
[EAI] [state_enter] eAIState_Flank
```

### 4.2. Threat calculation

`GetThreatToSelf()` — расчётная угроза от текущей цели. Зависит от:
- Расстояния до цели
- Наличия LOS (Line of Sight)
- Типа цели (игрок, зомби, животное)
- Состояния цели (поднято оружие, движется)

Ключевые пороги:
| Порог | Значение | Смысл |
|-------|----------|-------|
| `FLANK_GUARD` | `> 0.152` | Возможен вход во фланк |
| `FIGHTING_GUARD` | `>= 0.4` | Вход в Fighting (полноценный бой) |
| `IDLE_GUARD` | `< 0.2` | Возврат в Idle |

---

## 5. Фланкирование — `eAIState_Flank`

### 5.1. Структура состояния

```csharp
class eAIState_Flank: eAIState
{
    eAITarget m_Target;

    override void OnEntry(string Event, ExpansionState From)
    {
        unit.OverrideMovementDirection(false, 0);
        unit.OverrideMovementSpeed(false, 0);
        unit.eAI_SetShouldTakeCover(true);  // ← включаем поиск укрытий
    }

    override void OnExit(string Event, bool Aborted, ExpansionState To)
    {
        unit.eAI_SetShouldTakeCover(false); // ← выключаем поиск укрытий
    }

    override int OnUpdate(float DeltaTime, int SimulationPrecision)
    {
        if (m_Target)
        {
            unit.OverrideTargetPosition(m_Target);  // ← ВСЯ логика здесь
        }
        return EXIT;
    }
}
```

**Это критически важно:** состояние `Flank` само по себе НЕ считает фланковую точку. Оно просто вызывает `OverrideTargetPosition(m_Target)`, а уже `eAIBase` решает, куда идти.

### 5.2. Guard — условия переходов

```csharp
int Guard()
{
    // Если угроза высокая — уходим в Fighting
    if (unit.GetThreatToSelf() >= 0.4) return eAITransition.FAIL;
    
    if (unit.IsInTransport()) return eAITransition.FAIL;
    
    auto group = unit.GetGroup();
    if (!group) return eAITransition.FAIL;
    
    // Фланк возможен только если группа в бою ИЛИ в формации FLANK
    if (!unit.m_eAI_EnableFlankingOutsideCombat && 
        group.GetFormationState() != eAIGroupFormationState.FLANK && 
        !group.IsInCombat())
        return eAITransition.FAIL;
    
    m_Target = unit.GetTarget();
    if (!m_Target || m_Target.IsItem() || unit.GetThreatToSelf(true) <= 0.152)
        return eAITransition.FAIL;
    
    return eAITransition.SUCCESS;
}
```

### 5.3. `OverrideTargetPosition(eAITarget)` — ядро фланка

Метод `eAIBase.OverrideTargetPosition(eAITarget)` (строки 2808–3150) — это центральная точка принятия решений. Разберём его по шагам.

#### Шаг 1: Проверка на лестницу
```csharp
if (m_eAI_IsOnLadder)
    return;  // Если на лестнице — не меняем цель
```

#### Шаг 2: Получаем базовые данные
```csharp
vector pos;
vector dir = target.GetDirection();  // Направление от бота к цели
float dist = dir.Length();            // Дистанция до цели
float minDist = target.GetMinDistance(dist);  // Мин. дистанция (для ловушек = >0)
```

#### Шаг 3: Ветвление по `minDist`

**Ветка A: `minDist > 0` (цель требует дистанции)**
```csharp
if (minDist)
{
    if (!eAI_ShouldUpdatePosition())  // Если уже идём и не достигли конца
    {
        // Просто обновляем позицию для опасных зон
        if (m_eAI_IsInDangerByArea)
            OverrideTargetPosition(m_eAI_TargetPosition, ...);
        return;
    }
    keepMinDistToTarget = true;
}
```

**Ветка B: `minDist == 0` (обычная цель — игрок, зомби)**
```csharp
else
{
    // КРИТИЧЕСКАЯ ПРОВЕРКА:
    if (!IsRaised() && target.IsEntity() && !target.CanMeleeIfClose())
        cannotMelee = true;
    
    // Если оружие поднято ИЛИ идёт действие ИЛИ cannotMelee — сбрасываем фланк
    if ((IsRaised() && target.m_LOS) || 
        GetActionManager().GetRunningAction() || 
        GetWeaponManager().IsRunning() || 
        cannotMelee)
    {
        allowJumpClimb = false;
        m_eAI_FlankTime = 0.0;
        m_eAI_FlankTimeMax = 0.0;
    }
    // ИНАЧЕ — проверяем возможность фланка
    else if (eAI_IsInFlankRange(dist) && !eAI_IsDangerousAltitude())
    {
        // ← ВХОДИМ В ЛОГИКУ ФЛАНКА
        ...
    }
}
```

### 5.4. Ключевое условие: `CanMeleeIfClose()`

```csharp
// eAITarget.c
bool CanMeleeIfClose()
{
    EntityAI entity = GetEntity();
    if (!entity || entity.IsItemBase() || entity.IsTransport())
        return false;
    return true;
}
```

| `CanMeleeIfClose()` | `cannotMelee` | Результат |
|---------------------|---------------|-----------|
| `true` (игрок) | `false` | **Фланк возможен** (если дистанция позволяет) |
| `false` (item/transport) | `true` | Фланк НЕВОЗМОЖЕН, уход в укрытие |

**В нашем порте** этот метод был упрощён. В оригинальном Expansion он проверяет:
- Фракцию (`GetMeleeYeetForce()`)
- Родительский объект (`Expansion_GetParent()`)
- Скорость транспорта (`GetVelocity(parent).LengthSq()`)

### 5.5. Логика выбора фланковой точки

```csharp
else if (eAI_IsInFlankRange(dist) && !eAI_IsDangerousAltitude())
{
    if (m_eAI_FlankTime <= 0.0)
    {
        // Первый вход во фланк — выбираем угол
        if (Math.RandomInt(0, 2))
            m_eAI_FlankAngle = Math.RandomFloat(56.25, 67.5);
        else
            m_eAI_FlankAngle = -Math.RandomFloat(56.25, 67.5);
        
        // Рассчитываем время фланка
        m_eAI_FlankTimeMax = Math.RandomFloat(dist * 0.3, dist * 0.6);
    }
    
    m_eAI_FlankTime += m_dT;
    
    // Если ещё идём к текущей точке — не пересчитываем
    if (m_eAI_FlankTimeMax > 0.0 && !eAI_ShouldUpdatePosition() && !m_eAI_IsInCover)
    {
        if (m_eAI_IsInDangerByArea)
            OverrideTargetPosition(m_eAI_TargetPosition, ...);
        return;
    }
    
    // Если время фланка вышло
    if (m_eAI_FlankTime >= m_eAI_FlankTimeMax)
    {
        // С шансом 1/3000 или если недостижимо — сбрасываем фланк
        if ((m_eAI_FlankTime >= m_eAI_FlankTimeMax * 2 && Math.RandomInt(0, 3000) < 1) 
            || m_PathFinding.m_IsUnreachable)
        {
            m_eAI_FlankTime = 0.0;
            return;
        }
        
        // Если в укрытии — остаёмся
        if (m_eAI_IsInCover)
        {
            // Стойка: PRONE за камнем, CROUCH за другим укрытием
            if (m_eAI_CurrentCoverObject && m_eAI_CurrentCoverObject.IsRock())
                OverrideStance(DayZPlayerConstants.STANCEIDX_PRONE);
            else if (eAI_GetStance() == DayZPlayerConstants.STANCEIDX_ERECT)
                OverrideStance(DayZPlayerConstants.STANCEIDX_CROUCH);
            
            // Поднимаем оружие
            EntityAI hands = GetHumanInventory().GetEntityInHands();
            if (hands && hands.IsWeapon())
                eAI_RaiseWeapon(true);
            
            m_eAI_TargetPositionIsFinal = true;
            return;
        }
    }
    
    // Устанавливаем флаг фланка
    flank = true;
    isFinal = false;
    Expansion_GetUp();
}
```

### 5.6. Расчёт конечной позиции

```csharp
if (minDist || flank)
{
    pos = GetPosition();
    
    if (flank)
    {
        // Поворачиваем направление на flankAngle
        vector ori = dir.VectorToAngles();
        ori[0] = ExpansionMath.RelAngle(ori[0] + m_eAI_FlankAngle);
        dir = ori.AnglesToVector();
    }
    
    // Сбрасываем текущее укрытие
    if (m_eAI_CurrentCoverObject)
    {
        s_eAI_TakenCoverObjects.RemoveItem(m_eAI_CurrentCoverObject);
        m_eAI_CurrentCoverObject = null;
    }
    
    // Ищем новое укрытие
    if (m_eAI_PotentialCoverObjects.Count() && 
        (!target.IsMechanicalTrap() || target.IsExplosive()))
    {
        foreach (Object obj: m_eAI_PotentialCoverObjects)
        {
            // ... критерии выбора ...
            if (flank)
            {
                // Укрытие должно быть в направлении фланка
                dot = vector.Dot(dir, toObjDirNorm);
                if (dot < 0.75) continue;  // < 41° от направления фланка
            }
            // ...
        }
    }
    
    // Если укрытие найдено — идём к нему
    if (m_eAI_CurrentCoverObject)
    {
        // ... расчёт точки укрытия ...
        pos = coverPosition;
    }
    else
    {
        // Иначе — raw фланковая точка
        pos = target.GetPosition() + dir * dist;
    }
    
    // ... корректировка высоты, minDist ...
    
    OverrideTargetPosition(pos, isFinal, maxDistance);
}
```

### 5.7. Пример из лога

```
[flank_target] botPos=<5386.76, 107.53, 3150.88>
[move_target_ent] targetEntity=SurvivorM_Mirek targetPos=<5531.67, 107.55, 3131.89>
[move_target] newTarget=<5449.87, 107.55, 3019.06> isFinal=false maxDist=1
```

- Бот в точке `(5386, 3150)`
- Цель в точке `(5531, 3131)`
- Фланковая точка: `(5449, 3019)` — это юго-западнее цели, т.е. бот обходит сбоку

---

## 6. Система укрытий (Cover System)

### 6.1. Сбор потенциальных укрытий

```csharp
void eAI_UpdatePotentialCoverObjects(float pDt)
{
    // Срабатывает раз в 0.1–0.2 сек
    if (m_eAI_UpdateTargetsTick <= Math.RandomFloat(0.1, 0.2))
        return;
    
    m_eAI_UpdateTargetsTick = 0;
    
    // Сканируем AABB 60×60×60м вокруг бота
    vector center = GetPosition();
    vector min = Vector(center[0] - 30, center[1] - 30, center[2] - 30);
    vector max = Vector(center[0] + 30, center[1] + 30, center[2] + 30);
    
    DayZPlayerUtils.PhysicsGetEntitiesInBox(min, max, m_eAI_PotentialTargetEntities);
    
    // Фильтруем
    foreach (EntityAI potentialCoverObj: m_eAI_PotentialTargetEntities)
    {
        if (potentialCoverObj.IsBuilding())   // Land_*, Wreck_* (без Gate)
            isPotentialCoverObject = true;
        else if (potentialCoverObj.IsRock())
            isPotentialCoverObject = true;
        else if (potentialCoverObj.IsTree())
            isPotentialCoverObject = true;
        else if (potentialCoverObj.IsTransport())
            isPotentialCoverObject = true;
    }
}
```

### 6.2. Критерии выбора укрытия во фланке

```csharp
foreach (Object obj: m_eAI_PotentialCoverObjects)
{
    if (dBodyIsActive(obj))
        continue;  // Игнорируем объекты с физикой
    
    if (flank && obj == currentCoverObj)
        continue;  // Не берём текущее укрытие при фланке
    
    if (s_eAI_TakenCoverObjects.Find(obj) > -1)
        continue;  // Укрытие занято другим ботом
    
    toObjDir = vector.Direction(forwardPos, obj.GetPosition());
    toObjDirNorm = toObjDir.Normalized();
    
    if (flank)
    {
        dot = vector.Dot(dir, toObjDirNorm);
        // Укрытие должно быть В НАПРАВЛЕНИИ фланка
        if (dot < 0.75) continue;  // cos⁻¹(0.75) ≈ 41°
    }
    // ...
}
```

### 6.3. Расчёт точки укрытия

```csharp
if (m_eAI_CurrentCoverObject.IsTree())
    extension = 2.0;
else if (building && building.Expansion_IsEnterable())
    extension = 2.5;
else
    extension = 1.0;

// Берём AABB объекта
obj.GetCollisionBox(minMax);
// Проецируем направление на AABB
// ...
// Результат: точка за объектом относительно цели
pos = coverPosition;
```

---

## 7. Pathfinding — `ExpansionPathHandler`

### 7.1. Обновление пути

```csharp
void OnUpdate(float pDt, int pSimulationPrecision)
{
    EnsureAIWorld();
    
    // Периодически пересчитываем
    if (m_Recalculate && m_Time >= timeUntilNextUpdate)
    {
        // 1. Семплируем целевую позицию на навмеш
        if (!m_TargetReference.Parent)
        {
            vector inPos = m_TargetPosition;
            float maxDistance = 3.0;  // ← Радиус поиска точки на навмеш
            
            if (!m_AIWorld.SampleNavmeshPosition(inPos, maxDistance, m_AllFilter, inPos))
            {
                m_IsTargetUnreachable = true;
            }
            // ...
            m_TargetReference.Position = inPos;
        }
        
        // 2. Ищем путь
        m_Target.Copy(m_TargetReference);
        found = m_Target.FindPath(this, m_Points, m_PathGlueIdx);
        
        // 3. Проверяем достижимость (особенно для лестниц)
        if (!m_IsTargetUnreachable && m_Count <= 3 && 
            m_Unit.m_eAI_CommandTime > 1.0 && m_Unit.m_eAI_BuildingWithLadder)
        {
            // ...
        }
    }
    
    // 4. Обновляем следующие точки
    UpdateNext();
}
```

### 7.2. Параметры

| Параметр | Значение | Описание |
|----------|----------|----------|
| `PATH_RECALCULATE_THRESHOLD` | `5.0` | Мин. время между пересчётами |
| `maxDistance` (SampleNavmesh) | `3.0` | Радиус поиска ближайшей точки на навмеш |
| `m_AllFilter` | PGFilter | Фильтр полигонов (включает WALK, DOOR, LADDER, CLIMB и др.) |

**Важно:** `maxDistance = 3.0` означает, что если целевая точка дальше 3м от навмеш, бот считает её недостижимой (`m_IsTargetUnreachable = true`).

---

## 8. Команда движения — `eAICommandMove`

### 8.1. Получение waypoints

```csharp
// eAICommandMove получает waypoint из PathHandler
m_Waypoint = m_PathFinding.GetTarget();
```

### 8.2. Obstacle Avoidance

```csharp
static float OBSTACLE_AVOIDANCE_INTERVAL = 0.1;  // 100ms

// Каждые 0.1 секунды:
// 1. Пускаем raycast вперёд
// 2. Если препятствие — проверяем left/right/backward
// 3. Выбираем направление обхода
// 4. Корректируем m_Waypoint
```

### 8.3. Скорость и стойка

```csharp
// Управление скоростью
m_TargetSpeed = ...;  // 0 = стоять, 1 = walk, 2 = jog, 3 = sprint
m_SpeedLimit = unit.GetMovementSpeedLimit();

// Стойка
if (m_eAI_IsFightingFSM && !IsSwimming() && m_WeaponRaised)
{
    // В бою с поднятым оружием — НЕ прыгаем/лазаем
}
```

---

## 9. Бой — Fighting Sub-FSM

### 9.1. `eAIState_Fighting` — контейнер

```csharp
override void OnEntry(string Event, ExpansionState From)
{
    unit.eAI_SetIsFightingFSM(true);
    if (m_SubFSM)
        m_SubFSM.StartDefault();  // Запускаем Sub-FSM
}

override int OnUpdate(float DeltaTime, int SimulationPrecision)
{
    if (m_SubFSM)
        m_SubFSM.Update(DeltaTime, SimulationPrecision);
    return EXIT;
}
```

### 9.2. `eAIState_Fighting_Positioning`

```csharp
override int OnUpdate(float DeltaTime, int SimulationPrecision)
{
    auto target = unit.GetTarget();
    if (target)
    {
        // 1. Определяем, нужно ли поднять/опустить оружие
        bool wantsLower = false;
        bool wantsRaise = false;
        
        // 2. Расстояние до цели
        unit.m_eAI_DistanceToTargetSq = target.GetDistanceSq(true);
        
        // 3. Melee или ranged?
        if (shouldBeMeleeing)
        {
            if (unit.m_eAI_DistanceToTargetSq <= rangeSq * raiseRangeSqMult)
                wantsRaise = true;
            minDist = 1.0;  // Для melee
        }
        else
        {
            minDist = 2.0;  // Для ranged
        }
        
        // 4. Устанавливаем цель
        unit.OverrideTargetPosition(target);
    }
    
    return EXIT;
}
```

**Важно:** `Fighting_Positioning` тоже вызывает `OverrideTargetPosition(target)`. Но в отличие от `Flank`, здесь бот уже в состоянии `Fighting`, т.е. `GetThreatToSelf() >= 0.4`. Это значит, что во `Fighting_Positioning` бот скорее будет искать укрытие/держать дистанцию, чем фланковать.

### 9.3. `eAIState_Fighting_Evade`

```csharp
int Guard()
{
    // Уклонение срабатывает, если:
    // 1. Прошло 2.5–5 сек с прошлого уклонения
    // 2. Цель — игрок
    // 3. Игрок ПОДНЯЛ оружие
    // 4. У игрока firearm в руках
    // 5. Игрок целится в бота (dot > 0.97)
    // 6. Шанс 1/5
    // 7. Бот не на лестнице
}
```

---

## 10. Интеграция bqBots ↔ Expansion

### 10.1. Два режима работы

`BQBotShooterManager` поддерживает два режима движения:

| Режим | Условие | Кто управляет |
|-------|---------|---------------|
| **Expansion FSM** | `m_Bot.GetFSM() != null` | Expansion AI полностью |
| **BQ Custom** | `m_Bot.GetFSM() == null` | `BQBotShooterManager` сам толкает waypoints |

### 10.2. Делегирование движения

```csharp
// BQBotShooterManager.c
protected bool UseExpansionFSMMovement()
{
    if (!m_Bot)
        return false;
    return m_Bot.GetFSM() != null;
}

protected void eAI_OverrideTargetPosition(PlayerBase target, vector targetPos, 
    float distToTarget, bool hasLos, float threat, int now)
{
    if (!m_Bot) return;
    
    if (UseExpansionFSMMovement())
    {
        // Делегируем Expansion — просто синхронизируем цель
        SyncExpansionTarget(target, hasLos, targetPos, threat);
        m_ChaseMoveActive = false;
        return;
    }
    
    // Иначе — BQ сам управляет движением
    // ... custom movement code ...
}
```

### 10.3. Синхронизация цели

```csharp
protected void SyncExpansionTargetState(...)
{
    // Синхронизируем LOS
    eaiTarget.m_LOS = hasLos;
    
    // Угроза
    eaiTarget.m_Threat = threat;
    
    // Позиция поиска (когда цель потеряна из виду)
    eaiTarget.m_SearchPosition = searchPos;
}
```

### 10.4. Почему у нас не работал фланк

1. `CanMeleeIfClose()` был заглушен на `return false`
2. Это заставляло `cannotMelee = true` в `OverrideTargetPosition`
3. При `cannotMelee = true` ветка `flank = true` НЕ достигалась
4. Бот уходил в поиск укрытия вместо фланкирования

---

## 11. Настройки и константы

### 11.1. В `eAIBase`

| Поле | Значение по умолчанию | Описание |
|------|----------------------|----------|
| `m_eAI_MaxFlankingDistance` | `200` | Макс. дистанция для фланка |
| `m_eAI_FlankAngle` | `0` | Текущий угол фланка (±56–67°) |
| `m_eAI_FlankTimeMax` | `0` | Время движения к фланковой точке |
| `AI_HANDLEVAULTING` | `true` | Разрешить перепрыгивание препятствий |
| `AI_HANDLEDOORS` | `true` | Разрешить открытие дверей |

### 11.2. В `ExpansionPathHandler`

| Константа | Значение | Описание |
|-----------|----------|----------|
| `PATH_RECALCULATE_THRESHOLD` | `5.0` | Мин. секунд между пересчётами пути |
| `maxDistance` (navmesh) | `3.0` | Радиус семплирования навмеш |

### 11.3. Пороги угрозы

| Порог | Значение | Эффект |
|-------|----------|--------|
| `FLANK_GUARD` | `> 0.152` | Доступен вход в `eAIState_Flank` |
| `FIGHTING_GUARD` | `>= 0.4` | Вход в `eAIState_Fighting` |
| `IDLE_RETURN` | `< 0.2` | Возврат в `Idle` |

---

## 12. Чек-лист отладки

### 12.1. Как проверить, что фланк работает

**Логи, которые ДОЛЖНЫ появиться:**

```
[EAI] [state_exit] ... state=eAIState_TraversingWaypoints
[EAI] [state_enter] ... state=eAIState_Flank
[EAI] [flank_target] botPos=...
[EAI] [move_target_ent] targetEntity=... targetPos=...
[EAI] [move_target] newTarget=<...> isFinal=false maxDist=1
```

**Если `flank_target` есть, но `move_target` показывает позицию ЦЕЛИ (а не фланковую точку):**
- Проверить `CanMeleeIfClose()` — должен возвращать `true` для игрока
- Проверить `eAI_IsInFlankRange(dist)` — дистанция должна быть < `m_eAI_MaxFlankingDistance`

**Если `state_enter` сразу `eAIState_Fighting` (без `Flank`):**
- `GetThreatToSelf()` слишком высокий (>= 0.4) — бот сразу идёт в бой
- Это нормально при близкой дистанции или прямом LOS

### 12.2. Как проверить поиск укрытий

Включить `DIAG_DEVELOPER` и смотреть:
```
EXTrace.Print(EXTrace.AI, this, "New cover object selected: ...");
```

### 12.3. Как проверить pathfinding

```
[EAI] [wp_update] ... unreachable=true ...
```
Если `unreachable=true` — `SampleNavmeshPosition` не нашёл точку в радиусе 3м.

---

## 13. Сводная схема потока

```
Бот создан
  → Init() → LoadFSM() → eAIState_Idle

Idle
  → Guard SUCCESS → TraversingWaypoints (патруль)

TraversingWaypoints
  → target_acquired + threat < 0.4  → eAIState_Flank
  → target_acquired + threat >= 0.4 → eAIState_Fighting

Flank (OverrideTargetPosition(target))
  → CanMeleeIfClose() == true?
    → YES → eAI_IsInFlankRange(dist)?
      → YES → flank=true → выбор flankAngle → поиск cover → движение
      → NO  → keepMinDistToTarget / cover
    → NO  → cannotMelee=true → cover / minDist
  → threat >= 0.4? → переход в Fighting

Fighting
  → Sub-FSM: Positioning → FireWeapon → Evade → Melee
  → threat < 0.2? → возврат в Idle
```

---

*Документ актуален для версии bqBots с портом Expansion AI (май 2026).*
