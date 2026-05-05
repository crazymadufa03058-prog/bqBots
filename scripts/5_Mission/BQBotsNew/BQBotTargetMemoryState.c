/*
    BQBotTargetMemoryState

    Compatibility stub.
    Old BQ search-memory logic generated search points and influenced chase/flank behaviour.
    Expansion/eAI now owns threat memory, target loss, search, movement and tactical decisions,
    so this class only stores basic target/threat values for old code that may still instantiate it.
*/
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

        if (initialUpdate)
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
        m_ThreatLevel = threat;
        m_ThreatLevelActive = threat;
    }

    void SetLOS(bool los)
    {
        m_LOS = los;
        m_SearchOnLOSLost = !los;
    }

    void UpdateThreat(bool force = false)
    {
        int time = GetGame().GetTime();
        if (!force && time - m_ThreatLevelUpdateTimestamp <= 300)
            return;

        m_ThreatLevelUpdateTimestamp = time;
        if (m_AI)
            m_ThreatLevel = m_AI.BQBot_CalculateTargetMemoryThreat(m_Target, m_SearchPosition, m_LOS, time);
        else if (!m_Target)
            m_ThreatLevel = 0.0;
        else
            m_ThreatLevel = 0.1;

        m_ThreatLevelActive = m_ThreatLevel;
    }

    void UpdatePosition(bool force = false)
    {
        int time = GetGame().GetTime();
        if (!force && time - m_SearchPositionUpdateTimestamp <= 300)
            return;

        m_SearchPositionUpdateTimestamp = time;

        if (m_Target)
        {
            m_LastKnownPosition = m_Target.GetPosition();
            m_SearchPosition = m_LastKnownPosition;
            m_SearchDirection = vector.Direction(GetAIPosition(), m_SearchPosition);
        }
        else
        {
            m_SearchPosition = GetAIPosition();
            m_SearchDirection = "0 0 1";
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
}
