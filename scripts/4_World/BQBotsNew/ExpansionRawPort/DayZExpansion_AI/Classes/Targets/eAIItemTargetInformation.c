class eAIItemTargetInformation: eAIEntityTargetInformation
{
	protected ItemBase m_Item;

	void eAIItemTargetInformation(EntityAI target)
	{
		Class.CastTo(m_Item, target);
	}

	void UpdateItemInfo()
	{
	}

	override bool IsInanimate()
	{
		return true;
	}

	override bool IsItem()
	{
		return true;
	}

	override bool IsExplosive()
	{
		return false;
	}

	override bool IsMechanicalTrap()
	{
		return false;
	}

	override bool IsActive()
	{
		if (!super.IsActive())
			return false;

		Man player = m_Item.GetHierarchyRootPlayer();
		if (player && player.IsAlive() && (!player.IsUnconscious() || !player.GetIdentity()))
			return false;

		return true;
	}

	override vector GetPosition(eAIBase ai = null, bool actual = false, eAITargetInformationState state = null)
	{
		return m_Item.GetCenter();
	}

	override float CalculateThreat(eAIBase ai = null, eAITargetInformationState state = null)
	{
		if (!m_Item || m_Item.IsDamageDestroyed() || m_Item.IsSetForDeletion())
			return 0.0;

		if (ai && ai.eAI_GetThreatOverride(m_Item))
			return 0.1;

		return 0.0;
	}

	override bool ShouldRemove(eAIBase ai = null)
	{
		return GetThreat(ai) <= 0.1;
	}

	override void OnHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		if (g_Game.IsClient())
			GetDayZGame().GetExpansionGame().m_FirearmFXHitObject = m_Item.GetHierarchyRoot();
	}
};