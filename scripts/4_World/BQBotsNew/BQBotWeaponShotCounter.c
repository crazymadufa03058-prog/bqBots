modded class Weapon_Base
{
	protected const bool BQBOT_ONFIRE_LOGS = true;

	override void OnFire(int muzzle_index)
	{
		super.OnFire(muzzle_index);

		if (!GetGame() || !GetGame().IsServer())
			return;

		PlayerBase ownerPlayer = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (ownerPlayer && !ownerPlayer.BQBot_IsBot())
			ownerPlayer.BQBot_RecordPlayerShotNoise(ownerPlayer.GetPosition());

		bqBots_Boris pb = bqBots_Boris.Cast(ownerPlayer);
		if (!pb || !pb.BQBot_IsBot())
			return;

		Magazine mag = Magazine.Cast(GetMagazine(muzzle_index));
		int magAmmo = -1;
		if (mag)
			magAmmo = mag.GetAmmoCount();

		string chamberEmpty = "0";
		if (IsChamberEmpty(muzzle_index))
			chamberEmpty = "1";

		string chamberFiredOut = "0";
		if (IsChamberFiredOut(muzzle_index))
			chamberFiredOut = "1";

		string holdWanted = "0";
		if (pb.BQBot_IsVanillaTriggerHoldWanted())
			holdWanted = "1";

		if (BQBOT_ONFIRE_LOGS)
		{
			string shotDiagMsg = "[bqBots] [onfire] real_shot nextServerShots=" + (pb.BQBot_GetServerShotCount() + 1).ToString();
			shotDiagMsg = shotDiagMsg + " muzzle=" + muzzle_index.ToString();
			shotDiagMsg = shotDiagMsg + " mode=" + GetCurrentMode(muzzle_index).ToString();
			shotDiagMsg = shotDiagMsg + " modeName=" + GetCurrentModeName(muzzle_index);
			shotDiagMsg = shotDiagMsg + " holdWanted=" + holdWanted;
			shotDiagMsg = shotDiagMsg + " botPos=" + pb.GetPosition().ToString();
			shotDiagMsg = shotDiagMsg + " ori=" + pb.GetOrientation().ToString();
			shotDiagMsg = shotDiagMsg + " chamberEmpty=" + chamberEmpty;
			shotDiagMsg = shotDiagMsg + " chamberFiredOut=" + chamberFiredOut;
			shotDiagMsg = shotDiagMsg + " magAmmo=" + magAmmo.ToString();
			Print(shotDiagMsg);
		}

		float maxWeaponHealth = GetMaxHealth("", "Health");
		if (maxWeaponHealth > 0.0 && GetHealth("", "Health") < maxWeaponHealth)
			SetHealth("", "Health", maxWeaponHealth);

		ItemBase suppressor = GetAttachedSuppressor();
		if (suppressor)
		{
			float maxSuppressorHealth = suppressor.GetMaxHealth("", "Health");
			if (maxSuppressorHealth > 0.0 && suppressor.GetHealth("", "Health") < maxSuppressorHealth)
				suppressor.SetHealth("", "Health", maxSuppressorHealth);
		}

		pb.BQBot_MarkShot();
	}
}

