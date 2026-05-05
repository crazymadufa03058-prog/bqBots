// Variant A compile-stub only. Movement/pathfinding is owned by ExpansionPathHandler.
class bqCombatPathHandler
{
	protected bqBots_Boris m_Unit;
	protected ref array<ref bqCombatPathPoint> m_Path;
	protected int m_Index;
	protected vector m_Target;
	protected bool m_HasTarget;
	protected bool m_IsUnreachable;
	bool m_IsSwimmingEnabled;
	bool m_AllowJumpClimb;

	void bqCombatPathHandler(bqBots_Boris unit = null)
	{
		m_Unit = unit;
		m_Path = new array<ref bqCombatPathPoint>;
		m_Index = 0;
		m_Target = "0 0 0";
		m_HasTarget = false;
		m_IsUnreachable = false;
		m_IsSwimmingEnabled = true;
		m_AllowJumpClimb = true;
	}

	void SetTarget(vector target, float maxDistance = 1.0, bool allowJumpClimb = true)
	{
		m_Target = target;
		m_HasTarget = true;
		m_IsUnreachable = false;
		m_AllowJumpClimb = allowJumpClimb;
		m_Path.Clear();
		m_Path.Insert(new bqCombatPathPoint(target, true, false));
		m_Index = 0;
	}

	void SetTarget(vector target, float maxDistance, bool allowJumpClimb, bool forceRecalculate)
	{
		SetTarget(target, maxDistance, allowJumpClimb);
	}

	void Stop() { Clear(); }
	void StopOverride() { Clear(); }
	void Reset() { Clear(); }
	void Recalculate() {}
	void ForceRecalculate() {}
	void Update(float dt = 0) {}
	void OnUpdate(float dt = 0, int simulationPrecision = 0) {}
	void EnableSwimming(bool enable = true, bool forceUpdate = true) { m_IsSwimmingEnabled = enable; }
	void ResetUnreachable() { m_IsUnreachable = false; }
	void Clear()
	{
		m_HasTarget = false;
		m_IsUnreachable = false;
		if (m_Path)
			m_Path.Clear();
		m_Index = 0;
	}

	bool IsUnreachable() { return m_IsUnreachable; }
	bool IsTargetUnreachable() { return m_IsUnreachable; }
	bool HasTarget() { return m_HasTarget; }
	bool GetOverride() { return m_HasTarget; }
	vector GetTarget() { return m_Target; }
	vector GetEnd() { return m_Target; }
	PGFilter GetFilter() { return null; }

	bqCombatPathPoint GetCurrent()
	{
		if (!m_Path || m_Path.Count() == 0)
			return null;
		if (m_Index < 0)
			m_Index = 0;
		if (m_Index >= m_Path.Count())
			m_Index = m_Path.Count() - 1;
		return m_Path[m_Index];
	}

	vector GetCurrentPosition()
	{
		bqCombatPathPoint point = GetCurrent();
		if (!point)
			return m_Target;
		return point.GetPosition();
	}

	bool IsDone(vector currentPos = "0 0 0", float maxDistance = 1.0)
	{
		if (!m_HasTarget)
			return true;
		if (currentPos == "0 0 0" && m_Unit)
			currentPos = m_Unit.GetPosition();
		return vector.DistanceSq(currentPos, m_Target) <= maxDistance * maxDistance;
	}

	void Advance()
	{
		m_Index++;
		if (m_Path && m_Index >= m_Path.Count())
			m_Index = m_Path.Count() - 1;
	}

	int GetIndex() { return m_Index; }
	int GetCount()
	{
		if (!m_Path)
			return 0;
		return m_Path.Count();
	}
}
