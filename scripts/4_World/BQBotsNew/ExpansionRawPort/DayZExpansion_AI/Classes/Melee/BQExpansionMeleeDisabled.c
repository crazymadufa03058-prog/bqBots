//! bqBots does not use Expansion melee gameplay. These classes only satisfy eAIBase fields while keeping melee inactive.
class eAIMeleeCombat : DayZPlayerImplementMeleeCombat
{
	void eAIMeleeCombat(DayZPlayerImplement player)
	{
		Init(player);
	}

	float eAI_GetRange()
	{
		return 0.0;
	}

	float eAI_GetRangeSq()
	{
		return 0.0;
	}

	InventoryItem eAI_GetWeapon()
	{
		return null;
	}
}

class eAIMeleeFightLogic_LightHeavy: DayZPlayerMeleeFightLogic_LightHeavy
{
	bool m_eAI_Melee;
	bool m_eAI_IsInCombo;
	int m_eAI_BlockEndTime;
	int m_eAI_ComboCount;
	float m_eAI_ShoryukenChance;
	bool m_eAI_ShoryukenPrep;
	bool m_eAI_Shoryuken;
	float m_eAI_ShoryukenDamageMultiplier = 1.0;
	ref Timer m_eAI_ShoryukenSpinTimer;
	bool m_eAI_ShoryukenSpin;
	float m_eAI_ShoryukenSpinAngle;

	void eAIMeleeFightLogic_LightHeavy(DayZPlayerImplement player)
	{
		Init(player);
	}

	override bool HandleFightLogic(int pCurrentCommandID, HumanInputController pInputs, EntityAI pEntityInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	{
		m_eAI_Melee = false;
		pContinueAttack = false;
		SetBlock(false);
		return false;
	}

	void eAI_EndBlock()
	{
		SetBlock(false);
	}
}