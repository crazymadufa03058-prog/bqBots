class eAICreatureTargetInformation: eAIEntityTargetInformation
{
	private AnimalBase m_Creature;
	bool m_IsActive = true;

	void eAICreatureTargetInformation(EntityAI target)
	{
		Class.CastTo(m_Creature, target);
	}

	override bool IsCreature()
	{
		return true;
	}

	override bool IsFighting()
	{
		return false;
	}

	override bool IsAcuteDanger(eAIBase ai = null)
	{
		return false;
	}

	override float GetAttackCooldown()
	{
		return 0.0;
	}

	override vector GetAimOffset(eAIBase ai = null)
	{
		vector pos;

		int index = m_Creature.GetBoneIndexByName("Chest");
		if (index > -1)
			pos = m_Creature.GetBonePositionWS(index);
		else
			pos = m_Creature.GetCenter();

		pos = pos - m_Creature.GetPosition();

	#ifdef DIAG_DEVELOPER
		if (EXTrace.AI && ai)
			ai.Expansion_DebugObject(1234567890, m_Creature.GetPosition() + pos, "ExpansionDebugSphereSmall_Orange", m_Creature.GetDirection(), ai.GetPosition() + "0 1.5 0", 3.0, ShapeFlags.NOZBUFFER);
	#endif

		return pos;
	}




	override bool IsActive()
	{
		if (!m_Creature)
			return false;

		return m_IsActive;
	}

	override float CalculateThreat(eAIBase ai = null, eAITargetInformationState state = null)
	{
		if (m_Creature.IsDamageDestroyed())
		{
			m_IsActive = false;
		}

		return 0.0;
	}

	override bool ShouldRemove(eAIBase ai = null)
	{
		return GetThreat(ai) <= 0.1;
	}

	override float GetMinDistance(eAIBase ai = null, float distance = 0.0)
	{
		if (ai && m_Creature.IsAlive())
		{
			if (ai.m_eAI_AcuteDangerTargetCount > ai.GetGroup().Count() + 1)
				return 100.0;  //! Flee

			if (ai.eAI_IsLowVitals())
				return 100.0;  //! Flee
		}

		return m_MinDistance;
	}

	override bool ShouldAvoid(eAIBase ai = null, float distance = 0.0)
	{
		if (ai && !ai.IsRaised() && GetMinDistance(ai, distance) > 0.0)
			return true;

		return false;
	}
};
