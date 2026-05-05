class bqCombatNavMesh
{
	ref array<ref bqCombatNavMeshPolygon> m_Polygons = new array<ref bqCombatNavMeshPolygon>();
	ref array<vector> m_LastFoundPath;

	void Generate(Object object, LOD lod)
	{
		array<Selection> selections();
		lod.GetSelections(selections);

		foreach (Selection selection : selections)
		{
			if (selection.GetVertexCount() != 4)
				continue;

			bqCombatNavMeshPolygon polygon = new bqCombatNavMeshPolygon(object, lod, selection);
			m_Polygons.Insert(polygon);
		}

		for (int i = 0; i < m_Polygons.Count(); i++)
		{
			for (int j = i + 1; j < m_Polygons.Count(); j++)
			{
				bqCombatNavMeshPolygon pg0 = m_Polygons[i];
				bqCombatNavMeshPolygon pg1 = m_Polygons[j];
				if (pg0.ShouldBeConnecting(pg1))
					pg0.Add(pg1);
			}
		}
	}

	bool IsValid()
	{
		return m_Polygons.Count() > 0;
	}

	bool FindPath(vector start, vector end, PGFilter filter, out array<vector> path)
	{
		bqCombatNavMeshPolygon startPoly = SamplePolygon(start, start);
		bqCombatNavMeshPolygon endPoly = SamplePolygon(end, end);
		if (!startPoly || !endPoly)
			return false;

		int count;
		if (startPoly == endPoly)
		{
			path.Resize(2);
			path[0] = start;
			path[1] = end;
			count = 2;
		}
		else
		{
			array<bqCombatNavMeshPolygon> polyPath();
			bqCombatAStar<bqCombatNavMeshPolygon>.Perform(startPoly, endPoly, filter, polyPath);
			count = polyPath.Count();
			if (count > 0)
			{
				path.Resize(count);
				for (int i = 0; i < count; i++)
				{
					int j = count - (i + 1);
					path[j] = polyPath[i].m_Position;
				}
				path[0] = start;
				bqCombatNavMeshPolygon finalPoly = polyPath[0];
				if (finalPoly == endPoly)
					path[count - 1] = end;
			}
		}

		if (count > 0)
		{
			m_LastFoundPath = path;
			return true;
		}

		return false;
	}

	bool Raycast(vector start, vector end, PGFilter filter, out vector hitPosition, out vector hitNormal)
	{
		bqCombatNavMeshPolygon startPoly = SamplePolygon(start, start);
		SamplePolygon(end, end);
		if (!startPoly)
			return false;

		int includeFlags = filter.GetIncludeFlags();
		int excludeFlags = filter.GetExcludeFlags();
		if ((includeFlags & startPoly.m_Flags) == 0)
			return false;
		if ((excludeFlags & startPoly.m_Flags) != 0)
			return false;

		hitPosition = start;
		hitNormal = vector.Direction(start, end).Normalized();
		return true;
	}

	bqCombatNavMeshPolygon SamplePolygon(vector position, out vector sampledPosition)
	{
		bqCombatNavMeshPolygon result;
		sampledPosition = position;
		float minDistance = float.MAX;

		foreach (bqCombatNavMeshPolygon polygon : m_Polygons)
		{
			vector otherPos = polygon.SamplePosition(position);
			float distance = vector.DistanceSq(position, otherPos);
			if (minDistance < distance)
				continue;

			minDistance = distance;
			sampledPosition = otherPos;
			result = polygon;
		}

		return result;
	}

	bool SamplePosition(vector position, out vector sampledPosition)
	{
		sampledPosition = position;
		float minDistance = float.MAX;
		bool found = false;

		foreach (bqCombatNavMeshPolygon polygon : m_Polygons)
		{
			vector otherPos = polygon.SamplePosition(position);
			float distance = vector.DistanceSq(position, otherPos);
			if (minDistance < distance)
				continue;

			minDistance = distance;
			sampledPosition = otherPos;
			found = true;
		}

		return found;
	}
}