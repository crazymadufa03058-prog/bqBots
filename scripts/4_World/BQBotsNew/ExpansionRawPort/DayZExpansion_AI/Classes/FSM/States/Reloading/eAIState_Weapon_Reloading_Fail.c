class eAIState_Weapon_Reloading_Fail: eAIState
{
	float m_Time;

	override void OnEntry(string Event, ExpansionState From)
	{
		unit.eAI_RaiseWeapon(false);
		m_Time = 0;
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		m_Time += DeltaTime;
		if (m_Time < 0.5)
			return CONTINUE;
		return EXIT;
	}
}
