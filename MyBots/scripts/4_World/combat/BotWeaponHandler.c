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
	protected bool m_BurstInProgress;
	static const float AIM_TIME = 0.6;
	static const float RAISE_DELAY = 0.5;
	static const float FIRE_RATE = 0.1;  // 100ms между выстрелами в очереди

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
		m_BurstInProgress = false;
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

		// Если очередь в процессе, не начинаем новую
		if (m_BurstInProgress)
			return false;

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

		// Первый выстрел в очереди
		Print("[MyBots][FIRE] ProcessWeaponEvent(TRIGGER) weapon=" + weapon.GetType() + " muzzle=" + muzzle + " ammo=" + weapon.GetMagazine(muzzle).GetAmmoCount());
		weapon.ProcessWeaponEvent(new WeaponEventTrigger(m_Bot));

		m_BurstCount = 1;
		m_BurstInProgress = true;

		// Запускаем таймер для следующих выстрелов в очереди (ФИКСИРОВАННАЯ ЗАДЕРЖКА)
		if (m_BurstMax > 1)
		{
			int delayMs = (int)(FIRE_RATE * 1000);  // 100ms
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BotWeaponHandler.ContinueBurst, delayMs, false, this, weapon);
		}
		else
		{
			m_BurstInProgress = false;
			m_NextFireTime = now2 + Math.RandomFloat(m_FirePauseMin, m_FirePauseMax);
		}

		return true;
	}

	// Продолжение очереди выстрелов
	static void ContinueBurst(BotWeaponHandler self, Weapon_Base weapon)
	{
		if (!self || !self.m_Bot || self.m_Bot.IsDamageDestroyed())
			return;

		if (!weapon || weapon.IsDamageDestroyed())
			return;

		int muzzle = weapon.GetCurrentMuzzle();

		// Проверяем патронник и досылаем патрон если нужно
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
			else
			{
				Print("[MyBots][FIRE] Magazine empty during burst, starting reload");
				self.StartReload(weapon);
				self.m_BurstInProgress = false;
				return;
			}
		}

		// Проверяем можно ли стрелять
		if (!weapon.CanFire(muzzle))
		{
			Print("[MyBots][FIRE] CanFire=false during burst");
			self.StartReload(weapon);
			self.m_BurstInProgress = false;
			return;
		}

		// Выстрел
		self.m_BurstCount++;
		Print("[MyBots][FIRE] Burst shot " + self.m_BurstCount + "/" + self.m_BurstMax);
		weapon.ProcessWeaponEvent(new WeaponEventTrigger(self.m_Bot));

		// Проверяем конец очереди
		if (self.m_BurstCount >= self.m_BurstMax)
		{
			Print("[MyBots][FIRE] Burst завершён (" + self.m_BurstMax + "), пауза");
			self.m_BurstInProgress = false;
			self.m_NextFireTime = GetGame().GetTickTime() + Math.RandomFloat(self.m_FirePauseMin, self.m_FirePauseMax);
			return;
		}

		// Следующий выстрел в burst (ФИКСИРОВАННАЯ ЗАДЕРЖКА)
		int delayMs = (int)(FIRE_RATE * 1000);  // 100ms
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BotWeaponHandler.ContinueBurst, delayMs, false, self, weapon);
	}

	void StopFire()
	{
		m_IsFiring = false;
		m_AimTimer = 0;
		m_RaiseTimer = 0;
		m_BurstCount = 0;
		m_BurstInProgress = false;
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
				m_BurstInProgress = false;
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
		m_BurstInProgress = false;
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
