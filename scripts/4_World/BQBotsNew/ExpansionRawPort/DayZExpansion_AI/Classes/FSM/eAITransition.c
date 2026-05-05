class eAITransition : ExpansionTransition
{
	eAIBase unit;

	void eAITransition(ExpansionFSM fsm)
	{
		Class.CastTo(unit, fsm.GetOwner());
	}
};
