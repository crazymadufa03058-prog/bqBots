typedef Class ExpansionFSMOwnerType;

class ExpansionFSM
{
	static const int EXIT = 0;
	static const int CONTINUE = 1;
	static const int RESTART = 2;

	protected ref array<ref ExpansionState> m_States = new array<ref ExpansionState>;
	protected ref array<ref ExpansionTransition> m_Transitions = new array<ref ExpansionTransition>;
	protected ExpansionState m_CurrentState;
	protected ExpansionState m_ParentState;
	protected ExpansionFSMOwnerType m_Owner;
	protected string m_Name;
	protected string m_DefaultState;

	void ExpansionFSM(ExpansionFSMOwnerType owner = null, ExpansionState parentState = null)
	{
		m_Owner = owner;
		m_ParentState = parentState;
	}

	ExpansionFSMOwnerType GetOwner()
	{
		return m_Owner;
	}

	string GetName()
	{
		return m_Name;
	}

	ExpansionState GetParent()
	{
		return m_ParentState;
	}

	void BQ_SetMeta(string name, string defaultState)
	{
		m_Name = name;
		m_DefaultState = defaultState;
	}

	void AddState(ExpansionState state)
	{
		m_States.Insert(state);
	}

	void AddTransition(ExpansionTransition transition)
	{
		ExpansionState src = transition.GetSource();
		if (src)
			src.AddTransition(transition);
		else
			m_Transitions.Insert(transition);
	}

	ExpansionState GetState()
	{
		return m_CurrentState;
	}

	ExpansionState GetState(string type)
	{
		for (int i = 0; i < m_States.Count(); i++)
		{
			if (m_States[i].ClassName() == type || m_States[i].GetName() == type || m_States[i].m_ClassName == type)
				return m_States[i];
		}
		return null;
	}

	ExpansionState GetState(typename type)
	{
		for (int i = 0; i < m_States.Count(); i++)
		{
			if (m_States[i].Type() == type)
				return m_States[i];
		}
		return null;
	}

	bool IsInState(string name)
	{
		return m_CurrentState && m_CurrentState.GetName() == name;
	}

	bool IsInState(typename type)
	{
		return m_CurrentState && m_CurrentState.IsInherited(type);
	}

	bool StartDefault()
	{
		if (m_DefaultState == "")
			return Start();

		ExpansionState dst = GetState(m_DefaultState);
		ExpansionState src = m_CurrentState;
		if (src && src != dst)
			src.OnExit("", true, dst);

		m_CurrentState = dst;
		if (m_CurrentState)
		{
			BQExpansionDiag.Log("FSM_DEFAULT", "fsm=" + m_Name + " state=" + m_CurrentState.GetName() + " class=" + m_CurrentState.m_ClassName);
			m_CurrentState.OnEntry("", src);
			return true;
		}
		return false;
	}

	bool Start(string e = "")
	{
		ExpansionState dst;
		FindSuitableTransition(m_CurrentState, e, dst);
		ExpansionState src = m_CurrentState;
		if (m_CurrentState && m_CurrentState != dst)
			m_CurrentState.OnExit(e, true, dst);
		m_CurrentState = dst;
		if (m_CurrentState && src != m_CurrentState)
		{
			m_CurrentState.OnEntry(e, src);
			return true;
		}
		return false;
	}

	bool Abort(string e = "")
	{
		if (!m_CurrentState)
			return false;
		m_CurrentState.OnExit(e, true, null);
		m_CurrentState = null;
		return true;
	}

