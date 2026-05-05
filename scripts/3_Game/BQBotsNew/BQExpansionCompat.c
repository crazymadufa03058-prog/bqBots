// Compatibility layer for raw Expansion AI movement/combat port.
// The movement/combat algorithms remain the original Expansion AI code; this file replaces external Core/CF services.

class EXTimeIt
{
	protected int m_Ticks;

	void EXTimeIt()
	{
		Start();
	}

	void Start()
	{
		m_Ticks = TickCount(0);
	}

	int GetElapsed()
	{
		return TickCount(m_Ticks);
	}

	float GetElapsedMS()
	{
		return TickCount(m_Ticks) * 0.0001;
	}

	void Log()
	{
	}
}

class EXHitch: EXTimeIt
{
	protected int m_Threshold;
	protected string m_MsgPrefix;

	void EXHitch(string msgPrefix, int threshold = 125000)
	{
		m_Threshold = threshold;
		m_MsgPrefix = msgPrefix;
	}
}
class EXTrace
{
	static bool AI;
	static bool AI_PROFILE;
	static bool GROUPS;
	static bool FSM;
	static bool MISC;
	static bool PATH_INTERPOLATION;

	static EXTrace Start(bool category, Class sender = null, string a = "", string b = "", string c = "") { return null; }
	static EXTrace Start(bool category, Class sender, string a, string b, string c, string d) { return null; }
	static EXTrace Start(bool category, typename sender, string a = "", string b = "", string c = "") { return null; }
	static EXTrace StartStack(bool category, Class sender = null, string a = "", string b = "", string c = "") { return null; }
	static EXTrace Profile(bool category, Class sender = null, string a = "") { return null; }
	static void Print(bool category, Class sender, string msg) {}
	static void Add(EXTrace trace, string msg) {}
	static bool IsEnabled() { return false; }
}

class EXError
{
	static void Error(Class sender, string msg) { Print("[EAI][Error] " + msg); }
	static void Error(Class sender, string msg, string stack) { Print("[EAI][Error] " + msg); }
	static void Error(Class sender, string msg, array<string> stack) { Print("[EAI][Error] " + msg); }
	static void ErrorOnce(Class sender, string msg) { Print("[EAI][ErrorOnce] " + msg); }
	static void Warn(Class sender, string msg) { Print("[EAI][Warn] " + msg); }
	static void Warn(Class sender, string msg, array<string> stack) { Print("[EAI][Warn] " + msg); }
	static void WarnOnce(Class sender, string msg) { Print("[EAI][WarnOnce] " + msg); }
	static bool ErrorFalse(Class sender, string msg) { Error(sender, msg); return false; }
}

class ExpansionString
{
	static string DirName(string fileName)
	{
		string tmp = fileName;
		tmp.Replace("\\", "/");
		int index = tmp.LastIndexOf("/");
		if (index > -1)
			return fileName.Substring(0, index);
		return fileName;
	}

	static bool ContainsAny(string text, TStringArray words)
	{
		foreach (string word: words)
		{
			if (text.Contains(word))
				return true;
		}
		return false;
	}

	static bool StartsWith(string text, string prefix)
	{
		if (text.Length() < prefix.Length())
			return false;
		return text.Substring(0, prefix.Length()) == prefix;
	}

	static bool StartsWithAny(string text, TStringArray words)
	{
		foreach (string word: words)
		{
			if (ExpansionString.StartsWith(text, word))
				return true;
		}
		return false;
	}

	static bool EndsWith(string text, string suffix, out int index = -1)
	{
		int suffixLength = suffix.Length();
		if (text.Length() < suffixLength)
			return false;
		index = text.LastIndexOf(suffix);
		return index == text.Length() - suffixLength;
	}

	static string JustifyLeft(string text, int length, string pad)
	{
		string tmp = text;
		length = length - text.Length();
		for (int i = 0; i < length; i++)
			tmp += pad;
		return tmp;
	}

