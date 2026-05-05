// Variant A compile-stub only. Movement/pathfinding is owned by ExpansionPathHandler.
class bqCombatPathPoint
{
	vector Position;
	vector m_Position;
	bool m_IsFinal;
	bool m_IsJumpClimb;
	Object Parent;
	int Flags;
	bqCombatNavMesh NavMesh;
	bqCombatPathPoint Next;

	void bqCombatPathPoint(vector position = "0 0 0", bool isFinal = false, bool isJumpClimb = false)
	{
		Position = position;
		m_Position = position;
		m_IsFinal = isFinal;
		m_IsJumpClimb = isJumpClimb;
	}

	vector GetPosition()
	{
		return m_Position;
	}

	void OnParentUpdate()
	{
	}

	void Copy(bqCombatPathPoint other)
	{
		if (!other)
			return;
		Position = other.Position;
		m_Position = other.m_Position;
		m_IsFinal = other.m_IsFinal;
		m_IsJumpClimb = other.m_IsJumpClimb;
		Parent = other.Parent;
		Flags = other.Flags;
		NavMesh = other.NavMesh;
		Next = other.Next;
	}

	bool FindPath(bqCombatPathHandler pathFinding, inout array<vector> path, out int pathGlueIdx = -1)
	{
		if (!path)
			path = new array<vector>;
		path.Insert(m_Position);
		return true;
	}

	bool FindPathFrom(vector startPos, bqCombatPathHandler pathFinding, inout array<vector> path, out int pathGlueIdx = -1)
	{
		if (!path)
			path = new array<vector>;
		path.Insert(m_Position);
		return true;
	}

	array<vector> OptimizePathForSwimming(array<vector> path, bqCombatPathHandler pathFinding)
	{
		return path;
	}
}