	int Update(float pDt, int pSimulationPrecision)
	{
		if (!m_Owner)
			return EXIT;

		if (m_CurrentState)
		{
			BQExpansionDiag.Log("FSM_TICK", "fsm=" + m_Name + " state=" + m_CurrentState.GetName() + " class=" + m_CurrentState.m_ClassName);
			int result = m_CurrentState.OnUpdate(pDt, pSimulationPrecision);
			if (result == RESTART)
			{
				m_CurrentState.OnEntry("", m_CurrentState);
				return CONTINUE;
			}
			if (result == CONTINUE)
				return CONTINUE;
		}

		ExpansionState newState;
		bool found = FindSuitableTransition(m_CurrentState, "", newState);
		if (!found || (newState && m_CurrentState == newState))
			return CONTINUE;

		ExpansionState src = m_CurrentState;
		if (m_CurrentState)
		{
			m_CurrentState.OnExit("", false, newState);
			if (m_CurrentState.m_SubFSM)
				m_CurrentState.m_SubFSM.m_CurrentState = null;
		}

		m_CurrentState = newState;
		if (!m_CurrentState)
			return EXIT;

		string srcName = "NULL";
		if (src)
			srcName = src.GetName();
		BQExpansionDiag.Log("FSM_TRANSITION", "fsm=" + m_Name + " from=" + srcName + " to=" + m_CurrentState.GetName() + " class=" + m_CurrentState.m_ClassName);
		m_CurrentState.OnEntry("", src);
		return CONTINUE;
	}

	bool FindSuitableTransition(ExpansionState s, string e, out ExpansionState dst)
	{
		bool found;
		if (s)
			found = FindSuitableTransition(s.m_Transitions, e, dst);
		if (!found)
			found = FindSuitableTransition(m_Transitions, e, dst);
		return found;
	}

	bool FindSuitableTransition(array<ref ExpansionTransition> transitions, string e, out ExpansionState dst)
	{
		foreach (ExpansionTransition t: transitions)
		{
			if (e == "" || t.GetEvent() == e)
			{
				int guardResult = t.Guard();
				string srcName = "GLOBAL";
				if (t.GetSource())
					srcName = t.GetSource().GetName();
				string dstName = "NULL";
				if (t.GetDestination())
					dstName = t.GetDestination().GetName();
				string fsmGuardLog = "fsm=" + m_Name;
				fsmGuardLog = fsmGuardLog + " event=" + e;
				fsmGuardLog = fsmGuardLog + " from=" + srcName;
				fsmGuardLog = fsmGuardLog + " to=" + dstName;
				fsmGuardLog = fsmGuardLog + " result=" + guardResult.ToString();
				BQExpansionDiag.Log("FSM_GUARD", fsmGuardLog);
				if (guardResult == ExpansionTransition.SUCCESS)
				{
					dst = t.GetDestination();
					return true;
				}
			}
		}
		return false;
	}
}

class ExpansionState
{
	static const int EXIT = 0;
	static const int CONTINUE = 1;
	static const int RESTART = 2;

	string m_Name;
	string m_ClassName;
	ref ExpansionFSM m_SubFSM;
	ref array<ref ExpansionTransition> m_Transitions = new array<ref ExpansionTransition>;
	ExpansionState parent;

	void ExpansionState(ExpansionFSM fsm = null)
	{
		if (fsm)
			parent = fsm.GetParent();
	}

	void AddTransition(ExpansionTransition transition)
	{
		m_Transitions.Insert(transition);
	}

	string GetName()
	{
		return m_Name;
	}

	ExpansionFSM GetSubFSM()
	{
		return m_SubFSM;
	}

	void OnEntry(string Event, ExpansionState From) {}
	void OnExit(string Event, bool Aborted, ExpansionState To) {}
	int OnUpdate(float DeltaTime, int SimulationPrecision) { return CONTINUE; }
}

class ExpansionTransition
{
	static const int FAIL = 0;
	static const int SUCCESS = 1;
	protected string m_ClassName;

	void ExpansionTransition(ExpansionFSM fsm = null) {}
	ExpansionState GetSource() { return null; }
	ExpansionState GetDestination() { return null; }
	string GetEvent() { return ""; }
	int Guard() { return SUCCESS; }
}

class BQExpansionFSMTransition: ExpansionTransition
{
	static const int BQ_GUARD_SUCCESS = 0;
	static const int BQ_GUARD_DST = 1;
	static const int BQ_GUARD_SRC_IDLE = 2;
	static const int BQ_GUARD_FAIL = 3;
	static const int BQ_GUARD_DST_RELOAD_MASTER = 4;
	static const int BQ_GUARD_DST_RELOAD_SUB = 5;

