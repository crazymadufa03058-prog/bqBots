enum EBotRole
{
	ENGAGE,
	FLANK_LEFT,
	FLANK_RIGHT,
	SUPPORT
};

class BotSquad
{
	string m_ZoneName;
	ref array<ref BotUnit> m_Members;
	ref map<ref BotUnit, EBotRole> m_Roles;
	EntityAI m_SquadTarget;
	bool m_InCombat;
	float m_LastRoleAssignTime;
	static const float ROLE_REASSIGN_INTERVAL = 8.0;

	void BotSquad(string zoneName)
	{
		m_ZoneName = zoneName;
		m_Members = new array<ref BotUnit>();
		m_Roles = new map<ref BotUnit, EBotRole>();
		m_SquadTarget = NULL;
		m_InCombat = false;
		m_LastRoleAssignTime = 0;
	}

	void AddMember(BotUnit bot)
	{
		if (m_Members.Find(bot) < 0)
		{
			m_Members.Insert(bot);
			bot.m_Squad = this;
			Print("[MyBots] Бот добавлен в отряд '" + m_ZoneName + "' (" + m_Members.Count() + ")");
		}
	}

	void RemoveMember(BotUnit bot)
	{
		int idx = m_Members.Find(bot);
		if (idx >= 0)
		{
			m_Members.Remove(idx);
			bot.m_Squad = NULL;
		}
	}

	int GetAliveCount()
	{
		int count = 0;
		for (int i = 0; i < m_Members.Count(); i++)
		{
			if (m_Members[i].IsAlive())
				count++;
		}
		return count;
	}

	void OnEngage(EntityAI enemy)
	{
		m_SquadTarget = enemy;
		m_InCombat = true;
		AssignRoles();
	}

	void OnDisengage()
	{
		m_SquadTarget = NULL;
		m_InCombat = false;
		m_Roles.Clear();
	}

	void AssignRoles()
	{
		m_Roles.Clear();

		int aliveCount = GetAliveCount();
		if (aliveCount == 0)
			return;

		ref array<ref BotUnit> aliveMembers = new array<ref BotUnit>();
		for (int i = 0; i < m_Members.Count(); i++)
		{
			if (m_Members[i].IsAlive())
				aliveMembers.Insert(m_Members[i]);
		}

		if (aliveCount == 1)
		{
			m_Roles.Set(aliveMembers[0], EBotRole.ENGAGE);
			return;
		}

		m_Roles.Set(aliveMembers[0], EBotRole.ENGAGE);

		for (int j = 1; j < aliveMembers.Count(); j++)
		{
			if (j == 1)
				m_Roles.Set(aliveMembers[j], EBotRole.FLANK_LEFT);
			else if (j == 2)
				m_Roles.Set(aliveMembers[j], EBotRole.FLANK_RIGHT);
			else
				m_Roles.Set(aliveMembers[j], EBotRole.SUPPORT);
		}

		m_LastRoleAssignTime = GetGame().GetTickTime();
		Print("[MyBots] Роли назначены в отряде '" + m_ZoneName + "': " + aliveCount + " ботов");
	}

	EBotRole GetRole(BotUnit bot)
	{
		if (m_Roles.Contains(bot))
			return m_Roles.Get(bot);
		return EBotRole.ENGAGE;
	}

	bool ShouldFlank(BotUnit bot)
	{
		EBotRole role = GetRole(bot);
		return role == EBotRole.FLANK_LEFT || role == EBotRole.FLANK_RIGHT;
	}

	bool ShouldEngage(BotUnit bot)
	{
		return GetRole(bot) == EBotRole.ENGAGE;
	}

	bool ShouldSupport(BotUnit bot)
	{
		return GetRole(bot) == EBotRole.SUPPORT;
	}

	bool IsFlankingLeft(BotUnit bot)
	{
		return GetRole(bot) == EBotRole.FLANK_LEFT;
	}

	void UpdateRoles(float now)
	{
		if (m_InCombat && now - m_LastRoleAssignTime > ROLE_REASSIGN_INTERVAL)
		{
			AssignRoles();
		}
	}

	vector GetFlankDirection(BotUnit bot)
	{
		if (!m_SquadTarget)
			return "0 0 0";

		EBotRole role = GetRole(bot);
		if (role == EBotRole.FLANK_LEFT)
			return "-1 0 0";
		if (role == EBotRole.FLANK_RIGHT)
			return "1 0 0";
		return "0 0 0";
	}

	ref array<ref BotUnit> GetOtherEngagers(BotUnit bot)
	{
		ref array<ref BotUnit> engagers = new array<ref BotUnit>();
		for (int i = 0; i < m_Members.Count(); i++)
		{
			if (m_Members[i] != bot && m_Members[i].IsAlive() && GetRole(m_Members[i]) == EBotRole.ENGAGE)
				engagers.Insert(m_Members[i]);
		}
		return engagers;
	}

	bool HasRetreatingMember()
	{
		for (int i = 0; i < m_Members.Count(); i++)
		{
			if (m_Members[i].IsAlive() && m_Members[i].m_IsRetreating)
				return true;
		}
		return false;
	}

	int GetRetreatingCount()
	{
		int count = 0;
		for (int i = 0; i < m_Members.Count(); i++)
		{
			if (m_Members[i].IsAlive() && m_Members[i].m_IsRetreating)
				count++;
		}
		return count;
	}
};