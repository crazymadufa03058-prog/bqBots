class eAIState_Fighting_Melee: eAIState
{
	override void OnEntry(string Event, ExpansionState From)
	{
		Print("[EAI] [state_enter] bot=" + unit.ToString() + " state=eAIState_Fighting_Melee event=" + Event + " disabled=1");
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		return EXIT;
	}

	int Guard()
	{
		//! Melee is intentionally disabled in this BQ adaptation. Fire/reload/unjam use BQ backend;
		//! melee can be ported later without affecting firearm movement points.
		return eAITransition.FAIL;
	}
}