	protected ExpansionState m_Source;
	protected ExpansionState m_Destination;
	protected int m_GuardKind;
	protected string m_Event;

	void BQExpansionFSMTransition(ExpansionFSM fsm, ExpansionState source, ExpansionState destination, int guardKind, string eventName = "")
	{
		m_Source = source;
		m_Destination = destination;
		m_GuardKind = guardKind;
		m_Event = eventName;
	}

	override ExpansionState GetSource()
	{
		return m_Source;
	}

	override ExpansionState GetDestination()
	{
		return m_Destination;
	}

	override string GetEvent()
	{
		return m_Event;
	}

	override int Guard()
	{
		if (m_GuardKind == BQ_GUARD_SUCCESS)
			return SUCCESS;
		if (m_GuardKind == BQ_GUARD_DST)
			return BQ_GuardState(m_Destination, false);
		if (m_GuardKind == BQ_GUARD_SRC_IDLE)
			return BQ_GuardState(m_Source, true);
		if (m_GuardKind == BQ_GUARD_DST_RELOAD_MASTER)
			return BQ_GuardReloadMaster(m_Destination);
		if (m_GuardKind == BQ_GUARD_DST_RELOAD_SUB)
			return BQ_GuardReloadSub(m_Destination);
		return FAIL;
	}


	protected int BQ_GuardReloadMaster(ExpansionState state)
	{
		if (!state)
			return FAIL;

		eAIState_Weapon_Reloading reloading;
		if (!Class.CastTo(reloading, state))
			return FAIL;

		eAIFSM subFSM;
		if (!Class.CastTo(subFSM, state.GetSubFSM()))
			return FAIL;

		return reloading.GuardEx(subFSM.last_attempt_time, subFSM.weapon, subFSM.magazine);
	}

	protected int BQ_GuardReloadSub(ExpansionState state)
	{
		if (!state)
			return FAIL;

		eAIState_Weapon_Reloading_Reloading reloading;
		if (!Class.CastTo(reloading, state))
			return FAIL;

		eAIState_Weapon_Reloading parentReload;
		if (!Class.CastTo(parentReload, state.parent))
			return FAIL;

		eAIFSM subFSM;
		if (!Class.CastTo(subFSM, parentReload.GetSubFSM()))
			return FAIL;

		return reloading.GuardEx(subFSM.weapon, subFSM.magazine);
	}

	protected int BQ_GuardState(ExpansionState state, bool idleGuard)
	{
		if (!state)
			return FAIL;

		eAIState_Fighting fighting;
		if (Class.CastTo(fighting, state))
		{
			if (idleGuard)
				return fighting.GuardIdle();
			return fighting.Guard();
		}

		eAIState_Fighting_Evade fightingEvade;
		if (Class.CastTo(fightingEvade, state) && !idleGuard)
			return fightingEvade.Guard();

		eAIState_Fighting_FireWeapon fightingFire;
		if (Class.CastTo(fightingFire, state) && !idleGuard)
			return fightingFire.Guard();

		eAIState_Flank flank;
		if (Class.CastTo(flank, state))
		{
			if (idleGuard)
				return flank.GuardIdle();
			return flank.Guard();
		}

		eAIState_FollowFormation follow;
		if (Class.CastTo(follow, state))
		{
			if (idleGuard)
				return follow.GuardIdle();
			return follow.Guard();
		}

		eAIState_TraversingWaypoints waypoints;
		if (Class.CastTo(waypoints, state))
		{
			if (idleGuard)
				return waypoints.GuardIdle();
			return waypoints.Guard();
		}

		eAIState_Dormant dormant;
		if (Class.CastTo(dormant, state) && !idleGuard)
			return dormant.Guard();

		eAIState_Leave leave;
		if (Class.CastTo(leave, state) && !idleGuard)
			return leave.Guard();

		eAIState_Interacting interacting;
		if (Class.CastTo(interacting, state))
		{
			if (idleGuard)
				return interacting.GuardIdle();
			return interacting.Guard();
		}

		eAIState_Struggle struggle;
		if (Class.CastTo(struggle, state) && !idleGuard)
			return struggle.Guard();

		eAIState_PlayEmote playEmote;
		if (Class.CastTo(playEmote, state) && !idleGuard)
			return playEmote.Guard();

		eAIState_Weapon_Unjamming unjamming;
		if (Class.CastTo(unjamming, state) && !idleGuard)
			return unjamming.Guard();

		eAIState_Fighting_Melee melee;
		if (Class.CastTo(melee, state) && !idleGuard)
			return melee.Guard();

		return FAIL;
	}
}

