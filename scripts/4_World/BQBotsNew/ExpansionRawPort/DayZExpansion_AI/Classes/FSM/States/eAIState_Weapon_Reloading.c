class eAIState_Weapon_Reloading: eAIState
{
	override void OnEntry(string Event, ExpansionState From)
	{
		Print("[EAI] [state_enter] bot=" + unit.ToString() + " state=eAIState_Weapon_Reloading event=" + Event);
		if (unit.GetEmoteManager().IsEmotePlaying())
			unit.GetEmoteManager().ServerRequestEmoteCancel();

		unit.eAI_RaiseWeapon(false);
		unit.OverrideMovementSpeed(false, 0);

		eAIFSM sub;
		if (Class.CastTo(sub, m_SubFSM))
			sub.last_attempt_time = g_Game.GetTime();

		if (m_SubFSM)
			m_SubFSM.StartDefault();
	}

	override void OnExit(string Event, bool Aborted, ExpansionState To)
	{
		Print("[EAI] [state_exit] bot=" + unit.ToString() + " state=eAIState_Weapon_Reloading aborted=" + Aborted);
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		if (!m_SubFSM)
			return EXIT;

		int result = m_SubFSM.Update(DeltaTime, SimulationPrecision);
		if (result == EXIT)
			return EXIT;

		return CONTINUE;
	}

	int GuardEx(int lastAttemptTime, inout Weapon_Base weapon, inout Magazine magazine)
	{
		if (g_Game.GetTime() - lastAttemptTime < 1000)
			return eAITransition.FAIL;

		if (unit.IsClimbing() || unit.IsFalling() || unit.IsFighting() || !unit.GetCommand_Move())
			return eAITransition.FAIL;

		if (unit.IsRestrained() || unit.IsUnconscious() || unit.IsSwimming())
			return eAITransition.FAIL;

		if (!Class.CastTo(weapon, unit.GetItemInHands()))
			return eAITransition.FAIL;

		if (weapon.IsDamageDestroyed())
			return eAITransition.SUCCESS;

		WeaponManager weaponManager = unit.GetWeaponManager();
		if (weaponManager && weaponManager.CanUnjam(weapon))
			return eAITransition.FAIL;

		if (unit.GetActionManager() && unit.GetActionManager().GetRunningAction())
			return eAITransition.FAIL;

		if (unit.BQ_EAIWeaponNeedsReload(weapon, magazine))
			return eAITransition.SUCCESS;

		return eAITransition.FAIL;
	}
}
