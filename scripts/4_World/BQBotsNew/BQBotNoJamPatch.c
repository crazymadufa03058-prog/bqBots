modded class Weapon_Base
{
	override float GetChanceToJam()
	{
		bqBots_Boris pb = bqBots_Boris.Cast(GetHierarchyRootPlayer());
		if (pb && pb.BQBot_IsBot())
			return 0.0;

		return super.GetChanceToJam();
	}
}
