class eAIState_Weapon_Reloading_Start: eAIState
{
	override void OnEntry(string Event, ExpansionState From)
	{
		eAIFSM fsm;
		if (Class.CastTo(fsm, parent.GetSubFSM()))
			fsm.last_attempt_time = g_Game.GetTime();

		unit.OverrideMovementSpeed(false, 0);
		unit.eAI_RaiseWeapon(false);
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		return EXIT;
	}
}
