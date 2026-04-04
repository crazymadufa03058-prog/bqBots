class BotSpawner
{
	// Список мужских и женских моделей для рандома
	static const string BOT_MODELS_MALE[] = {
		"SurvivorM_Mirek",
		"SurvivorM_Guo",
		"SurvivorM_Franta",
		"SurvivorM_Resort"
	};

	static const string BOT_MODELS_FEMALE[] = {
		"SurvivorF_Eva",
		"SurvivorF_Gena",
		"SurvivorF_Linda"
	};

	static PlayerBase SpawnBot(vector pos, BotLoadoutConfig loadout)
	{
		// Случайная модель
		string model;
		if (Math.RandomInt(0, 2) == 0)
			model = BOT_MODELS_MALE[Math.RandomInt(0, 4)];
		else
			model = BOT_MODELS_FEMALE[Math.RandomInt(0, 3)];

		PlayerBase bot = PlayerBase.Cast(
			GetGame().CreateObjectEx(model, pos, ECE_PLACE_ON_SURFACE | ECE_CREATEPHYSICS, RF_DEFAULT));

		if (!bot)
		{
			Print("[MyBots] ОШИБКА: Не удалось создать бота в позиции " + pos.ToString());
			return NULL;
		}

		bot.SetAllowDamage(true);
		bot.SetHealth("", "Health", 100);
		bot.SetHealth("", "Blood", 5000);

		if (loadout)
			BotLoadout.Equip(bot, loadout);

		Print("[MyBots] Бот заспавнен в " + pos.ToString());
		return bot;
	}

	static void DespawnBot(PlayerBase bot)
	{
		if (!bot)
			return;

		vector deathPos = bot.GetPosition();
		vector deathOrient = bot.GetOrientation();

		Print("[MyBots] Бот деспавнится в " + deathPos.ToString());

		// Запускаем анимацию смерти — бот красиво падает
		float deathDir = deathOrient[0] * Math.DEG2RAD;
		bot.StartCommand_Death(0, deathDir, HumanCommandDeathCallback, false);

		// Удаляем через 4 секунды — после окончания анимации падения
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BotSpawner.DeleteBot, 4000, false, bot);
	}

	static void DeleteBot(PlayerBase bot)
	{
		if (bot && !bot.IsBotDeleted())
			GetGame().ObjectDelete(bot);
	}

	// Создаёт статичный труп с предметами бота
	static void SpawnCorpse(vector pos, vector orient, PlayerBase bot)
	{
		// Небольшой случайный сдвиг чтобы труп не проваливался в землю
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]) + 0.05;

		// Выбираем модель трупа — та же что у бота
		string model = bot.GetType();

		Object corpse = GetGame().CreateObjectEx(model, pos, ECE_PLACE_ON_SURFACE | ECE_CREATEPHYSICS, RF_DEFAULT);
		if (!corpse)
			return;

		corpse.SetOrientation(orient);

		// Перекладываем предметы из рук бота на труп
		EntityAI weapon = bot.GetItemInHands();
		if (weapon)
		{
			EntityAI corpseEntity = EntityAI.Cast(corpse);
			if (corpseEntity)
			{
				// Спавним копию оружия рядом
				vector dropPos = pos + "0.3 0.1 0";
				GetGame().CreateObjectEx(weapon.GetType(), dropPos, ECE_PLACE_ON_SURFACE | ECE_CREATEPHYSICS, RF_DEFAULT);
			}
		}

		// Труп исчезнет через 5 минут чтобы не нагружать сервер
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BotSpawner.DeleteCorpse, 300000, false, corpse);

		Print("[MyBots] Труп создан в " + pos.ToString());
	}

	static void DeleteCorpse(Object corpse)
	{
		if (corpse)
			GetGame().ObjectDelete(corpse);
	}

	static vector GetSpawnPosition(BotZoneConfig zone)
	{
		if (zone.m_Waypoints.Count() > 0)
		{
			int idx = Math.RandomInt(0, zone.m_Waypoints.Count());
			vector wp = zone.m_Waypoints[idx];
			// Исправляем Y если 0 — ставим на поверхность
			wp[1] = GetGame().SurfaceY(wp[0], wp[2]);
			return wp;
		}

		return BotNavigation.GetRandomPointInRadius(zone.m_Center, zone.m_Radius);
	}
};