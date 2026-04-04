class BotWeaponHandler
{
	protected PlayerBase m_Bot;
	protected ref BotAimController m_Aim;
	protected float m_NextFireTime;
	protected float m_FirePauseMin;
	protected float m_FirePauseMax;
	protected int m_BurstCount;
	protected int m_BurstMax;
	protected bool m_IsReloading;
	protected float m_ReloadEndTime;
	protected bool m_IsFiring;
	protected float m_AimTimer;
	protected float m_RaiseTimer;
	protected float m_BurstTimer;
	protected ref array<ref WeaponEventTrigger> m_PendingShots;

	static const float AIM_TIME = 0.6;
	static const float RAISE_DELAY = 0.5;
	static const float BURST_DELAY = 0.08;

	void BotWeaponHandler(PlayerBase bot, BotAimController aim)
	{
		m_Bot = bot;
		m_Aim = aim;
		m_NextFireTime = 0;
		m_FirePauseMin = 0.5;
		m_FirePauseMax = 1.5;
		m_BurstCount = 0;
		m_BurstMax = 3;
		m_IsReloading = false;
		m_ReloadEndTime = 0;
		m_IsFiring = false;
		m_AimTimer = 0;
		m_RaiseTimer = 0;
		m_BurstTimer = 0;
		m_PendingShots = new array<ref WeaponEventTrigger>();
	}

	void SetBurstParams(float pauseMin, float pauseMax, int burstMax)
	{
		m_FirePauseMin = pauseMin;
		m_FirePauseMax = pauseMax;
		m_BurstMax = burstMax;
	}

	void ResetRaiseTimer()
	{
		m_RaiseTimer = 0;
		m_AimTimer = 0;
	}

	bool TryFire(vector targetPos, float accuracy, BotDamageConfig dmgCfg, float dt = 0.1)
	{
		if (!m_Bot || !GetGame().IsServer())
			return false;

		// Обработка отложенных выстрелов в очереди
		if (m_PendingShots.Count() > 0)
		{
			m_BurstTimer -= dt;
			if (m_BurstTimer <= 0)
			{
				Weapon_Base weapon = Weapon_Base.Cast(m_Bot.GetItemInHands());
				if (weapon && weapon.CanFire(weapon.GetCurrentMuzzle()))
				{
					weapon.ProcessWeaponEvent(m_PendingShots[0]);
					m_PendingShots.Remove(0);
					
					// Досыл после выстрела
					int muzzle = weapon.GetCurrentMuzzle();
					if (weapon.IsChamberEmpty(muzzle) || weapon.IsChamberFiredOut(muzzle))
					{
						Magazine mag = weapon.GetMagazine(muzzle);
						if (mag && mag.GetAmmoCount() > 0)
						{
							float damage;
							string type;
							if (mag.LocalAcquireCartridge(damage, type))
							{
								weapon.PushCartridgeToChamber(muzzle, damage, type);
								weapon.SelectionBulletShow();
							}
						}
					}
					
					m_BurstCount++;
					
					if (m_BurstCount < m_BurstMax && m_PendingShots.Count() < m_BurstMax)
					{
						m_BurstTimer = BURST_DELAY;
						Print("[MyBots][FIRE] Burst shot " + m_BurstCount + "/" + m_BurstMax);
					}
					else if (m_PendingShots.Count() == 0)
					{
						m_BurstCount = 0;
						float pauseTime = Math.RandomFloat(m_FirePauseMin, m_FirePauseMax);
						m_NextFireTime = GetGame().GetTickTime() + pauseTime;
						Print("[MyBots][FIRE] Burst завершён (" + m_BurstMax + "), пауза " + pauseTime);
					}
				}
			}
			return false;
		}

		if (m_IsReloading)
		{
			float now = GetGame().GetTickTime();
			if (now < m_ReloadEndTime)
				return false;
			FinishReload();
		}

		// Проверяем стойку
		ref HumanMovementState ms = new HumanMovementState();
		m_Bot.GetMovementState(ms);
		bool isRaised = ms.IsRaised();
		
		if (!isRaised)
		{
			m_RaiseTimer = 0;
			m_AimTimer = 0;
			return false;
		}

		m_RaiseTimer += dt;
		if (m_RaiseTimer < RAISE_DELAY)
			return false;

		m_AimTimer += dt;
		if (m_AimTimer < AIM_TIME)
			return false;

		float now2 = GetGame().GetTickTime();
		if (now2 < m_NextFireTime)
			return false;

		Weapon_Base weapon = Weapon_Base.Cast(m_Bot.GetItemInHands());
		if (!weapon)
		{
			Print("[MyBots][FIRE][ERROR] No weapon in hands");
			return false;
		}

		int muzzle = weapon.GetCurrentMuzzle();

		if (!weapon.CanFire(muzzle))
		{
			bool chamberEmpty = weapon.IsChamberEmpty(muzzle);
			if (chamberEmpty)
				StartReload(weapon);
			return false;
		}

		// Первый выстрел в очереди
		Print("[MyBots][FIRE] ProcessWeaponEvent(TRIGGER) weapon=" + weapon.GetType() + " muzzle=" + muzzle + " ammo=" + weapon.GetMagazine(muzzle).GetAmmoCount());
		weapon.ProcessWeaponEvent(new WeaponEventTrigger(m_Bot));

		// Досыл патрона
		if (weapon.IsChamberEmpty(muzzle) || weapon.IsChamberFiredOut(muzzle))
		{
			Magazine mag = weapon.GetMagazine(muzzle);
			if (mag && mag.GetAmmoCount() > 0)
			{
				float damage;
				string type;
				if (mag.LocalAcquireCartridge(damage, type))
				{
					weapon.PushCartridgeToChamber(muzzle, damage, type);
					weapon.SelectionBulletShow();
				}
			}
		}

		m_BurstCount = 1;
		
		// Добавляем оставшиеся выстрелы в очередь
		for (int i = 1; i < m_BurstMax; i++)
		{
			m_PendingShots.Insert(new WeaponEventTrigger(m_Bot));
		}

		Print("[MyBots][FIRE] Burst shot 1/" + m_BurstMax);
		m_BurstTimer = BURST_DELAY;

		return true;
	}

	void StopFire()
	{
		m_IsFiring = false;
		m_AimTimer = 0;
		m_RaiseTimer = 0;
		m_BurstCount = 0;
		m_BurstTimer = 0;
		m_PendingShots.Clear();
	}

	void StartReload(Weapon_Base weapon = null)
	{
		if (!weapon)
			weapon = Weapon_Base.Cast(m_Bot.GetItemInHands());

		if (weapon)
		{
			Magazine mag = Magazine.Cast(weapon.GetAttachmentByType(Magazine));
			if (mag)
			{
				int ammoBefore = mag.GetAmmoCount();
				mag.ServerSetAmmoCount(mag.GetAmmoMax());
				Print("[MyBots][RELOAD] Ammo: " + ammoBefore + " -> " + mag.GetAmmoCount());

				int muzzle = weapon.GetCurrentMuzzle();
				if (weapon.IsChamberEmpty(muzzle) || weapon.IsChamberFiredOut(muzzle))
				{
					float damage;
					string type;
					if (mag.LocalAcquireCartridge(damage, type))
					{
						weapon.PushCartridgeToChamber(muzzle, damage, type);
						weapon.SelectionBulletShow();
						Print("[MyBots][RELOAD] Патрон дослан damage=" + damage + " type=" + type);
					}
				}

				m_IsReloading = false;
				m_AimTimer = 0;
				m_RaiseTimer = 0;
				return;
			}
		}

		m_IsReloading = true;
		m_ReloadEndTime = GetGame().GetTickTime() + 2.5;
		StopFire();
	}

	protected void FinishReload()
	{
		Weapon_Base weapon = Weapon_Base.Cast(m_Bot.GetItemInHands());
		if (!weapon)
		{
			m_IsReloading = false;
			return;
		}

		int muzzle = weapon.GetCurrentMuzzle();

		Magazine mag = Magazine.Cast(weapon.GetAttachmentByType(Magazine));
		if (mag)
		{
			mag.ServerSetAmmoCount(mag.GetAmmoMax());

			if (weapon.IsChamberEmpty(muzzle) || weapon.IsChamberFiredOut(muzzle))
			{
				float damage;
				string type;
				if (mag.LocalAcquireCartridge(damage, type))
				{
					weapon.PushCartridgeToChamber(muzzle, damage, type);
					weapon.SelectionBulletShow();
				}
			}
		}

		m_IsReloading = false;
		m_AimTimer = 0;
		m_RaiseTimer = 0;
		Print("[MyBots][RELOAD] Перезарядка завершена");
	}

	bool IsReloading() { return m_IsReloading; }
	bool IsFiring()    { return m_IsFiring; }

	static float CalculateAccuracy(BotAccuracyConfig cfg, float distance, bool isMoving, bool isCrouching, bool isProne)
	{
		float accuracy = cfg.m_BaseAccuracy;
		accuracy -= distance * cfg.m_AccuracyDropPerMeter;
		if (isMoving)    accuracy -= cfg.m_MovingPenalty;
		if (isCrouching) accuracy += cfg.m_CrouchBonus;
		if (isProne)     accuracy += cfg.m_ProneBonus;
		return Math.Clamp(accuracy, 0.05, 1.0);
	}
};
