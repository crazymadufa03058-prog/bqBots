class bqCombatFormation
{
	static const float DIR_RECALC_DIST_SQ = 9.0;

	protected vector m_Transform[4];
	protected float m_Scale = 1.0;
	protected float m_Looseness = 0.0;
	protected int m_MemberCount = 1;
	protected vector m_LastUpdatePosition;

	void bqCombatFormation()
	{
		UpdateTransform(vector.Zero, vector.Forward);
	}

	string GetName()
	{
		string cls = Type().ToString();
		return cls.Substring(17, cls.Length() - 17);
	}

	vector GetPosition(int member_no);

	vector GetDirection(int member_no)
	{
		return vector.Forward;
	}

	void SetLooseness(float looseness)
	{
		m_Looseness = looseness;
	}

	float GetLooseness()
	{
		return Math.RandomFloatInclusive(-m_Looseness, m_Looseness);
	}

	float GetLoosenessPreference()
	{
		return m_Looseness;
	}

	void SetScale(float scale)
	{
		if (scale <= 0)
			scale = 1.0;
		m_Scale = scale;
	}

	float GetScale()
	{
		return m_Scale;
	}

	void SetMemberCount(int count)
	{
		if (count < 1)
			count = 1;
		m_MemberCount = count;
	}

	int GetMemberCount()
	{
		return m_MemberCount;
	}

	vector ToWorld(vector fs, vector leaderPos)
	{
		m_Transform[3] = leaderPos;
		return fs.Multiply4(m_Transform);
	}

	void Update(vector leaderPos)
	{
		if (m_LastUpdatePosition == vector.Zero)
		{
			UpdateTransform(leaderPos, vector.Forward);
			return;
		}

		vector dir = vector.Direction(m_LastUpdatePosition, leaderPos);
		float distSq = dir.LengthSq();
		if (distSq < DIR_RECALC_DIST_SQ)
		{
			m_Transform[3] = leaderPos;
			return;
		}

		UpdateTransform(leaderPos, dir.Normalized());
	}

	void UpdateForced(vector leaderPos, vector dir)
	{
		float lenSq = dir[0] * dir[0] + dir[2] * dir[2];
		if (lenSq < 0.0001)
		{
			Update(leaderPos);
			return;
		}
		UpdateTransform(leaderPos, dir.Normalized());
	}

	void UpdateTransform(vector newPos, vector dir)
	{
		m_Transform[3] = newPos;
		m_Transform[2] = dir;
		m_Transform[1] = "0 1 0";
		m_Transform[0] = dir.Perpend();
		m_LastUpdatePosition = newPos;
	}

	static bqCombatFormation Create(string formationName)
	{
		formationName.ToLower();
		if (formationName == "column")
			return new bqCombatFormationColumn();
		if (formationName == "invcolumn")
			return new bqCombatFormationInvColumn();
		if (formationName == "file")
			return new bqCombatFormationFile();
		if (formationName == "invfile")
			return new bqCombatFormationInvFile();
		if (formationName == "invvee")
			return new bqCombatFormationInvVee();
		if (formationName == "wall" || formationName == "line")
			return new bqCombatFormationWall();
		if (formationName == "circle")
			return new bqCombatFormationCircle();
		if (formationName == "circledot")
			return new bqCombatFormationCircleDot();
		if (formationName == "star")
			return new bqCombatFormationStar();
		if (formationName == "stardot")
			return new bqCombatFormationStarDot();
		return new bqCombatFormationVee();
	}
}