	static string JoinStrings(TStringArray strings, string glue = ", ")
	{
		string output;
		for (int i = 0; i < strings.Count(); i++)
		{
			if (i > 0)
				output += glue;
			output += strings[i];
		}
		return output;
	}
}
class ExpansionUtil2<Class T1, Class T2>
{
	static void RefCopy(map<T1, ref T2> from, map<T1, ref T2> to)
	{
		if (!from || !to)
			return;
		to.Clear();
		foreach (T1 key, T2 value: from)
		{
			to[key] = value;
		}
	}
}

enum ExpansionVectorToString
{
	Plain,
	Beautify,
	Labels
}
class ExpansionStatic
{
	static string VectorToString(vector v, ExpansionVectorToString format = ExpansionVectorToString.Beautify, int precision = 2)
	{
		TStringArray output = {};
		float mul = Math.Pow(10, precision);
		for (int i = 0; i < 3; i++)
			output.Insert((Math.Round(v[i] * mul) / mul).ToString());
		if (format == ExpansionVectorToString.Beautify)
			return "<" + ExpansionString.JoinStrings(output, ", ") + ">";
		if (format == ExpansionVectorToString.Labels)
			return "X: " + output[0] + " Y: " + output[1] + " Z: " + output[2];
		return ExpansionString.JoinStrings(output, " ");
	}
	static string FloatToString(float v) { return v.ToString(); }
	static string FormatFloat(float v, int precision = 2, bool useLocale = true, bool formatInt = true, bool enableIncreasedAccuracy = false) { return v.ToString(); }
	static string FormatTime(float seconds, bool includeMS = false) { return seconds.ToString(); }
	static string GetDebugInfo(Class obj, bool checkParent = true) { return "debug_disabled"; }
	static string GetHierarchyInfo(Object obj) { return "debug_disabled"; }
	static string BitmaskEnumToString(typename e, int mask) { return mask.ToString(); }
	static string DumpToString(Class instance, int depth = 1) { return "debug_disabled"; }
	static int GetTimestamp(bool seconds = false)
	{
		if (!GetGame())
			return 0;
		int t = GetGame().GetTime();
		if (seconds)
			return t / 1000;
		return t;
	}

	static vector GetSurfacePosition(vector pos)
	{
		if (GetGame())
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
		return pos;
	}

	static vector GetSurfaceRoadPosition(vector pos, RoadSurfaceDetection rsd = RoadSurfaceDetection.CLOSEST)
	{
		if (GetGame())
			pos[1] = GetGame().SurfaceRoadY(pos[0], pos[2], rsd);
		return pos;
	}

	static vector GetSurfaceRoadPosition(float x, float z, RoadSurfaceDetection rsd = RoadSurfaceDetection.CLOSEST)
	{
		vector pos = Vector(x, 0.0, z);
		return GetSurfaceRoadPosition(pos, rsd);
	}

	static float GetSurfaceRoadY3D(float x, float y, float z, RoadSurfaceDetection rsd)
	{
		if (!GetGame())
			return y;
		return GetGame().SurfaceRoadY3D(x, y, z, rsd);
	}

	static bool SurfaceIsWater(vector pos)
	{
		return SurfaceIsWater(pos[0], pos[2]);
	}

	static bool SurfaceIsWater(float x, float z)
	{
		return false;
	}

	static bool MakeDirectoryRecursive(string path) { return MakeDirectory(path); }
	static void DeleteDirectoryStructureRecursive(string path, string extension = "") {}
	static void MessageNearPlayers(vector pos, float radius, string msg) { Print("[EAI] " + msg); }
	static bool Is(Object obj, string typeName)
	{
		if (!obj)
			return false;

		return obj.IsKindOf(typeName);
	}
	static bool CanObstruct(Object obj) { return obj != null; }
	static bool IsColliding(Object obj, vector point)
	{
		vector minMax[2];
		if (obj && obj.GetCollisionBox(minMax))
		{
			vector localPoint = obj.WorldToModel(point);

			for (int i = 0; i < 3; i++)
			{
				if (localPoint[i] < minMax[0][i])
					return false;

				if (localPoint[i] > minMax[1][i])
					return false;
			}

			return true;
		}

		return false;
	}
	static float GetBoundingRadius(Object obj)
	{
		if (!obj)
			return 0.0;

		vector minMax[2];
		return obj.ClippingInfo(minMax);
	}

