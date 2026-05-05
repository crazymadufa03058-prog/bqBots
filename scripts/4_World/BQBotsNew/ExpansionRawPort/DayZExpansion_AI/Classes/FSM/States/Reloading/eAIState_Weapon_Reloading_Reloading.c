class eAIState_Weapon_Reloading_Reloading: eAIState
{
	Weapon_Base m_Weapon;
	Magazine m_Magazine;
	float m_Time;

	override void OnEntry(string Event, ExpansionState From)
	{
		Print("[EAI] [state_enter] bot=" + unit.ToString() + " state=eAIState_Weapon_Reloading_Reloading");
		m_Time = 0;
		unit.eAI_RaiseWeapon(false);
		unit.BQ_EAIRequestReload(m_Weapon, m_Magazine);
	}

	override void OnExit(string Event, bool Aborted, ExpansionState To)
	{
		Print("[EAI] [state_exit] bot=" + unit.ToString() + " state=eAIState_Weapon_Reloading_Reloading aborted=" + Aborted);
		if (unit.eAI_ShouldGetUp())
			unit.Expansion_GetUp();
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		if (!m_Weapon || unit.IsUnconscious())
			return EXIT;

		m_Time += DeltaTime;
		unit.BQ_EAIRequestReload(m_Weapon, m_Magazine);

		eAITarget target = unit.GetTarget();
		if (target && unit.GetThreatToSelf() >= 0.4)
		{
			if (!unit.m_eAI_IsInCover)
				unit.OverrideTargetPosition(target, true);

			if (target.m_LOS && target.GetDistanceSq(true) <= 9.0)
			{
				float movementDirection;
				if (Math.RandomIntInclusive(0, 1))
					movementDirection = Math.RandomFloat(135, 180);
				else
					movementDirection = Math.RandomFloat(-135, -180);
				unit.OverrideMovementDirection(true, movementDirection);
				unit.OverrideMovementSpeed(true, 2);
			}
		}

		if (unit.BQ_EAIWeaponIsChambered(m_Weapon))
			return EXIT;

		if (m_Time > 8.0)
			return EXIT;

		return CONTINUE;
	}

	int GuardEx(Weapon_Base weapon, inout Magazine magazine)
	{
		if (!weapon || weapon.IsDamageDestroyed())
			return eAITransition.FAIL;

		m_Weapon = weapon;
		m_Magazine = magazine;
		return eAITransition.SUCCESS;
	}
}
