modded class Weapon_Base
{
	override bool ProcessWeaponEvent(WeaponEventBase e)
	{
		bool result = super.ProcessWeaponEvent(e);

		PlayerBase player = PlayerBase.Cast(e.m_player);
		if (result && player && player.GetIdentity() == NULL && GetGame().IsServer())
		{
			ScriptRemoteInputUserData ctx = new ScriptRemoteInputUserData();
			ctx.Write(INPUT_UDT_WEAPON_REMOTE_EVENT);
			e.WriteToContext(ctx);
			player.StoreInputForRemotes(ctx);
		}

		return result;
	}
};