	static bool GetCollisionBox(Object obj, out vector minMax[2])
	{
		if (!obj)
			return false;

		bool hasCollisionBox = obj.GetCollisionBox(minMax);
		if (!hasCollisionBox)
			obj.ClippingInfo(minMax);

		return hasCollisionBox;
	}
}

class ExpansionAIRoamingLocation
{
	vector Position;
	float Radius;
	string Name;
	string Type;
	string ClassName;
	Object ObjectRef;
	vector m_Position;
	float m_Radius;
	string m_Name;
	string m_Type;
	string m_ClassName;
	Object m_Object;
	int m_Index;
	bool Enabled;

	void ExpansionAIRoamingLocation(vector position = "0 0 0", float radius = 30.0, string name = "", string type = "", string className = "", Object obj = null, int index = -1)
	{
		Position = position;
		Radius = radius;
		Name = name;
		Type = type;
		ClassName = className;
		ObjectRef = obj;
		m_Position = position;
		m_Radius = radius;
		m_Name = name;
		m_Type = type;
		m_ClassName = className;
		m_Object = obj;
		m_Index = index;
		Enabled = true;
	}
}

class ExpansionAISettings
{
	float AccuracyMin = 0.25;
	float AccuracyMax = 0.85;
	float DamageMultiplier = 1.0;
	float DamageReceivedMultiplier = 1.0;
	float FormationScale = 1.0;
	bool EnableFlankingOutsideCombat = true;
	bool Manners = false;
	bool Vaulting = true;
	float MaxFlankingDistance = 80.0;
	int MemeLevel = 0;
	float NoiseInvestigationDistanceLimit = 400.0;
	float ThreatDistanceLimit = 400.0;
	ref map<int, float> LightingConfigMinNightVisibilityMeters = new map<int, float>;
	bool OverrideClientWeaponFiring = false;
	bool RecreateWeaponNetworkRepresentation = false;
	float SniperProneDistanceThreshold = 300.0;
	float GuardAggressionTimeout = 30.0;
	float AggressionTimeout = 30.0;
	bool LogAIHitBy = false;
	ref TStringArray PreventClimb = new TStringArray;

	static ExpansionAISettings Get()
	{
		return GetExpansionSettings().GetAI();
	}

	float GetDefaultMinVisibility(int lightingConfig) { return 80.0; }
	bool IsAdmin(PlayerIdentity identity) { return false; }
}

class ExpansionAILocationSettings
{
	ref array<ref ExpansionAIRoamingLocation> RoamingLocations = new array<ref ExpansionAIRoamingLocation>;
	ref TStringArray ExcludedRoamingBuildings = new TStringArray;
}

class ExpansionGeneralSettings
{
	bool DisableShootToUnlock = false;
}

class BQExpansionSettings
{
	ref ExpansionAISettings AI = new ExpansionAISettings();
	ref ExpansionAILocationSettings AILocation = new ExpansionAILocationSettings();
	ref ExpansionGeneralSettings General = new ExpansionGeneralSettings();

	ExpansionAISettings GetAI() { return AI; }
	ExpansionAILocationSettings GetAILocation() { return AILocation; }
	ExpansionGeneralSettings GetGeneral() { return General; }
}

ref BQExpansionSettings g_BQExpansionSettings;
BQExpansionSettings GetExpansionSettings()
{
	if (!g_BQExpansionSettings)
		g_BQExpansionSettings = new BQExpansionSettings();
	return g_BQExpansionSettings;
}

class eAIFaction
{
	protected string m_Loadout = "HumanLoadout";
	protected bool m_IsGuard;
	protected bool m_IsInvincible;
	protected bool m_IsPassive;
	protected bool m_IsObserver;
	protected bool m_DisableWeaponPickup;
	protected float m_MeleeDamageMultiplier = 1.0;
	protected float m_MeleeYeetForce;
	protected vector m_MeleeYeetFactors = "1 1 1";
	protected bool m_HasUnlimitedStamina;

