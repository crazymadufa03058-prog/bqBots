modded class DayZPlayerImplementAiming
{
	override void SetRecoil(Weapon_Base weapon)
	{
		// Disable recoil only for our custom bots, keep vanilla recoil for everyone else.
		if (m_PlayerPb && m_PlayerPb.BQBot_IsBot())
		{
			m_CurrentRecoil = null;
			return;
		}

		super.SetRecoil(weapon);
	}
}
