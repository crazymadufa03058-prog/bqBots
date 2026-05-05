modded class WeaponManager
{
	override void Fire(Weapon_Base wpn)
	{
		if (!m_player || !m_player.BQBot_IsBot() || !wpn)
		{
			super.Fire(wpn);
			return;
		}

		int mi = wpn.GetCurrentMuzzle();

		// Bots are configured jam-free; if some legacy jam flag appears, clear it and continue.
		if (wpn.IsJammed())
			wpn.SetJammed(false);

		// Keep vanilla trigger event path for animations/sounds/FSM, but never trigger jam branch.
		if (wpn.IsChamberFiredOut(mi) || wpn.IsChamberEmpty(mi))
		{
			wpn.ProcessWeaponEvent(new WeaponEventTrigger(m_player));
			return;
		}

		wpn.ProcessWeaponEvent(new WeaponEventTrigger(m_player));
	}

	override bool CanFire(Weapon_Base wpn)
	{
		if (super.CanFire(wpn))
			return true;

		if (!m_player || !m_player.BQBot_IsBot())
			return false;

		if (!wpn)
			return false;

		if (m_player.GetEntityInHands() != wpn)
			return false;

		if (wpn.IsDamageDestroyed())
			return false;
		if (wpn.IsJammed())
			wpn.SetJammed(false);

		DayZPlayerInventory inv = m_player.GetDayZPlayerInventory();
		if (inv && inv.IsProcessing())
			return false;

		if (m_player.IsFighting())
			return false;

		int muzzle = wpn.GetCurrentMuzzle();
		if (wpn.IsChamberEmpty(muzzle))
			return false;
		if (wpn.IsChamberFiredOut(muzzle))
			return false;

		return !wpn.IsCoolDown();
	}
}
