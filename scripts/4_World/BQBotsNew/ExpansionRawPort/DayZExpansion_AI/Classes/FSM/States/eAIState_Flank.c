class eAIState_Flank: eAIState
{
	eAITarget m_Target;

	override void OnEntry(string Event, ExpansionState From)
	{
		Print("[EAI] [state_enter] bot=" + unit.ToString() + " state=eAIState_Flank event=" + Event);
		unit.OverrideMovementDirection(false, 0);
		unit.OverrideMovementSpeed(false, 0);

		unit.eAI_SetShouldTakeCover(true);
	}

	override void OnExit(string Event, bool Aborted, ExpansionState To)
	{
		Print("[EAI] [state_exit] bot=" + unit.ToString() + " state=eAIState_Flank aborted=" + Aborted + " event=" + Event);
		unit.eAI_SetShouldTakeCover(false);
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		if (m_Target)
		{
			Print("[EAI] [flank_target] bot=" + unit.ToString() + " botPos=" + unit.GetPosition().ToString());
			unit.OverrideTargetPosition(m_Target);
		}
		
		return EXIT;
	}

	int Guard()
	{
		float threatSelf = unit.GetThreatToSelf();
		if (threatSelf >= 0.4)
		{
			Print(string.Format("[EAI] [flank_guard] bot=%1 result=FAIL reason=threatSelf_high threat=%2", unit.ToString(), threatSelf.ToString()));
			return eAITransition.FAIL;
		}
				
		if (unit.IsInTransport())
		{
			Print("[EAI] [flank_guard] bot=" + unit.ToString() + " result=FAIL reason=in_transport");
			return eAITransition.FAIL;
		}
		
		auto group = unit.GetGroup();
		if (!group)
		{
			Print("[EAI] [flank_guard] bot=" + unit.ToString() + " result=FAIL reason=no_group");
			return eAITransition.FAIL;
		}
		if (!unit.m_eAI_EnableFlankingOutsideCombat && group.GetFormationState() != eAIGroupFormationState.FLANK && !group.IsInCombat())
		{
			string inCombatStr = "false";
			if (group.IsInCombat())
				inCombatStr = "true";
			Print(string.Format("[EAI] [flank_guard] bot=%1 result=FAIL reason=flanking_disabled formation=%2 inCombat=%3", unit.ToString(), group.GetFormationState().ToString(), inCombatStr));
			return eAITransition.FAIL;
		}
		
		m_Target = unit.GetTarget();
		float threatAbsolute = unit.GetThreatToSelf(true);
		if (!m_Target || m_Target.IsItem() || threatAbsolute <= 0.152)
		{
			string isItemStr = "false";
			if (m_Target && m_Target.IsItem())
				isItemStr = "true";
			Print(string.Format("[EAI] [flank_guard] bot=%1 result=FAIL reason=target_invalid target=%2 isItem=%3 threatAbs=%4", unit.ToString(), m_Target.ToString(), isItemStr, threatAbsolute.ToString()));
			return eAITransition.FAIL;
		}
		
		//if (!unit.eAI_IsInFlankRange(m_Target)) return eAITransition.FAIL;
		
		string targetEntStr = "null";
		if (m_Target.GetEntity())
			targetEntStr = m_Target.GetEntity().ToString();
		Print(string.Format("[EAI] [flank_guard] bot=%1 result=SUCCESS target=%2", unit.ToString(), targetEntStr));
		return eAITransition.SUCCESS;
	}

	int GuardIdle()
	{
		auto group = unit.GetGroup();
		bool groupAllowsFlank = false;
		if (group)
		{
			if (unit.m_eAI_EnableFlankingOutsideCombat)
				groupAllowsFlank = true;
			else if (group.GetFormationState() == eAIGroupFormationState.FLANK)
				groupAllowsFlank = true;
			else if (group.IsInCombat())
				groupAllowsFlank = true;
		}

		bool validFlankTarget = m_Target && !m_Target.IsItem();
		bool stayInFlank = groupAllowsFlank && validFlankTarget && unit.GetThreatToSelf(true) > 0.152;
		if (stayInFlank)
		{
			Print("[EAI] [flank_guardidle] bot=" + unit.ToString() + " result=FAIL reason=stay_in_flank");
			return eAITransition.FAIL;
		}
		
		Print("[EAI] [flank_guardidle] bot=" + unit.ToString() + " result=SUCCESS reason=exit_flank");
		return eAITransition.SUCCESS;
	}
};
