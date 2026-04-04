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
	static const float AIM_TIME = 0.6;
	static const float RAISE_DELAY = 0.5;

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

		if (m_IsReloading)
		{
			float now = GetGame().GetTickTime();
			if (now < m_ReloadEndTime)
				return false;
			FinishReload();
		}

		// Проверяем стойку через GetMovementState
		ref HumanMovementState ms = new HumanMovementState();
		m_Bot.GetMovementState(ms);
		bool isRaised = ms.IsRaised();
		if (!isRaised)
		{
			m_RaiseTimer = 0;
			m_AimTimer = 0;
			return false;
		}

		// Ждём пока анимация поднятия пройдёт
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

		// Проверяем можно ли стрелять (патронник, заедание)
		if (!weapon.CanFire(muzzle))
		{
			bool chamberEmpty = weapon.IsChamberEmpty(muzzle);
			bool chamberFiredOut = weapon.IsChamberFiredOut(muzzle);
			Print("[MyBots][FIRE] weapon.CanFire=false chamberEmpty=" + chamberEmpty + " chamberFiredOut=" + chamberFiredOut);
			if (chamberEmpty || chamberFiredOut)
				StartReload(weapon);
			return false;
		}

		// Прямой вызов через ProcessWeaponEvent — обходит WeaponManager.IsRaised()
		Print("[MyBots][FIRE] ProcessWeaponEvent(TRIGGER) weapon=" + weapon.GetType() + " muzzle=" + muzzle + " ammo=" + weapon.GetMagazine(muzzle).GetAmmoCount());
		weapon.ProcessWeaponEvent(new WeaponEventTrigger(m_Bot));

		// Сброс FSM + досыл патрона после выстрела
		float reloadTime = weapon.GetReloadTime(muzzle) + 0.05;
		int delayMs = (int)(reloadTime * 1000);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BotWeaponHandler.ResetAndChamberSafe, delayMs, false, this, weapon);

		m_NextFireTime = now2 + Math.RandomFloat(m_FirePauseMin, m_FirePauseMax);
		return true;
	}

	// Статический wrapper для CallLater
	static void ResetAndChamberSafe(BotWeaponHandler self, Weapon_Base weapon)
	{
		if (!self || !self.m_Bot || self.m_Bot.IsDamageDestroyed())
			return;
		self.ResetAndChamber(weapon);
	}

	void ResetAndChamber(Weapon_Base weapon)
	{
		if (!weapon || weapon.IsDamageDestroyed())
			return;
		if (!m_Bot || m_Bot.IsDamageDestroyed())
			return;

		int muzzle = weapon.GetCurrentMuzzle();

		// 2. Если патронник пуст — досылаем патрон из магазина
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
					Print("[MyBots][FIRE] Chamber reloaded, ammo=" + mag.GetAmmoCount());
				}
				else
				{
					Print("[MyBots][FIRE][ERROR] LocalAcquireCartridge failed during ResetAndChamber");
				}
			}
			else
			{
				Print("[MyBots][FIRE] Magazine empty after shot, need reload");
				StartReload(weapon);
				m_BurstCount = 0;
				return;
			}
		}

		// 3. Burst логика
		m_BurstCount++;
		if (m_BurstCount >= m_BurstMax)
		{
			m_BurstCount = 0;
			Print("[MyBots][FIRE] Burst завершён (" + m_BurstMax + "), пауза");
			return;
		}

		// 4. Следующий выстрел в burst
		if (weapon.CanFire(muzzle))
		{
			Print("[MyBots][FIRE] Burst shot " + (m_BurstCount + 1) + "/" + m_BurstMax);
			weapon.ProcessWeaponEvent(new WeaponEventTrigger(m_Bot));

			float reloadTime = weapon.GetReloadTime(muzzle) + 0.05;
			int delayMs = (int)(reloadTime * 1000);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BotWeaponHandler.ResetAndChamberSafe, delayMs, false, this, weapon);
		}
	}

	void StopFire()
	{
		m_IsFiring = false;
		m_AimTimer = 0;
		m_RaiseTimer = 0;
		m_BurstCount = 0;
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
					else
					{
						Print("[MyBots][RELOAD][ERROR] LocalAcquireCartridge failed");
					}
				}

				m_IsReloading = false;
				m_AimTimer = 0;
				m_RaiseTimer = 0;
				return;
			}
			else
			{
				Print("[MyBots][RELOAD][ERROR] No magazine on " + weapon.GetType());
			}
		}
		else
		{
			Print("[MyBots][RELOAD][ERROR] No weapon in hands");
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