	static eAIFaction CreateByID(int id) { return new eAIFactionRaiders(); }
	int GetTypeID() { return 0; }
	string GetName() { return "bqBots"; }
	string GetDefaultLoadout() { return m_Loadout; }
	bool IsFriendly(eAIFaction other) { return false; }
	bool IsFriendlyEntity(EntityAI other, DayZPlayer factionMember = null) { return false; }
	bool IsGuard() { return m_IsGuard; }
	bool IsInvincible() { return m_IsInvincible; }
	bool IsPassive() { return m_IsPassive; }
	bool IsObserver() { return m_IsObserver; }
	bool IsWeaponPickupEnabled() { return !m_DisableWeaponPickup; }
	bool HasUnlimitedStamina() { return m_HasUnlimitedStamina; }
	float GetMeleeDamageMultiplier() { return m_MeleeDamageMultiplier; }
	float GetMeleeYeetForce() { return m_MeleeYeetForce; }
	vector GetMeleeYeetFactors() { return m_MeleeYeetFactors; }
	bool CanLoot() { return false; }
}

class eAIFactionRaiders: eAIFaction
{
	override bool IsFriendly(eAIFaction other)
	{
		if (other && other.IsPassive())
			return true;

		return false;
	}
}

class eAIFactionPassive: eAIFaction
{
	void eAIFactionPassive()
	{
		m_IsPassive = true;
	}

	override bool IsFriendly(eAIFaction other) { return true; }
}


class eAINoiseParams
{
	float Strength = 1.0;
	float m_Strength = 1.0;
	eAINoiseType m_Type = eAINoiseType.SOUND;
	string m_Path;
}

enum eAINoiseType
{
	SOUND,
	SHOT,
	BULLETIMPACT,
	EXPLOSION
}

class eAINoiseSystem
{
	static ref ScriptInvoker SI_OnNoiseAdded = new ScriptInvoker();
	static eAINoiseParams GetNoiseParams(string path, eAINoiseType type) { eAINoiseParams p = new eAINoiseParams(); p.m_Path = path; p.m_Type = type; p.m_Strength = p.Strength; return p; }
	static void AddNoise(Object source, vector pos, string path, float coef = 1.0, eAINoiseType type = eAINoiseType.SHOT) {}
	static void AddNoiseEx(Object source, eAINoiseParams noiseParams, float strengthMultiplier = 1.0) {}
}

class ExpansionRPCManager
{
	ExpansionRPCManager CreateRPC(string name) { return this; }
	int RegisterClient(string name) { return name.Hash(); }
	int RegisterServer(string name) { return name.Hash(); }
	void Write(bool value) {}
	void Write(int value) {}
	void Write(string value) {}
	void Send(PlayerIdentity identity = null, bool guaranteed = true) {}
}

class ExpansionGame
{
	int m_InstanceId = 0;
	Object m_FirearmFXHitObject;
	ref ExpansionRPCManager m_RPCManager = new ExpansionRPCManager();

	ExpansionRPCManager GetRPCManager() { return m_RPCManager; }
	void SetInGroup(bool state) {}
	void SpectateAI(PlayerIdentity identity, Object ai, Object target) {}
	void eAI_OnUpdate(bool doSim, float timeslice) {}
}

modded class DayZGame
{
	ref ExpansionGame m_BQExpansionGame;

	void SetExpansionGame(ExpansionGame game)
	{
		m_BQExpansionGame = game;
	}

	ExpansionGame GetExpansionGame()
	{
		if (!m_BQExpansionGame)
			m_BQExpansionGame = new ExpansionGame();
		return m_BQExpansionGame;
	}

	override void OnUpdate(bool doSim, float timeslice)
	{
		super.OnUpdate(doSim, timeslice);

		if (IsServer() && m_BQExpansionGame)
			m_BQExpansionGame.eAI_OnUpdate(doSim, timeslice);
	}
}