class BQExpansionFSMStateDef
{
	string Name;
	string ClassName;
	string SubFSMName;
}

class BQExpansionFSMTransitionDef
{
	ref array<string> FromNames = new array<string>;
	string ToName;
	string GuardText;
	string EventName;
}

class BQExpansionFSMXML
{
	string Name;
	string DefaultState;
	ref array<ref BQExpansionFSMStateDef> States = new array<ref BQExpansionFSMStateDef>;
	ref array<ref BQExpansionFSMTransitionDef> Transitions = new array<ref BQExpansionFSMTransitionDef>;
}

class BQExpansionFSMXMLLoader
{
	static ref BQExpansionFSMXML Load(string path, string name)
	{
		string fileName = BQResolveFSMPath(path, name);
		ref array<string> lines = new array<string>;

		if (!BQReadLines(fileName, lines))
		{
			Print("[EAI][FSM XML] failed to read " + fileName + ", using built-in XML-derived fallback for " + name);
			return BQCreateFallback(name);
		}

		ref BQExpansionFSMXML xml = BQParse(lines, name);
		if (!xml || xml.States.Count() == 0)
		{
			Print("[EAI][FSM XML] failed to parse " + fileName + ", using built-in XML-derived fallback for " + name);
			return BQCreateFallback(name);
		}

		Print("[EAI][FSM XML] loaded " + fileName + " states=" + xml.States.Count().ToString() + " transitions=" + xml.Transitions.Count().ToString());
		return xml;
	}

	protected static string BQResolveFSMPath(string path, string name)
	{
		string fileName = path + "/" + name + ".xml";
		if (FileExist(fileName))
			return fileName;

		fileName = "bqbots/FSM/" + name + ".xml";
		if (FileExist(fileName))
			return fileName;

		fileName = "bqBots/FSM/" + name + ".xml";
		if (FileExist(fileName))
			return fileName;

		return "bqbots/FSM/" + name + ".xml";
	}

	protected static bool BQReadLines(string fileName, array<string> lines)
	{
		FileHandle file = OpenFile(fileName, FileMode.READ);
		if (!file)
			return false;

		string line;
		while (FGets(file, line) >= 0)
		{
			lines.Insert(line);
		}

		CloseFile(file);
		return lines.Count() > 0;
	}

	protected static ref BQExpansionFSMXML BQParse(array<string> lines, string fallbackName)
	{
		ref BQExpansionFSMXML xml = new BQExpansionFSMXML();
		xml.Name = fallbackName;

		bool inStates = false;
		bool inTransitions = false;
		bool inTransition = false;
		bool inGuard = false;
		BQExpansionFSMTransitionDef transition;

		for (int i = 0; i < lines.Count(); i++)
		{
			string line = lines[i];

			if (line.Contains("<fsm "))
			{
				string fsmName = BQGetAttribute(line, "name");
				if (fsmName != "")
					xml.Name = fsmName;
			}

			if (line.Contains("<states"))
			{
				inStates = true;
				xml.DefaultState = BQGetAttribute(line, "default");
				continue;
			}

			if (line.Contains("</states>"))
			{
				inStates = false;
				continue;
			}

			if (line.Contains("<transitions"))
			{
				inTransitions = true;
				continue;
			}

			if (line.Contains("</transitions>"))
			{
				inTransitions = false;
				continue;
			}

			if (inStates && line.Contains("<state "))
			{
				ref BQExpansionFSMStateDef stateDef = new BQExpansionFSMStateDef();
				stateDef.Name = BQGetAttribute(line, "name");
				stateDef.ClassName = BQGetAttribute(line, "class");
				stateDef.SubFSMName = BQGetAttribute(line, "fsm");

				if (stateDef.Name != "")
					xml.States.Insert(stateDef);
				continue;
			}

			if (inTransitions && line.Contains("<transition>"))
			{
				transition = new BQExpansionFSMTransitionDef();
				inTransition = true;
				continue;
			}

			if (inTransitions && inTransition && line.Contains("</transition>"))
			{
				xml.Transitions.Insert(transition);
				transition = null;
				inTransition = false;
				inGuard = false;
				continue;
			}

			if (!inTransition || !transition)
				continue;

			if (line.Contains("<guard>"))
			{
				inGuard = true;
				continue;
			}

			if (line.Contains("</guard>"))
			{
				inGuard = false;
				continue;
			}

			if (inGuard)
			{
				transition.GuardText = transition.GuardText + line;
				continue;
			}

			if (line.Contains("<from_state"))
			{
				string fromAttr = BQGetAttribute(line, "name");
				if (fromAttr != "")
					fromAttr.Split(",", transition.FromNames);
				continue;
			}

			if (line.Contains("<to_state"))
			{
				transition.ToName = BQGetAttribute(line, "name");
				continue;
			}

			if (line.Contains("<event"))
			{
				transition.EventName = BQGetAttribute(line, "name");
				continue;
			}
		}

		return xml;
	}

