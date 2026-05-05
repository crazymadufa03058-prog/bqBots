class BQBotTargetMemoryState
{
	BQBotShooterManager m_AI;
	PlayerBase m_Target;

	int m_ThreatLevelUpdateTimestamp;
	float m_ThreatLevel;
	float m_ThreatLevelActive;
	vector m_LastKnownPosition;
	int m_SearchPositionUpdateCount;
	int m_SearchPositionUpdateTimestamp;
	vector m_SearchPosition;
	vector m_SearchDirection;
	bool m_SearchOnLOSLost;
	bool m_LOS;
	bool m_SkipMelee;
	int m_AggressionTimeout;

	void BQBotTargetMemoryState(BQBotShooterManager ai, PlayerBase target, bool initialUpdate = true)
	{
		m_AI = ai;
		m_Target = target;
		m_SearchDirection = "0 0 1";

		if (initialUpdate && m_Target)
		{
			UpdatePosition(true);
			UpdateThreat(true);
		}
	}

	void SetTarget(PlayerBase target)
	{
		if (m_Target == target)
			return;

		m_Target = target;
		m_ThreatLevel = 0.0;
		m_ThreatLevelActive = 0.0;
		m_LastKnownPosition = "0 0 0";
		m_SearchPosition = "0 0 0";
		m_SearchDirection = "0 0 1";
		m_SearchPositionUpdateCount = 0;
		m_SearchPositionUpdateTimestamp = 0;
		m_SearchOnLOSLost = false;
		m_LOS = false;
	}

	void SetInitial(float threat, vector position)
	{
		int time = GetGame().GetTime();

		m_ThreatLevelUpdateTimestamp = time;
		if (threat > m_ThreatLevel)
			m_ThreatLevel = threat;
		m_ThreatLevelActive = threat;

		m_SearchPositionUpdateTimestamp = time;
		m_LastKnownPosition = position;
		m_SearchPosition = position;
		m_SearchDirection = vector.Direction(GetAIPosition(), m_SearchPosition);
		m_SearchOnLOSLost = true;
	}

	void SetThreat(float threat)
	{
		m_ThreatLevelUpdateTimestamp = GetGame().GetTime();
		if (threat > m_ThreatLevel)
			m_ThreatLevel = threat;
		m_ThreatLevelActive = threat;
	}

	void SetLOS(bool los)
	{
		m_LOS = los;
		if (los)
			m_SearchOnLOSLost = false;
		else
			m_SearchOnLOSLost = true;
	}

	void UpdateThreat(bool force = false)
	{
		int time = GetGame().GetTime();
		int diff = time - m_ThreatLevelUpdateTimestamp;
		if (force || diff > Math.RandomIntInclusive(250, 300))
		{
			m_ThreatLevelUpdateTimestamp = time;
			if (m_AI)
				m_ThreatLevel = m_AI.BQBot_CalculateTargetMemoryThreat(m_Target, m_SearchPosition, m_LOS, time);

			if (m_LOS)
			{
				m_ThreatLevelActive = m_ThreatLevel;
			}
			else if (!force)
			{
				float threatMin;
				if (!m_AI || !m_AI.BQBot_IsTargetMemoryFlankActive() || !m_SearchOnLOSLost)
					threatMin = 0.0999;
				else
					threatMin = 0.4;

				float threatGoal = Math.Min(threatMin, m_ThreatLevel);
				float threatLerpT = diff * 0.00003333;
				m_ThreatLevelActive = Math.Lerp(m_ThreatLevelActive, threatGoal, threatLerpT);
			}
		}
	}

	void UpdatePosition(bool force = false)
	{
		if (!m_AI)
			return;

		int time = GetGame().GetTime();
		if (force || m_LOS || time - m_SearchPositionUpdateTimestamp > Math.RandomIntInclusive(250, 300))
		{
			if (force || m_LOS)
			{
				if (!m_Target)
					return;

				m_LastKnownPosition = m_AI.BQBot_ProjectTargetMemoryPosition(m_Target.GetPosition(), 3.0);
				m_SearchPositionUpdateCount = 0;
				m_SearchPositionUpdateTimestamp = time;
				m_SearchPosition = m_LastKnownPosition;
				m_SearchDirection = vector.Direction(GetAIPosition(), m_SearchPosition);
				if (!force)
					m_AI.BQBot_SetGroupTargetMemoryUpdateFlag(false);
			}
			else if (m_ThreatLevelActive > 0.2 && !m_AI.BQBot_IsTargetMemoryFlankActive())
			{
				bool updateSearchPosition = m_AI.BQBot_GetGroupTargetMemoryUpdateFlag();

				bool firstGroupStep = updateSearchPosition && !m_SearchPositionUpdateCount;
				bool groupTimeout = updateSearchPosition && time - m_SearchPositionUpdateTimestamp > 10000;
				bool reachedSearchPoint = vector.DistanceSq(GetAIPosition(), m_SearchPosition) < 1.0;
				if (firstGroupStep || groupTimeout || reachedSearchPoint)
				{
					m_SearchPositionUpdateCount++;
					m_SearchPositionUpdateTimestamp = time;
					if (m_SearchPositionUpdateCount <= 3)
					{
						vector searchDirN = GetSearchDirectionNormalized();
						vector angles = searchDirN.VectorToAngles();
						angles[0] = BQBot_AbsAngle(BQBot_RelAngle(angles[0] + Math.RandomFloat(-33.75, 33.75)));
						vector dir = angles.AnglesToVector();
						m_SearchPosition = m_AI.BQBot_ProjectTargetMemoryPosition(m_SearchPosition + dir * Math.RandomFloat(5.0, 10.0), 3.0);
					}
					else
					{
						vector radialCenter = m_SearchPosition + GetSearchDirectionNormalized();
						m_SearchPosition = m_AI.BQBot_ProjectTargetMemoryPosition(BQBot_GetRandomPointInCircle(radialCenter, 30.0), 3.0);
					}

					m_SearchDirection = vector.Direction(m_LastKnownPosition, m_SearchPosition);
					m_AI.BQBot_SetGroupTargetMemoryUpdateFlag(true);
				}
			}
		}
	}

	vector GetPosition(bool actual = false)
	{
		if (actual && m_Target)
			return m_Target.GetPosition();

		UpdatePosition(false);
		return m_SearchPosition;
	}

	vector GetDirection(bool actual = false)
	{
		return vector.Direction(GetAIPosition(), GetPosition(actual));
	}

	float GetDistance(bool actual = false)
	{
		return vector.Distance(GetAIPosition(), GetPosition(actual));
	}

	float GetDistanceSq(bool actual = false)
	{
		return vector.DistanceSq(GetAIPosition(), GetPosition(actual));
	}

	float GetCachedThreat(bool ignoreLOS = false)
	{
		if (ignoreLOS)
			return m_ThreatLevel;

		return m_ThreatLevelActive;
	}

	int GetAggressionCooldown()
	{
		if (m_AggressionTimeout > 0)
			return m_AggressionTimeout - GetGame().GetTime();

		return 0;
	}

	protected vector GetAIPosition()
	{
		if (m_AI)
			return m_AI.BQBot_GetTargetMemoryAIPosition();

		return "0 0 0";
	}

	protected vector GetSearchDirectionNormalized()
	{
		vector dir = m_SearchDirection;
		dir[1] = 0.0;
		if (dir.LengthSq() < 0.0001)
			dir = vector.Direction(GetAIPosition(), m_SearchPosition);
		dir[1] = 0.0;
		if (dir.LengthSq() < 0.0001)
			dir = "0 0 1";
		dir.Normalize();
		return dir;
	}

	protected float BQBot_RelAngle(float angle)
	{
		while (angle > 180.0)
			angle = angle - 360.0;
		while (angle < -180.0)
			angle = angle + 360.0;
		return angle;
	}

	protected float BQBot_AbsAngle(float angle)
	{
		while (angle < 0.0)
			angle = angle + 360.0;
		while (angle >= 360.0)
			angle = angle - 360.0;
		return angle;
	}

	protected vector BQBot_GetRandomPointInCircle(vector center, float radius)
	{
		float randomAngle = Math.RandomFloat(0.0, 360.0) * Math.DEG2RAD;
		float randomRadius = radius * Math.Sqrt(Math.RandomFloat(0.0, 1.0));
		return center + Vector(Math.Sin(randomAngle), 0.0, Math.Cos(randomAngle)) * randomRadius;
	}
}