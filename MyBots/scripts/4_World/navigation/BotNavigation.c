class BotNavigation
{
	protected PlayerBase m_Bot;
	protected ref TVectorArray m_CachedPath;
	protected vector m_CachedTarget;
	protected float m_LastPathTime;
	static const float PATH_CACHE_DISTANCE = 5.0;
	static const float PATH_UPDATE_INTERVAL = 2.0;

	void BotNavigation(PlayerBase bot)
	{
		m_Bot = bot;
		m_CachedPath = new TVectorArray();
		m_CachedTarget = "0 0 0";
		m_LastPathTime = 0;
	}

	// Создаёт стандартный PGFilter для ходьбы
	// SetFlags(includeFlags, excludeFlags, exclusiveFlags) — все три параметра обязательны
	protected PGFilter CreateWalkFilter()
	{
		PGFilter filter = new PGFilter();
		int include = PGPolyFlags.WALK | PGPolyFlags.DOOR | PGPolyFlags.JUMP;
		int exclude = PGPolyFlags.DISABLED | PGPolyFlags.SWIM_SEA | PGPolyFlags.SWIM;
		filter.SetFlags(include, exclude, 0);
		return filter;
	}

	// Создаёт PGFilter для помещений (crawl/crouch/inside)
	protected PGFilter CreateIndoorFilter()
	{
		PGFilter filter = new PGFilter();
		filter.SetCost(PGAreaType.WATER, 1000);
		filter.SetCost(PGAreaType.TREE, 10);
		filter.SetCost(PGAreaType.OBJECTS, 5);
		int include = PGPolyFlags.WALK | PGPolyFlags.DOOR | PGPolyFlags.INSIDE | PGPolyFlags.CRAWL | PGPolyFlags.CROUCH;
		int exclude = PGPolyFlags.DISABLED | PGPolyFlags.SWIM_SEA | PGPolyFlags.SWIM;
		filter.SetFlags(include, exclude, 0);
		return filter;
	}

	TVectorArray FindPathTo(vector targetPos)
	{
		float now = GetGame().GetTickTime();

		if (now - m_LastPathTime < PATH_UPDATE_INTERVAL && m_CachedPath.Count() > 0)
		{
			float dist = vector.Distance(m_CachedTarget, targetPos);
			if (dist < PATH_CACHE_DISTANCE)
				return m_CachedPath;
		}

		vector botPos = m_Bot.GetPosition();
		TVectorArray path = new TVectorArray();

		AIWorld aiWorld = GetGame().GetWorld().GetAIWorld();
		if (aiWorld)
		{
			aiWorld.FindPath(botPos, targetPos, CreateWalkFilter(), path);
		}

		m_CachedPath = path;
		m_CachedTarget = targetPos;
		m_LastPathTime = now;

		return path;
	}

	TVectorArray FindPathToIndoor(vector targetPos)
	{
		float now = GetGame().GetTickTime();

		if (now - m_LastPathTime < PATH_UPDATE_INTERVAL && m_CachedPath.Count() > 0)
		{
			float dist = vector.Distance(m_CachedTarget, targetPos);
			if (dist < PATH_CACHE_DISTANCE)
				return m_CachedPath;
		}

		vector botPos = m_Bot.GetPosition();
		TVectorArray path = new TVectorArray();

		AIWorld aiWorld = GetGame().GetWorld().GetAIWorld();
		if (aiWorld)
		{
			aiWorld.FindPath(botPos, targetPos, CreateIndoorFilter(), path);
		}

		m_CachedPath = path;
		m_CachedTarget = targetPos;
		m_LastPathTime = now;

		return path;
	}

	vector FindNearestBuilding(float searchRadius = 50.0)
	{
		vector botPos = m_Bot.GetPosition();
		array<Object> nearbyObjects = new array<Object>();
		array<CargoBase> proxyCargos = new array<CargoBase>();

		GetGame().GetObjectsAtPosition(botPos, searchRadius, nearbyObjects, proxyCargos);

		vector bestBuilding = "0 0 0";
		float bestDist = searchRadius + 1;

		for (int i = 0; i < nearbyObjects.Count(); i++)
		{
			Object obj = nearbyObjects[i];
			if (!obj.IsBuilding() && !obj.IsInherited(BuildingSuper))
				continue;

			vector bPos = obj.GetPosition();
			float dist = vector.Distance(botPos, bPos);

			if (dist < bestDist)
			{
				bestDist = dist;
				bestBuilding = bPos;
			}
		}

		return bestBuilding;
	}

	bool IsInsideBuilding()
	{
		vector botPos = m_Bot.GetPosition() + "0 1.7 0";
		vector aboveBot = m_Bot.GetPosition() + "0 50 0";

		vector contactPos;
		vector contactDir;
		int contactComponent;

		bool hit = DayZPhysics.RaycastRV(botPos, aboveBot, contactPos, contactDir, contactComponent, NULL, NULL, m_Bot);
		return hit;
	}

	// SampleNavmeshPosition(position, maxDistance, pgFilter, out sampledPosition)
	vector GetNearestNavmeshPos(vector pos, float radius)
	{
		vector result = pos;
		AIWorld aiWorld = GetGame().GetWorld().GetAIWorld();
		if (aiWorld)
		{
			vector sampled;
			if (aiWorld.SampleNavmeshPosition(pos, radius, CreateWalkFilter(), sampled))
				result = sampled;
		}
		return result;
	}

	bool IsDirectPathClear(vector from, vector to)
	{
		vector contactPos;
		vector contactDir;
		int contactComponent;

		bool hit = DayZPhysics.RaycastRV(from + "0 1.5 0", to + "0 1.5 0", contactPos, contactDir, contactComponent, NULL, NULL, m_Bot);
		return !hit;
	}

	float GetPathLength(TVectorArray path)
	{
		if (path.Count() < 2)
			return 0;

		float total = 0;
		for (int i = 1; i < path.Count(); i++)
		{
			total += vector.Distance(path[i - 1], path[i]);
		}
		return total;
	}

	void ClearCache()
	{
		m_CachedPath.Clear();
		m_CachedTarget = "0 0 0";
		m_LastPathTime = 0;
	}

	static vector GetRandomPointInRadius(vector center, float radius)
	{
		float angle = Math.RandomFloat01() * Math.PI * 2;
		float dist = Math.RandomFloat01() * radius;
		float x = center[0] + Math.Cos(angle) * dist;
		float z = center[2] + Math.Sin(angle) * dist;
		float y = GetGame().SurfaceY(x, z);
		return Vector(x, y, z);
	}
};