	protected static string BQGetAttribute(string line, string attr)
	{
		string needle = attr + "=\"";
		int start = line.IndexOf(needle);
		if (start < 0)
			return "";

		start = start + needle.Length();
		string rest = line.Substring(start, line.Length() - start);
		int end = rest.IndexOf("\"");
		if (end < 0)
			return "";

		return rest.Substring(0, end);
	}

	protected static ref BQExpansionFSMXML BQCreateFallback(string name)
	{
		ref BQExpansionFSMXML xml = new BQExpansionFSMXML();
		xml.Name = name;

		if (name == "Fighting")
		{
			xml.DefaultState = "Positioning";
			BQAddState(xml, "Positioning", "eAIState_Fighting_Positioning", "");
			BQAddState(xml, "Evade", "eAIState_Fighting_Evade", "");
			BQAddState(xml, "FireWeapon", "eAIState_Fighting_FireWeapon", "");
			BQAddState(xml, "Melee", "eAIState_Fighting_Melee", "");

			BQAddTransition(xml, "", "Melee", "return dst.Guard();");
			BQAddTransition(xml, "", "Evade", "return dst.Guard();");
			BQAddTransition(xml, "", "FireWeapon", "return dst.Guard();");
			BQAddTransition(xml, "", "Positioning", "return SUCCESS;");
			return xml;
		}

		xml.DefaultState = "Idle";
		BQAddState(xml, "Idle", "eAIState_Idle", "");
		BQAddState(xml, "Dormant", "eAIState_Dormant", "");
		BQAddState(xml, "FollowFormation", "eAIState_FollowFormation", "");
		BQAddState(xml, "TraversingWaypoints", "eAIState_TraversingWaypoints", "");
		BQAddState(xml, "Flank", "eAIState_Flank", "");
		BQAddState(xml, "Fighting", "eAIState_Fighting", "Fighting");

		BQAddTransition(xml, "Idle,TraversingWaypoints,FollowFormation,Flank", "Fighting", "return dst.Guard();");
		BQAddTransition(xml, "Fighting", "Idle", "return src.GuardIdle();");
		BQAddTransition(xml, "Idle", "FollowFormation", "return dst.Guard();");
		BQAddTransition(xml, "Idle", "TraversingWaypoints", "return dst.Guard();");
		BQAddTransition(xml, "Idle,TraversingWaypoints,FollowFormation", "Flank", "return dst.Guard();");
		BQAddTransition(xml, "TraversingWaypoints", "Idle", "return src.GuardIdle();");
		BQAddTransition(xml, "FollowFormation", "Idle", "return src.GuardIdle();");
		BQAddTransition(xml, "Flank", "Idle", "return src.GuardIdle();");
		BQAddTransition(xml, "Idle,TraversingWaypoints,FollowFormation", "Dormant", "return dst.Guard();");
		BQAddTransition(xml, "Dormant", "Idle", "return SUCCESS;");
		return xml;
	}

