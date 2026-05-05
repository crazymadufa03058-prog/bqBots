class eAIState_Weapon_Unjamming: eAIState
{
	Weapon_Base m_Weapon;
	float m_Time;

	override void OnEntry(string Event, ExpansionState From)
	{
		Print("[EAI] [state_enter] bot=" + unit.ToString() + " state=eAIState_Weapon_Unjamming event=" + Event);
		m_Time = 0;
		unit.eAI_RaiseWeapon(false);
		unit.BQ_EAIRequestUnjam(m_Weapon);
	}

	override void OnExit(string Event, bool Aborted, ExpansionState To)
	{
		Print("[EAI] [state_exit] bot=" + unit.ToString() + " state=eAIState_Weapon_Unjamming aborted=" + Aborted);
		if (unit.eAI_ShouldGetUp())
			unit.Expansion_GetUp();
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		if (!m_Weapon || unit.IsUnconscious())
			return EXIT;

		m_Time += DeltaTime;
		unit.BQ_EAIRequestUnjam(m_Weapon);

		eAITarget target = unit.GetTarget();
		if (target && unit.GetThreatToSelf() >= 0.4)
		{
			if (!unit.m_eAI_IsInCover)
				unit.OverrideTargetPosition(target, true);
		}

		WeaponManager weaponManager = unit.GetWeaponManager();
		if (!weaponManager || !weaponManager.CanUnjam(m_Weapon))
			return EXIT;

		if (m_Time > 3.0)
		{
			m_Weapon.SetJammed(false);
			return EXIT;
		}

		return CONTINUE;
	}

	int Guard()
	{
		if (unit.IsClimbing() || unit.IsFalling() || unit.IsFighting() || !unit.GetCommand_Move())
			return eAITransition.FAIL;

		if (unit.IsRestrained() || unit.IsUnconscious() || unit.IsSwimming())
			return eAITransition.FAIL;

		if (!Class.CastTo(m_Weapon, unit.GetItemInHands()))
			return eAITransition.FAIL;

		WeaponManager weaponManager = unit.GetWeaponManager();
		if (!weaponManager || !weaponManager.CanUnjam(m_Weapon))
			return eAITransition.FAIL;

		if (unit.GetActionManager() && unit.GetActionManager().GetRunningAction())
			return eAITransition.FAIL;

		return eAITransition.SUCCESS;
	}
}
