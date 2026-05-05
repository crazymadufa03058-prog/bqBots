modded class WeaponFire
{
	override void OnEntry(WeaponEventBase e)
	{
		if (e)
		{
			if (g_Game.IsServer())
			{
				PlayerBase playerOwner;
				Class.CastTo(playerOwner, m_weapon.GetHierarchyParent());
				m_weapon.AddJunctureToAttachedMagazine(playerOwner, 100);
			}

			m_dtAccumulator = 0;

			int mi = m_weapon.GetCurrentMuzzle();
			bqBots_Boris botOwner = bqBots_Boris.Cast(m_weapon.GetHierarchyParent());
			bool ownerIsBot = botOwner && botOwner.BQBot_IsBot();
			bool fired = BQBot_TryDirectedFire(mi);
			if (!fired && !ownerIsBot)
				fired = TryFireWeapon(m_weapon, mi);

			if (fired)
			{
				DayZPlayerImplement p;
				if (Class.CastTo(p, e.m_player))
				{
					bqBots_Boris pb = bqBots_Boris.Cast(p);
					if (!pb || !pb.BQBot_IsBot())
						p.GetAimingModel().SetRecoil(m_weapon);
				}
				m_weapon.OnFire(mi);
			}
		}

		super.OnEntry(e);
	}

	protected bool BQBot_TryDirectedFire(int muzzleIndex)
	{
		if (!GetGame() || !GetGame().IsServer())
			return false;

		bqBots_Boris owner = bqBots_Boris.Cast(m_weapon.GetHierarchyParent());
		if (!owner || !owner.BQBot_IsBot())
			return false;

		vector aimPos;
		if (!owner.BQBot_GetAimTarget(aimPos))
			return false;

		int neckBone = owner.GetBoneIndexByName("neck");
		if (neckBone < 0)
			return false;

		vector firePos = owner.GetBonePositionWS(neckBone);
		vector fireDir = vector.Direction(firePos, aimPos).Normalized();
		float dirLenSq = fireDir[0] * fireDir[0] + fireDir[1] * fireDir[1] + fireDir[2] * fireDir[2];
		if (dirLenSq < 0.000001)
			return false;

		float distanceToTarget = vector.Distance(firePos, aimPos);
		float spreadDeg = owner.BQBot_GetSpreadDegForDistance(distanceToTarget);
		if (spreadDeg > 0.01)
			fireDir = BQBot_ApplySpreadCone(fireDir, spreadDeg);

		firePos = firePos + fireDir * 0.2;
		bool fired = m_weapon.Fire(muzzleIndex, firePos, fireDir, fireDir);
		if (fired)
			owner.BQBot_RecordShotTelemetry(firePos, fireDir);
		return fired;
	}

	protected vector BQBot_ApplySpreadCone(vector baseDir, float spreadDeg)
	{
		vector dir = baseDir.Normalized();
		float spreadRad = spreadDeg * 0.0174532925;
		if (spreadRad <= 0.00001)
			return dir;

		vector up = "0 1 0";
		if (Math.AbsFloat(dir[1]) > 0.98)
			up = "1 0 0";

		vector right = BQBot_Cross(dir, up).Normalized();
		vector orthoUp = BQBot_Cross(right, dir).Normalized();

		float angle = Math.RandomFloatInclusive(0.0, 6.28318530718);
		float radius = Math.Sqrt(Math.RandomFloatInclusive(0.0, 1.0)) * Math.Tan(spreadRad);
		float x = Math.Cos(angle) * radius;
		float y = Math.Sin(angle) * radius;

		vector offset = right * x + orthoUp * y;
		return (dir + offset).Normalized();
	}

	protected vector BQBot_Cross(vector a, vector b)
	{
		vector outVec = "0 0 0";
		outVec[0] = a[1] * b[2] - a[2] * b[1];
		outVec[1] = a[2] * b[0] - a[0] * b[2];
		outVec[2] = a[0] * b[1] - a[1] * b[0];
		return outVec;
	}
}
