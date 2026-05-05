modded class ItemBase
{
	protected const bool BQBOT_ITEM_DAMAGE_DIAG = false;

	protected bool BQBotItem_IsProjectileAmmo(string ammo)
	{
		if (ammo == "")
			return false;

		string lower = ammo;
		lower.ToLower();
		if (lower.Contains("bullet_"))
			return true;
		if (lower.Contains("ammo_"))
			return true;
		return false;
	}

	protected PlayerBase BQBotItem_GetDamageSourcePlayer(EntityAI source)
	{
		if (!source)
			return null;

		PlayerBase srcPlayer = PlayerBase.Cast(source);
		if (srcPlayer)
			return srcPlayer;

		Man rootMan = source.GetHierarchyRootPlayer();
		if (rootMan)
		{
			srcPlayer = PlayerBase.Cast(rootMan);
			if (srcPlayer)
				return srcPlayer;
		}

		Object parent = source.GetHierarchyParent();
		int guard = 0;
		while (parent && guard < 10)
		{
			srcPlayer = PlayerBase.Cast(parent);
			if (srcPlayer)
				return srcPlayer;

			EntityAI parentEnt = EntityAI.Cast(parent);
			if (!parentEnt)
				return null;

			rootMan = parentEnt.GetHierarchyRootPlayer();
			if (rootMan)
			{
				srcPlayer = PlayerBase.Cast(rootMan);
				if (srcPlayer)
					return srcPlayer;
			}

			parent = parentEnt.GetHierarchyParent();
			guard++;
		}

		return null;
	}

	protected PlayerBase BQBotItem_FindLikelyBotShooterFromTelemetry(string ammo)
	{
		if (!GetGame() || !GetGame().IsServer())
			return null;
		if (!BQBotItem_IsProjectileAmmo(ammo))
			return null;

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		if (!players || players.Count() == 0)
			return null;

		vector hitPos = GetPosition();
		int now = GetGame().GetTime();
		PlayerBase best = null;
		float bestScore = -2.0;
		foreach (Man man : players)
		{
			PlayerBase bot = PlayerBase.Cast(man);
			if (!bot || !bot.IsAlive() || !bot.BQBot_IsBot())
				continue;

			int shotTime;
			vector firePos;
			vector fireDir;
			if (!bot.BQBot_GetShotTelemetry(shotTime, firePos, fireDir))
				continue;

			int ageMs = now - shotTime;
			if (ageMs < 0 || ageMs > 900)
				continue;

			vector toHit = hitPos - firePos;
			float toHitLenSq = toHit[0] * toHit[0] + toHit[1] * toHit[1] + toHit[2] * toHit[2];
			if (toHitLenSq < 0.04 || toHitLenSq > 640000.0)
				continue;

			vector dirToHit = toHit.Normalized();
			float dot = vector.Dot(fireDir, dirToHit);
			if (dot < 0.95)
				continue;

			float score = dot - (ageMs * 0.0001);
			if (score > bestScore)
			{
				bestScore = score;
				best = bot;
			}
		}

		return best;
	}

	protected bool BQBotItem_IsInOwnerHandsTree(PlayerBase owner)
	{
		if (!owner)
			return false;

		EntityAI hands = owner.GetEntityInHands();
		if (!hands)
			return false;
		if (this == hands)
			return true;

		Object parent = GetHierarchyParent();
		int guard = 0;
		while (parent && guard < 12)
		{
			if (parent == hands)
				return true;

			EntityAI parentEnt = EntityAI.Cast(parent);
			if (!parentEnt)
				return false;

			parent = parentEnt.GetHierarchyParent();
			guard++;
		}

		return false;
	}

	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (!GetGame() || !GetGame().IsServer() || !damageResult)
			return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		PlayerBase owner = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!owner || owner.BQBot_IsBot())
			return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (!BQBotItem_IsInOwnerHandsTree(owner))
			return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		PlayerBase srcPlayer = BQBotItem_GetDamageSourcePlayer(source);
		if (!srcPlayer)
			srcPlayer = BQBotItem_FindLikelyBotShooterFromTelemetry(ammo);
		if (!srcPlayer || !srcPlayer.BQBot_IsBot())
			return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		float damageMultiplier = srcPlayer.BQBot_GetDamageMultiplier();
		if (Math.AbsFloat(damageMultiplier - 1.0) <= 0.001)
			return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		string applyZone = dmgZone;
		float healthDamage = damageResult.GetDamage(applyZone, "Health");
		if (Math.AbsFloat(healthDamage) <= 0.0001 && applyZone != "")
		{
			applyZone = "";
			healthDamage = damageResult.GetDamage(applyZone, "Health");
		}
		if (Math.AbsFloat(healthDamage) <= 0.0001)
			healthDamage = damageResult.GetDamage("", "");

		if (Math.AbsFloat(healthDamage) > 0.0001)
			AddHealth(applyZone, "Health", -healthDamage * damageMultiplier);

		if (BQBOT_ITEM_DAMAGE_DIAG)
		{
			string itemDmgLog = "[EAI] [itemdmgmult] item=" + GetType();
			itemDmgLog = itemDmgLog + " owner=" + owner.GetType();
			itemDmgLog = itemDmgLog + " source=" + srcPlayer.GetType();
			itemDmgLog = itemDmgLog + " mult=" + damageMultiplier.ToString();
			itemDmgLog = itemDmgLog + " zone=" + applyZone;
			itemDmgLog = itemDmgLog + " health=" + healthDamage.ToString();
			itemDmgLog = itemDmgLog + " ammo=" + ammo;
			Print(itemDmgLog);
		}

		return false;
	}
}
