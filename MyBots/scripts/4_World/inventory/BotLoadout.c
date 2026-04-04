class BotLoadout
{
	static void Equip(PlayerBase bot, BotLoadoutConfig cfg)
	{
		if (!bot || !cfg)
			return;

		BotLoadout.EquipWeapon(bot, cfg);
		BotLoadout.EquipClothing(bot, cfg.m_Clothing);

		Print("[MyBots] Бот экипирован: " + cfg.m_Name);
	}

	static void EquipWeapon(PlayerBase bot, BotLoadoutConfig cfg)
	{
		if (cfg.m_Weapons.Count() == 0)
			return;

		BotWeaponConfig wCfg = cfg.m_Weapons[0];

		Weapon_Base weapon = Weapon_Base.Cast(bot.GetHumanInventory().CreateInHands(wCfg.m_Type));
		if (!weapon)
			return;

		// Крепим аттачменты к оружию
		for (int i = 0; i < wCfg.m_Attachments.Count(); i++)
			weapon.GetInventory().CreateAttachment(wCfg.m_Attachments[i]);

		// Создаём магазин и вставляем его в оружие
		if (wCfg.m_Magazine != "" && wCfg.m_AmmoCount > 0)
		{
			// Первый магазин — вставляем в оружие
			Magazine mag = Magazine.Cast(weapon.GetInventory().CreateAttachment(wCfg.m_Magazine));
			if (mag)
			{
				mag.ServerSetAmmoCount(mag.GetAmmoMax());
				// Досылаем патрон в патронник через стандартную утилиту
				pushToChamberFromAttachedMagazine(weapon, 0);
				Print("[MyBots] Магазин вставлен и патрон дослан: " + wCfg.m_Magazine);
			}
			else
			{
				Print("[MyBots] ОШИБКА: не удалось вставить магазин " + wCfg.m_Magazine);
			}

			// Запасные магазины в инвентарь
			for (int a = 1; a < wCfg.m_AmmoCount; a++)
			{
				Magazine spareMag = Magazine.Cast(bot.GetInventory().CreateInInventory(wCfg.m_Magazine));
				if (spareMag)
					spareMag.ServerSetAmmoCount(spareMag.GetAmmoMax());
			}
		}
	}

	static void EquipClothing(PlayerBase bot, BotClothingConfig cfg)
	{
		if (!cfg)
			return;

		if (cfg.m_Head.Count() > 0)
			BotLoadout.CreateRandomAttachment(bot, cfg.m_Head, "Headgear");
		if (cfg.m_Top.Count() > 0)
			BotLoadout.CreateRandomAttachment(bot, cfg.m_Top, "Body");
		if (cfg.m_Pants.Count() > 0)
			BotLoadout.CreateRandomAttachment(bot, cfg.m_Pants, "Legs");
		if (cfg.m_Feet.Count() > 0)
			BotLoadout.CreateRandomAttachment(bot, cfg.m_Feet, "Feet");
		if (cfg.m_Vest.Count() > 0)
			BotLoadout.CreateRandomAttachment(bot, cfg.m_Vest, "Vest");
		if (cfg.m_Backpack.Count() > 0)
			BotLoadout.CreateRandomAttachment(bot, cfg.m_Backpack, "Back");
	}

	static void CreateRandomAttachment(PlayerBase bot, array<string> items, string slot)
	{
		if (items.Count() == 0)
			return;

		int idx = Math.RandomInt(0, items.Count());
		EntityAI item = bot.GetInventory().CreateAttachment(items[idx]);
		if (item)
			item.SetHealth("", "", Math.RandomFloat(30, 100));
	}
};