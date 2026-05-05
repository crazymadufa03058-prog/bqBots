enum ExpansionFireMode
{
	INVALID = -1,
	SemiAuto,
	Burst,
	FullAuto,
	Single,
	Double
}

enum ExpansionWeaponType
{
	Handgun,
	Shotgun,
	Submachinegun,
	Assault,
	Marksman,
	GenericRifle,
	Launcher,
	Archery
}

class ExpansionWeaponInfo
{
	ref map<ExpansionFireMode, int> m_FireModes = new map<ExpansionFireMode, int>;
	float m_AvgDmg = 1.0;
	float m_DPS = 1.0;
	ExpansionWeaponType m_WeaponType = ExpansionWeaponType.GenericRifle;

	void ExpansionWeaponInfo(Weapon_Base weapon = null)
	{
		m_FireModes[ExpansionFireMode.Single] = 0;
		m_FireModes[ExpansionFireMode.SemiAuto] = 0;
		m_FireModes[ExpansionFireMode.Burst] = 0;
		m_FireModes[ExpansionFireMode.FullAuto] = 0;
	}
}

class ExpansionWeaponUtils
{
	static float GetDamageAppliedByAmmo(string ammoTypeName)
	{
		return 1.0;
	}

	static float GetWeaponInitSpeedMultiplier(string weaponType)
	{
		return 1.0;
	}
}

class ExpansionScriptRPC: ScriptRPC
{
	static const int EXPANSION_RPC_UNTARGETED = 1506850293;
	static const int EXPANSION_RPC_TARGETED = 1506850294;
	int m_Expansion_RPCID;

	static ExpansionScriptRPC Create(int rpcID)
	{
		ExpansionScriptRPC rpc = new ExpansionScriptRPC();
		rpc.m_Expansion_RPCID = rpcID;
		rpc.Write(rpcID);
		return rpc;
	}

	void Expansion_Send(bool guaranteed = true, PlayerIdentity recipient = null)
	{
		Expansion_Send(null, guaranteed, recipient);
	}

	void Expansion_Send(Object target, bool guaranteed = true, PlayerIdentity recipient = null)
	{
		if (target)
			Send(target, EXPANSION_RPC_TARGETED, guaranteed, recipient);
		else
			Send(null, EXPANSION_RPC_UNTARGETED, guaranteed, recipient);
	}
}

modded class Weapon_Base
{
	ref ExpansionWeaponInfo m_Expansion_WeaponInfo;
	ExpansionFireMode m_BQExpansion_FireMode = ExpansionFireMode.Single;

	void Weapon_Base()
	{
		if (!m_Expansion_WeaponInfo)
			m_Expansion_WeaponInfo = new ExpansionWeaponInfo(this);
	}

	ExpansionFireMode Expansion_GetFireMode()
	{
		return m_BQExpansion_FireMode;
	}

	bool Expansion_SetFireMode(ExpansionFireMode mode)
	{
		bool changed;
		return Expansion_SetFireMode(mode, changed);
	}

	bool Expansion_SetFireMode(ExpansionFireMode mode, out bool changed)
	{
		changed = m_BQExpansion_FireMode != mode;
		m_BQExpansion_FireMode = mode;
		return true;
	}

	float Expansion_GetDPS()
	{
		if (m_Expansion_WeaponInfo)
			return m_Expansion_WeaponInfo.m_DPS;
		return 0.0;
	}

	ExpansionWeaponType Expansion_GetWeaponType()
	{
		if (m_Expansion_WeaponInfo)
			return m_Expansion_WeaponInfo.m_WeaponType;
		return ExpansionWeaponType.GenericRifle;
	}

	float Expansion_GetMinSafeFiringDistance()
	{
		return 0.0;
	}

}
