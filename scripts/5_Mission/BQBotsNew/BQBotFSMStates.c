class BQBotShooterManager;

// Variant A: Expansion owns all movement/tactical FSM decisions.
// These classes are compile-stubs only. They must never set waypoints,
// flank points, cover points, movement speed or movement direction.
class BQBotStateBase
{
	protected BQBotShooterManager m_Manager;

	void BQBotStateBase(BQBotShooterManager manager)
	{
		m_Manager = manager;
	}

	int GetId() { return -1; }
	string GetName() { return "disabled"; }
	void OnEntry(int now) {}
	void OnExit(int now) {}
	void OnUpdate(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target) {}
	int ResolveTransition(int now, float detectRadius, PlayerBase previousTarget, PlayerBase target) { return GetId(); }
}

class BQBotState_Idle: BQBotStateBase
{
	void BQBotState_Idle(BQBotShooterManager manager) { m_Manager = manager; }
	override int GetId() { return 0; }
	override string GetName() { return "idle_disabled"; }
}

class BQBotState_Pursuit: BQBotStateBase
{
	void BQBotState_Pursuit(BQBotShooterManager manager) { m_Manager = manager; }
	override int GetId() { return 1; }
	override string GetName() { return "pursuit_disabled"; }
}

class BQBotState_TraversingWaypoints: BQBotStateBase
{
	void BQBotState_TraversingWaypoints(BQBotShooterManager manager) { m_Manager = manager; }
	override int GetId() { return 2; }
	override string GetName() { return "waypoints_disabled"; }
}

class BQBotState_Fighting_Positioning: BQBotStateBase
{
	void BQBotState_Fighting_Positioning(BQBotShooterManager manager) { m_Manager = manager; }
	override int GetId() { return 3; }
	override string GetName() { return "positioning_disabled"; }
}

class BQBotState_Fighting_FireWeapon: BQBotStateBase
{
	void BQBotState_Fighting_FireWeapon(BQBotShooterManager manager) { m_Manager = manager; }
	override int GetId() { return 4; }
	override string GetName() { return "fireweapon_disabled"; }
}

class BQBotState_Fighting_Evade: BQBotStateBase
{
	void BQBotState_Fighting_Evade(BQBotShooterManager manager) { m_Manager = manager; }
	override int GetId() { return 5; }
	override string GetName() { return "evade_disabled"; }
}

class BQBotState_FollowFormation: BQBotStateBase
{
	void BQBotState_FollowFormation(BQBotShooterManager manager) { m_Manager = manager; }
	override int GetId() { return 6; }
	override string GetName() { return "follow_disabled"; }
}

class BQBotState_Flank: BQBotStateBase
{
	void BQBotState_Flank(BQBotShooterManager manager) { m_Manager = manager; }
	override int GetId() { return 7; }
	override string GetName() { return "flank_disabled"; }
}
