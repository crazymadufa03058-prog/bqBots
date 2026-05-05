class eAIState_Fighting: eAIState
{
	override void OnEntry(string Event, ExpansionState From)
	{
		string msg = "[EAI] [state_enter] bot=" + unit.ToString();
		msg = msg + " state=eAIState_Fighting event=" + Event;
		Print(msg);

		if (unit.GetEmoteManager().IsEmotePlaying())
			unit.GetEmoteManager().ServerRequestEmoteCancel();

		unit.eAI_SetIsFightingFSM(true);
		if (m_SubFSM)
			m_SubFSM.StartDefault();
	}

	override void OnExit(string Event, bool Aborted, ExpansionState To)
	{
		string msg = "[EAI] [state_exit] bot=" + unit.ToString();
		msg = msg + " state=eAIState_Fighting aborted=" + Aborted;
		msg = msg + " event=" + Event;
		Print(msg);

		unit.eAI_SetIsFightingFSM(false);
		unit.m_eAI_IsPreparingMelee = false;
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		if (m_SubFSM)
			m_SubFSM.Update(DeltaTime, SimulationPrecision);

		return EXIT;
	}

	int Guard()
	{
		if (unit.IsRestrained())
			return eAITransition.FAIL;
		if (unit.IsUnconscious())
			return eAITransition.FAIL;
		if (unit.GetThreatToSelf() < 0.4)
			return eAITransition.FAIL;
		if (unit.GetActionManager().GetRunningAction())
			return eAITransition.FAIL;
		if (unit.IsInTransport())
			return eAITransition.FAIL;

		return eAITransition.SUCCESS;
	}

	int GuardIdle()
	{
		if (unit.GetThreatToSelf() > 0.2)
			return eAITransition.FAIL;
		if (unit.IsFighting())
			return eAITransition.FAIL;

		return eAITransition.SUCCESS;
	}
}