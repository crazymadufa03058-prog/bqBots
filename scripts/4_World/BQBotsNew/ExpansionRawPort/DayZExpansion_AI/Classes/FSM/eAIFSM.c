class eAIFSM : ExpansionFSM
{
	protected eAIBase m_Unit;

	//! XML FSM compatibility data used by Reloading/Vehicles sub-FSMs.
	int last_attempt_time;
	Weapon_Base weapon;
	Magazine magazine;
	EntityAI entity;
	int seat;

	void Init(eAIBase unit)
	{
		m_Unit = unit;
	}

	eAIBase GetUnit()
	{
		#ifdef EAI_TRACE
		#endif

		return m_Unit;
	}
};
