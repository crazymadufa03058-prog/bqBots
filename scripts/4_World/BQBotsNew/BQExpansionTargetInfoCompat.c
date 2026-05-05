//! Minimal Expansion AI target-information hooks used by the raw target system.
//! Damage handlers and vehicle targeting are intentionally not ported for bqBots.

modded class ZombieBase
{
	protected ref eAIZombieTargetInformation m_BQExpansion_TargetInformation;

	eAIZombieTargetInformation GetTargetInformation()
	{
		if (!m_BQExpansion_TargetInformation)
			m_BQExpansion_TargetInformation = new eAIZombieTargetInformation(this);

		return m_BQExpansion_TargetInformation;
	}
}

modded class AnimalBase
{
	protected ref eAICreatureTargetInformation m_BQExpansion_TargetInformation;

	eAICreatureTargetInformation GetTargetInformation()
	{
		if (!m_BQExpansion_TargetInformation)
			m_BQExpansion_TargetInformation = new eAICreatureTargetInformation(this);

		return m_BQExpansion_TargetInformation;
	}
}

modded class ItemBase
{
	protected ref eAIItemTargetInformation m_BQExpansion_TargetInformation;

	eAIItemTargetInformation GetTargetInformation()
	{
		if (!m_BQExpansion_TargetInformation)
			m_BQExpansion_TargetInformation = new eAIItemTargetInformation(this);

		return m_BQExpansion_TargetInformation;
	}
}