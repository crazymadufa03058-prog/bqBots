// Variant A compile-stub only. Movement is owned by Expansion eAICommandMove.
class BQBotCommandMove extends HumanCommandScript
{
	protected Human m_Human;
	protected bqBots_Boris m_Player;
	protected ref bqCombatPathHandler m_PathFinding;
	protected bool m_Valid;
	protected bool m_Raised;

	void BQBotCommandMove(Human pHuman)
	{
		m_Human = pHuman;
		m_Player = bqBots_Boris.Cast(pHuman);
		m_Valid = true;
		m_Raised = false;
		if (m_Player)
			m_PathFinding = new bqCombatPathHandler(m_Player);
	}

	void SetRaised(bool raised)
	{
		m_Raised = raised;
	}

	void Invalidate()
	{
		m_Valid = false;
	}

	bool IsValid()
	{
		return m_Valid;
	}

	bqCombatPathHandler BQBot_GetPathFinding()
	{
		return m_PathFinding;
	}

	void PreAnimUpdate(float pDt)
	{
	}
}
