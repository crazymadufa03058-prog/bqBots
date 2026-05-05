class eAIZombieTargetInformation: eAIEntityTargetInformation
{
	private ZombieBase m_Zombie;

	bool m_Crawling;

	void eAIZombieTargetInformation(EntityAI target)
	{
		Class.CastTo(m_Zombie, target);
	}

	override bool IsZombie()
	{
		return true;
	}

	override bool IsCrawling()
	{
		return m_Crawling;
	}

	override bool IsFighting()
	{
		if (m_Zombie.GetCommand_Attack())
			return true;

		return false;
	}

	override bool IsAcuteDanger(eAIBase ai = null)
	{
		return IsFighting();
	}

	override float GetAttackCooldown()
	{
		return 0.0;
	}

	override vector GetAimOffset(eAIBase ai = null)
	{
		string boneName;

		Weapon_Base weapon;
		if (ai && Class.CastTo(weapon, ai.GetHumanInventory().GetEntityInHands()))
		{
			if (weapon.ShootsExplosiveAmmo())
				boneName = "spine3";
			else
				boneName = "neck";
		}
		else if (m_Crawling)
		{
			boneName = "pelvis";  //! Aim even lower for melee if crawling
		}
		else
		{
			boneName = "spine3";  //! Aim lower for melee
		}

		vector pos;
		int index = m_Zombie.GetBoneIndexByName(boneName);
		if (index > -1)
			pos = m_Zombie.GetBonePositionWS(index);
		else
			pos = m_Zombie.GetCenter();

		pos = pos - m_Zombie.GetPosition();

		return pos;
	}




	// https://www.desmos.com/calculator/r4mqu91qff
	override float CalculateThreat(eAIBase ai = null, eAITargetInformationState state = null)
	{
		if (m_Zombie.IsDamageDestroyed())
			return 0.0;

		float levelFactor;
		if (m_Zombie.GetCommand_Attack())
			levelFactor = 2.0;
		else if (ai && GetVelocity(m_Zombie).LengthSq() > 0.1388)
			levelFactor = 0.25;
		else
			return 0.0;

		if (ai)
		{
			if (ai.eAI_GetThreatOverride(m_Target))
				return 0.0;

			float distance = GetDistance(ai, true);
			levelFactor *= 10 / (distance + 0.1);
			if (levelFactor > 1.0)
				levelFactor = Math.Pow(levelFactor, 2.0);

			levelFactor *= ai.eAI_GetThreatDistanceFactor(distance);
		}

		return Math.Clamp(levelFactor, 0.0, 1000000.0);
	}

	override bool ShouldRemove(eAIBase ai = null)
	{
		return GetThreat(ai) <= 0.1;
	}

	override float GetMinDistance(eAIBase ai = null, float distance = 0.0)
	{
		//! @note at health below 30%, we are too slow to flee from Zs that are close and are better off fighting
		if (ai)
		{
			if (ai.m_eAI_AcuteDangerTargetCount > 1 && (ai.GetHealth01("", "") >= 0.3 || GetDistanceSq(ai, true) > 25))
				return 100.0;  //! Flee

			if (ai.m_eAI_AcuteDangerTargetCount <= 1 && ai.eAI_IsLowVitals() && GetDistanceSq(ai, true) > 25)
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
