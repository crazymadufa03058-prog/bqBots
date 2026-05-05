class bqCombatPolyFlags
{
	static int ToFlag(string name)
	{
		name.ToLower();
		if (name == "unreachable") return PGPolyFlags.UNREACHABLE;
		if (name == "disabled") return PGPolyFlags.DISABLED;
		if (name == "walk") return PGPolyFlags.WALK;
		if (name == "door") return PGPolyFlags.DOOR;
		if (name == "inside") return PGPolyFlags.INSIDE;
		if (name == "ladder") return PGPolyFlags.LADDER;
		if (name == "special") return PGPolyFlags.SPECIAL;
		if (name == "crawl") return PGPolyFlags.CRAWL;
		if (name == "crouch") return PGPolyFlags.CROUCH;
		if (name == "swim") return PGPolyFlags.SWIM;
		if (name == "swim_sea") return PGPolyFlags.SWIM_SEA;
		return PGPolyFlags.NONE;
	}

	static int ToFlag(TStringArray names)
	{
		int flag = 0;
		foreach (string name : names)
		{
			flag |= ToFlag(name);
		}

		return flag;
	}
}