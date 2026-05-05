class BQExpansionDiag
{
	static string B(bool value)
	{
		if (value)
			return "1";
		return "0";
	}

	static string Obj(Object obj)
	{
		if (!obj)
			return "NULL";
		return obj.ToString();
	}

	static void Log(string tag, string msg)
	{
		Print("[BQDIAG] [" + tag + "] " + msg);
	}
}
