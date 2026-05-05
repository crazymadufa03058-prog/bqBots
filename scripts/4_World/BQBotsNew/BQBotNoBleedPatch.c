modded class BleedingSourcesManagerServer
{
	override protected void AddBleedingSource(int bit)
	{
		bqBots_Boris pb = bqBots_Boris.Cast(m_Player);
		if (pb && pb.BQBot_IsBot())
		{
			return;
		}

		super.AddBleedingSource(bit);
	}

	override void ProcessHit(float damage, EntityAI source, int component, string zone, string ammo, vector modelPos)
	{
		bqBots_Boris pb = bqBots_Boris.Cast(m_Player);
		if (pb && pb.BQBot_IsBot())
		{
			return;
		}

		super.ProcessHit(damage, source, component, zone, ammo, modelPos);
	}

	override void OnTick(float delta_time)
	{
		bqBots_Boris pb = bqBots_Boris.Cast(m_Player);
		if (pb && pb.BQBot_IsBot())
		{
			SetBloodLoss(true);
			if (pb.GetBleedingBits() != 0)
			{
				RemoveAllSources();
			}
			return;
		}

		super.OnTick(delta_time);
	}
}
