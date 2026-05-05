modded class Weapon_Base
{
	override void SyncEventToRemote(WeaponEventBase e)
	{
		DayZPlayer p = DayZPlayer.Cast(GetHierarchyParent());
		if (p && (p.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || p.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_AI_SERVER))
		{
			ScriptRemoteInputUserData ctx = new ScriptRemoteInputUserData();

			ctx.Write(INPUT_UDT_WEAPON_REMOTE_EVENT);
			e.WriteToContext(ctx);

			p.StoreInputForRemotes(ctx);
		}
	}
}
