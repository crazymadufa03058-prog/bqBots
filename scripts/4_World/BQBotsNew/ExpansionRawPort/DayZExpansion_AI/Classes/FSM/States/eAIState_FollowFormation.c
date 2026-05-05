class eAIState_FollowFormation: eAIState
{
	eAIGroup m_Group;
	int m_SpeedLimit;
	bool m_GotUp;

	override void OnEntry(string Event, ExpansionState From)
	{
		Print("[EAI] [state_enter] bot=" + unit.ToString() + " state=eAIState_FollowFormation event=" + Event);
		m_GotUp = false;
		unit.eAI_LeaveCover();
		unit.OverrideMovementDirection(false, 0);
		unit.OverrideMovementSpeed(false, 0);
		m_SpeedLimit = unit.GetMovementSpeedLimit();
		if (m_SpeedLimit > 0)
			unit.SetMovementSpeedLimit(3);  //! Make sure AI can reach formation positions when following leader by overriding speed limit
	}

	override void OnExit(string Event, bool Aborted, ExpansionState To)
	{
		Print("[EAI] [state_exit] bot=" + unit.ToString() + " state=eAIState_FollowFormation aborted=" + Aborted + " event=" + Event);
		if (m_SpeedLimit > 0)
			unit.SetMovementSpeedLimit(unit.m_eAI_SpeedLimitPreference);  //! Restore speed limit
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		if (!m_Group) return EXIT;
		vector slotPos = m_Group.GetFormationPosition(unit);
		eAIFormation form = m_Group.GetFormation();
		string formName;
		if (form)
			formName = form.ClassName();
		else
			formName = "null";
		Print("[EAI] [formation_follow] bot=" + unit.ToString() + " targetSlot=" + slotPos.ToString() + " botPos=" + unit.GetPosition().ToString() + " formation=" + formName);
		unit.OverrideTargetPosition(slotPos);
		
		if (!m_GotUp && unit.Expansion_GetMovementSpeed() > 0)
		{
			unit.Expansion_GetUp(true);
			m_GotUp = true;
		}
		
		return EXIT;
	}

	int Guard()
	{
		m_Group = unit.GetGroup();
		if (!m_Group) return eAITransition.FAIL;
		
		if (unit.GetThreatToSelf() >= 0.4) return eAITransition.FAIL;
		
		if (m_Group.IsInCombat())
		{
			eAITarget target = unit.GetTarget();
			if (target && !target.IsItem() && unit.GetThreatToSelf(true) >= 0.4) return eAITransition.FAIL;
		}
		
		if (m_Group.GetFormationState() != eAIGroupFormationState.IN) return eAITransition.FAIL;
		
		auto leader = m_Group.GetFormationLeader();
		if (!leader || leader == unit) return eAITransition.FAIL;
		
		if (leader.IsInTransport()) return eAITransition.FAIL;
		
		return eAITransition.SUCCESS;
	}

	int GuardIdle()
	{
		auto group = unit.GetGroup();
		if (!group || group.GetFormationLeader() == unit || group.GetFormationState() != eAIGroupFormationState.IN) return eAITransition.SUCCESS;
		
		if (unit.GetThreatToSelf() >= 0.4) return eAITransition.SUCCESS;
		
		return eAITransition.FAIL;
	}
};
