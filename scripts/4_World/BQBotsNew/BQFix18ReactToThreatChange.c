modded class eAIBase
{
	void ReactToThreatChange(float pDt, EntityAI entityInHands = null)
	{
		// Compatibility port of Expansion threat reaction for bqBots fix18.
		// Keeps the movement/FSM-critical behaviour without depending on the full Expansion Core stack.
		if (m_eAI_CurrentThreatToSelfActive >= 0.4)
		{
			if (m_ThreatClearedTimeout <= 0)
				m_ThreatClearedTimeout = Math.RandomFloat(1.0, 3.0);
		}
		else
		{
			if (m_ThreatClearedTimeout > 0)
			{
				m_ThreatClearedTimeout = m_ThreatClearedTimeout - pDt;
				if (m_ThreatClearedTimeout < 0)
					m_ThreatClearedTimeout = 0;
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
					eAI_RaiseWeapon(false);
			}
		}
	}
}