	protected static void BQAddState(BQExpansionFSMXML xml, string name, string className, string subFSMName)
	{
		ref BQExpansionFSMStateDef stateDef = new BQExpansionFSMStateDef();
		stateDef.Name = name;
		stateDef.ClassName = className;
		stateDef.SubFSMName = subFSMName;
		xml.States.Insert(stateDef);
	}

	protected static void BQAddTransition(BQExpansionFSMXML xml, string fromNames, string toName, string guardText)
	{
		ref BQExpansionFSMTransitionDef transition = new BQExpansionFSMTransitionDef();
		if (fromNames != "")
			fromNames.Split(",", transition.FromNames);
		transition.ToName = toName;
		transition.GuardText = guardText;
		xml.Transitions.Insert(transition);
	}
}

class BQExpansionFSMFactory
{
	static void Setup(eAIFSM fsm, ExpansionFSMType type)
	{
		if (!type || !type.m_BQXML)
		{
			SetupFallback(fsm, "Master");
			return;
		}

		BQSetupFromXML(fsm, type);
	}

	protected static ExpansionState AddState(ExpansionFSM fsm, ExpansionState state, string name, string className)
	{
		state.m_Name = name;
		state.m_ClassName = className;
		fsm.AddState(state);
		return state;
	}

	protected static void AddTransition(ExpansionFSM fsm, ExpansionState source, ExpansionState destination, int guardKind, string eventName = "")
	{
		fsm.AddTransition(new BQExpansionFSMTransition(fsm, source, destination, guardKind, eventName));
	}

	protected static void BQSetupFromXML(eAIFSM fsm, ExpansionFSMType type)
	{
		BQExpansionFSMXML xml = type.m_BQXML;
		fsm.BQ_SetMeta(xml.Name, xml.DefaultState);

		for (int i = 0; i < xml.States.Count(); i++)
		{
			BQExpansionFSMStateDef stateDef = xml.States[i];
			ExpansionState state = BQCreateState(fsm, stateDef.ClassName);
			AddState(fsm, state, stateDef.Name, stateDef.ClassName);

			if (stateDef.SubFSMName != "")
				BQAttachSubFSM(type, fsm, state, stateDef.SubFSMName);
		}

		for (int j = 0; j < xml.Transitions.Count(); j++)
		{
			BQExpansionFSMTransitionDef transitionDef = xml.Transitions[j];
			ExpansionState dst = null;
			if (transitionDef.ToName != "")
				dst = fsm.GetState(transitionDef.ToName);

			int guardKind = BQGuardKindFromText(transitionDef.GuardText);

			if (transitionDef.FromNames.Count() == 0)
			{
				AddTransition(fsm, null, dst, guardKind, transitionDef.EventName);
				continue;
			}

			for (int k = 0; k < transitionDef.FromNames.Count(); k++)
			{
				string sourceName = transitionDef.FromNames[k];
				ExpansionState src = fsm.GetState(sourceName);
				if (src)
					AddTransition(fsm, src, dst, guardKind, transitionDef.EventName);
			}
		}
	}

	protected static void BQAttachSubFSM(ExpansionFSMType parentType, eAIFSM fsm, ExpansionState state, string subFSMName)
	{
		if (subFSMName != "Fighting" && subFSMName != "Reloading" && subFSMName != "Vehicles")
			return;

		ExpansionFSMType subType = ExpansionFSMType.LoadXML(parentType.m_BQPath, subFSMName);
		if (!subType)
			return;

		ExpansionFSM subFSM = subType.Spawn(fsm.GetOwner(), state);
		if (subFSM)
			state.m_SubFSM = subFSM;
	}

