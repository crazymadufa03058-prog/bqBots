class eAIState : ExpansionState
{
	eAIBase unit;

	void eAIState(ExpansionFSM fsm)
	{
		Class.CastTo(unit, fsm.GetOwner());
	}
};
