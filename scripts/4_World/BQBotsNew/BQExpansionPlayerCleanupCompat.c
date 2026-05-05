//! Minimal base hooks required by Expansion eAIBase overrides.
//! Full Expansion DayZPlayerImplement is intentionally not ported because bqBots uses its own damage/fire pipeline.
modded class DayZPlayerImplement
{
	protected ref eAIPlayerTargetInformation m_TargetInformation;
	bool m_Expansion_EnableBonePositionUpdate;
	ref SHumanCommandClimbResult m_ExClimbResult;
	protected bool m_Expansion_CanBeLooted = true;
	ref ExpansionNetsyncData m_Expansion_NetsyncData;
	protected typename m_eAI_FactionType;
	protected int m_eAI_FactionTypeID = -1;
	protected int m_eAI_FactionTypeIDSynch = -1;
	protected int m_eAI_GroupID = -1;
	int m_eAI_GroupMemberID = -1;
	protected int m_eAI_GroupMemberIndex;
	protected int m_eAI_GroupMemberIndexSynch;
	protected eAIGroup m_eAI_Group;
	protected eAIGroup m_Expansion_FormerGroup;
	protected bool m_eAI_IsPassive;
	int m_eAI_LastAggressionTime;
	int m_eAI_LastAggressionTimeout;
	float m_eAI_LastHitTime;

	float m_Expansion_ActualVelocityUpdateDT;
	vector m_Expansion_PreviousPosition;
	vector m_Expansion_ActualVelocity;
	void DayZPlayerImplement()
	{
		m_ExClimbResult = new SHumanCommandClimbResult;
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		Expansion_UpdateActualVelocity(pDt);
	}


	IEntity Expansion_GetParent()
	{
		IEntity parent = GetParent();

		if (!parent)
			parent = PhysicsGetLinkedEntity();

		return parent;
	}


	bool IsAI()
	{
		return false;
	}

	bool Expansion_IsInSafeZone()
	{
		return false;
	}


	bool Expansion_IsAI()
	{
		return IsAI();
	}
	eAIPlayerTargetInformation GetTargetInformation()
	{
		if (!m_TargetInformation)
			m_TargetInformation = new eAIPlayerTargetInformation(this);

		return m_TargetInformation;
	}
	void Expansion_SetCanBeLooted(bool canBeLooted)
	{
		m_Expansion_CanBeLooted = canBeLooted;
		SetSynchDirty();
	}

	bool Expansion_CanBeLooted()
	{
		return m_Expansion_CanBeLooted;
	}

	//! Uses head/barrel to determine direction, can be used on server.
	vector Expansion_GetAimDirection()
	{
		vector headTransform[4];
		GetBoneTransformWS(GetBoneIndexByName("head"), headTransform);

		EntityAI hands = GetHumanInventory().GetEntityInHands();
		vector dir;

		if (hands && IsRaised() && hands.IsWeapon())
		{
#ifdef SERVER
			vector ori = GetOrientation();
			vector headOri = headTransform[1].VectorToAngles();
			ori[0] = headOri[0] + 5;
			if (ori[0] > 360)
				ori[0] = ori[0] - 360;
			ori[2] = headOri[2];
			ori[1] = headOri[1] + 12.5;
			if (ori[1] > 360)
				ori[1] = ori[1] - 360;
			dir = ori.AnglesToVector();
#else
			vector weaponTransform[4];
			hands.GetTransform(weaponTransform);
			vector barrelStart = hands.GetSelectionPositionLS("konec hlavne").Multiply4(weaponTransform);
			vector barrelEnd = hands.GetSelectionPositionLS("usti hlavne").Multiply4(weaponTransform);
			dir = vector.Direction(barrelStart, barrelEnd).Normalized();
#endif
		}
		else
		{
			dir = headTransform[1];
		}

		return dir;
	}

	void Expansion_UpdateActualVelocity(float pDt)
	{
		vector position = GetPosition();

		if (m_Expansion_PreviousPosition == vector.Zero)
			m_Expansion_PreviousPosition = position;

		m_Expansion_ActualVelocityUpdateDT += pDt;

		if (m_Expansion_ActualVelocityUpdateDT < 0.333333)
			return;

		m_Expansion_ActualVelocity = (position - m_Expansion_PreviousPosition) * (1.0 / m_Expansion_ActualVelocityUpdateDT);
		m_Expansion_ActualVelocityUpdateDT = 0.0;
		m_Expansion_PreviousPosition = position;
	}

	vector Expansion_GetActualVelocity()
	{
		return m_Expansion_ActualVelocity;
	}


	float Expansion_GetMovementSpeed()
	{
		HumanCommandMove hcm = GetCommand_Move();
		if (hcm)
			return hcm.GetCurrentMovementSpeed();

		return 0.0;
	}


	float Expansion_GetMovementAngle()
	{
		HumanCommandMove hcm = GetCommand_Move();
		if (hcm)
			return hcm.GetCurrentMovementAngle();

		return 0.0;
	}
	void SetGroup(eAIGroup group, bool autoDeleteFormerGroupIfEmpty = true, int groupMemberIndex = -1)
	{
		if (!group && Expansion_IsAI())
		{
			EXError.Error(this, "Setting AI group to NULL is NOT allowed");
			return;
		}

		if (m_eAI_Group == group)
			return;

		if (m_eAI_Group)
		{
			m_eAI_Group.RemoveMember(this, autoDeleteFormerGroupIfEmpty);
			m_eAI_GroupID = -1;
			m_eAI_GroupMemberID = -1;

			if (!autoDeleteFormerGroupIfEmpty)
				m_Expansion_FormerGroup = m_eAI_Group;
		}

		m_eAI_Group = group;

		if (m_eAI_Group)
		{
			m_eAI_GroupID = m_eAI_Group.GetID();
			m_eAI_GroupMemberID = m_eAI_Group.m_NextGroupMemberID++;

			int factionTypeID = m_eAI_Group.GetFaction().GetTypeID();
			if (factionTypeID != m_eAI_FactionTypeID)
				eAI_SetFactionTypeID(factionTypeID);

			if (groupMemberIndex == -1)
				SetGroupMemberIndex(m_eAI_Group.AddMember(this));
			else
				m_eAI_Group.SetGroupMemberIndex(this, groupMemberIndex);
		}
		else if (m_eAI_FactionTypeID != -1)
		{
			eAI_SetFactionTypeID(-1);
		}

		if (g_Game.IsDedicatedServer())
			SetSynchDirty();
	}

	eAIGroup GetGroup()
	{
		return m_eAI_Group;
	}

	void Expansion_SetFormerGroup(eAIGroup group)
	{
		m_Expansion_FormerGroup = group;
	}

	eAIGroup Expansion_GetFormerGroup()
	{
		return m_Expansion_FormerGroup;
	}

	int GetGroupID()
	{
		return m_eAI_GroupID;
	}

	void SetGroupMemberIndex(int index)
	{
		m_eAI_GroupMemberIndex = index;
		m_eAI_GroupMemberIndexSynch = index;

		if (g_Game.IsDedicatedServer())
			SetSynchDirty();
	}

	void eAI_SetFactionTypeID(int id)
	{
		int oldFactionTypeID = m_eAI_FactionTypeID;
		m_eAI_FactionTypeID = id;
		m_eAI_FactionTypeIDSynch = id;
		eAI_OnFactionChange(oldFactionTypeID, id);

		if (g_Game.IsDedicatedServer())
			SetSynchDirty();
	}

	void eAI_OnFactionChange(int oldFactionTypeID, int newFactionTypeID)
	{
	}

	int eAI_GetFactionTypeID()
	{
		return m_eAI_FactionTypeID;
	}

	void eAI_SetPassive(bool state = true)
	{
		m_eAI_IsPassive = state;
	}

	bool eAI_IsPassive()
	{
		if (m_eAI_IsPassive)
			return true;

		if (m_eAI_Group)
			return m_eAI_Group.GetFaction().IsPassive();

		return false;
	}

	bool eAI_IsSideSteppingObstacles()
	{
		return false;
	}

	bool eAI_IsLit()
	{
		return false;
	}
	protected void eAI_Cleanup(bool autoDeleteGroup = false)
	{
		eAIGroup group = GetGroup();
		if (group && !group.RemoveMember(this, autoDeleteGroup) && autoDeleteGroup)
		{
			if (group.Count())
				group.RemoveDeceased(this);
			else
				group.Delete();
		}
	}

	bool eAI_UpdateAgressionTimeout(int timeThreshold)
	{
		if (!m_eAI_LastAggressionTime)
			return false;

		int time = ExpansionStatic.GetTimestamp(true);
		int timeout = timeThreshold - (time - m_eAI_LastAggressionTime);
		bool active;
		if (timeout > 0)
			active = true;

		if (active && time + timeout > m_eAI_LastAggressionTimeout)
		{
			m_eAI_LastAggressionTimeout = time + timeout;
			SetSynchDirty();
		}

		return active;
	}

	int eAI_GetLastAggressionCooldown()
	{
		int cooldown = m_eAI_LastAggressionTimeout - ExpansionStatic.GetTimestamp(true);
		if (cooldown > 0)
			return cooldown;

		return 0;
	}

	void eAI_ResetLastAggressionTimeout()
	{
		m_eAI_LastAggressionTimeout = 0;
		SetSynchDirty();
	}
	void eAI_DeletePersistentFiles()
	{
		if (!m_eAI_Group)
			return;

		string path = m_eAI_Group.GetStorageDirectory() + m_eAI_GroupMemberID.ToString();

		string aiDir = path + "\\";
		ExpansionStatic.DeleteDirectoryStructureRecursive(aiDir, ".bin");

		string aiPath = path + ".bin";
		if (FileExist(aiPath))
			DeleteFile(aiPath);
	}
	bool Expansion_IsAnimationIdle()
	{
		HumanCommandAdditives ad = GetCommandModifier_Additives();
		if (ad && ad.IsModifierActive())
			return false;

		return true;
	}
}