	protected static ExpansionState BQCreateState(ExpansionFSM fsm, string className)
	{
		if (className == "eAIState_Idle")
			return new eAIState_Idle(fsm);
		if (className == "eAIState_Dormant")
			return new eAIState_Dormant(fsm);
		if (className == "eAIState_Leave")
			return new eAIState_Leave(fsm);
		if (className == "eAIState_Interacting")
			return new eAIState_Interacting(fsm);
		if (className == "eAIState_FollowFormation")
			return new eAIState_FollowFormation(fsm);
		if (className == "eAIState_TraversingWaypoints")
			return new eAIState_TraversingWaypoints(fsm);
		if (className == "eAIState_Flank")
			return new eAIState_Flank(fsm);
		if (className == "eAIState_Fighting")
			return new eAIState_Fighting(fsm);
		if (className == "eAIState_Struggle")
			return new eAIState_Struggle(fsm);
		if (className == "eAIState_PlayEmote")
			return new eAIState_PlayEmote(fsm);
		if (className == "eAIState_Fighting_Positioning")
			return new eAIState_Fighting_Positioning(fsm);
		if (className == "eAIState_Fighting_Evade")
			return new eAIState_Fighting_Evade(fsm);
		if (className == "eAIState_Fighting_FireWeapon")
			return new eAIState_Fighting_FireWeapon(fsm);
		if (className == "eAIState_Fighting_Melee")
			return new eAIState_Fighting_Melee(fsm);
		if (className == "eAIState_Weapon_Reloading")
			return new eAIState_Weapon_Reloading(fsm);
		if (className == "eAIState_Weapon_Unjamming")
			return new eAIState_Weapon_Unjamming(fsm);
		if (className == "eAIState_Weapon_Reloading_Start")
			return new eAIState_Weapon_Reloading_Start(fsm);
		if (className == "eAIState_Weapon_Reloading_Reloading")
			return new eAIState_Weapon_Reloading_Reloading(fsm);
		if (className == "eAIState_Weapon_Reloading_Fail")
			return new eAIState_Weapon_Reloading_Fail(fsm);
		if (className == "eAIState_Weapon_Reloading_Removing")
			return new eAIState_Weapon_Reloading_Removing(fsm);

		// Missing states from the full Expansion stack are intentionally represented by a generic state.
		// Their dst.Guard()/GuardEx() transitions resolve to FAIL, so they cannot steal movement/combat flow.
		return new ExpansionState(fsm);
	}

	protected static int BQGuardKindFromText(string guardText)
	{
		if (guardText.Contains("dst.GuardEx(dst.sub_fsm.last_attempt_time"))
			return BQExpansionFSMTransition.BQ_GUARD_DST_RELOAD_MASTER;
		if (guardText.Contains("dst.GuardEx(fsm.weapon"))
			return BQExpansionFSMTransition.BQ_GUARD_DST_RELOAD_SUB;
		if (guardText.Contains("return SUCCESS"))
			return BQExpansionFSMTransition.BQ_GUARD_SUCCESS;
		if (guardText.Contains("dst.Guard()"))
			return BQExpansionFSMTransition.BQ_GUARD_DST;
		if (guardText.Contains("src.GuardIdle()"))
			return BQExpansionFSMTransition.BQ_GUARD_SRC_IDLE;

		return BQExpansionFSMTransition.BQ_GUARD_FAIL;
	}

	protected static void SetupFallback(eAIFSM fsm, string name)
	{
		ExpansionFSMType type = new ExpansionFSMType();
		type.m_BQName = name;
		type.m_BQPath = "bqbots/FSM";
		type.m_BQXML = BQExpansionFSMXMLLoader.Load(type.m_BQPath, name);
		BQSetupFromXML(fsm, type);
	}
}

class ExpansionFSMType
{
	string m_BQName;
	string m_BQPath;
	ref BQExpansionFSMXML m_BQXML;

	static void UnloadAll() {}

	static ExpansionFSMType LoadXML(string path, string name)
	{
		ExpansionFSMType type = new ExpansionFSMType();
		type.m_BQName = name;
		type.m_BQPath = path;
		type.m_BQXML = BQExpansionFSMXMLLoader.Load(path, name);
		return type;
	}

	ExpansionFSM Spawn(ExpansionFSMOwnerType owner, ExpansionState parentState)
	{
		eAIFSM fsm = new eAIFSM(owner, parentState);
		eAIBase unit;
		if (Class.CastTo(unit, owner))
			fsm.Init(unit);

		BQExpansionFSMFactory.Setup(fsm, this);
		return fsm;
	}
}
