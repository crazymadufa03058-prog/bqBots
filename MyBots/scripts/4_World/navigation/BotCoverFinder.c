class BotCoverFinder
{
	protected PlayerBase m_Bot;
	protected ref BotNavigation m_Nav;

	void BotCoverFinder(PlayerBase bot, BotNavigation nav)
	{
		m_Bot = bot;
		m_Nav = nav;
	}

	vector FindBestCover(vector threatPos, float searchRadius = 15.0)
	{
		vector botPos = m_Bot.GetPosition();
		vector toThreat = threatPos - botPos;
		toThreat.Normalize();

		vector bestCover = "0 0 0";
		float bestScore = -1;

		int directions = 8;
		for (int i = 0; i < directions; i++)
		{
			float angle = (i / directions) * Math.PI * 2;
			vector dir = Vector(Math.Cos(angle), 0, Math.Sin(angle));

			float checkDist = Math.RandomFloat(5, searchRadius);
			vector checkPos = botPos + dir * checkDist;
			checkPos = m_Nav.GetNearestNavmeshPos(checkPos, searchRadius);

			if (checkPos == "0 0 0")
				continue;

			vector eyePos = checkPos + "0 1.5 0";
			vector threatEye = threatPos + "0 1.5 0";

			vector contactPos;
			vector contactDir;
			int contactComponent;

			bool blocked = DayZPhysics.RaycastRV(eyePos, threatEye, contactPos, contactDir, contactComponent, NULL, NULL, m_Bot);

			if (!blocked)
				continue;

			float dist = vector.Distance(botPos, checkPos);
			float score = 100.0 / (dist + 1);

			float dot = vector.Dot(dir, toThreat);
			if (dot < 0)
				score *= 2;

			if (score > bestScore)
			{
				bestScore = score;
				bestCover = checkPos;
			}
		}

		return bestCover;
	}

	vector FindBuildingCover(vector threatPos, float searchRadius = 50.0)
	{
		vector buildingPos = m_Nav.FindNearestBuilding(searchRadius);
		if (buildingPos == "0 0 0")
			return "0 0 0";

		vector botPos = m_Bot.GetPosition();
		vector entryPos = m_Nav.GetNearestNavmeshPos(buildingPos, 10);

		if (entryPos == "0 0 0")
			return "0 0 0";

		vector eyePos = entryPos + "0 1.5 0";
		vector threatEye = threatPos + "0 1.5 0";

		vector contactPos;
		vector contactDir;
		int contactComponent;

		bool blocked = DayZPhysics.RaycastRV(eyePos, threatEye, contactPos, contactDir, contactComponent, NULL, NULL, m_Bot);

		if (blocked)
			return entryPos;

		array<Object> nearbyObjects = new array<Object>();
		array<CargoBase> proxyCargos = new array<CargoBase>();
		GetGame().GetObjectsAtPosition(botPos, searchRadius, nearbyObjects, proxyCargos);

		vector bestPos = "0 0 0";
		float bestDist = searchRadius + 1;

		for (int i = 0; i < nearbyObjects.Count(); i++)
		{
			Object obj = nearbyObjects[i];
			if (!obj.IsBuilding() && !obj.IsInherited(BuildingSuper))
				continue;

			vector bPos = obj.GetPosition();
			float dist = vector.Distance(botPos, bPos);

			if (dist > bestDist)
				continue;

			vector navPos = m_Nav.GetNearestNavmeshPos(bPos, 10);
			if (navPos == "0 0 0")
				continue;

			vector testEye = navPos + "0 1.5 0";
			bool testBlocked = DayZPhysics.RaycastRV(testEye, threatEye, contactPos, contactDir, contactComponent, NULL, NULL, m_Bot);

			if (testBlocked)
			{
				bestDist = dist;
				bestPos = navPos;
			}
		}

		return bestPos;
	}

	vector FindSafeRetreatPosition(vector threatPos, float minDist = 80.0)
	{
		vector botPos = m_Bot.GetPosition();
		vector fromThreat = botPos - threatPos;
		fromThreat[1] = 0;
		fromThreat.Normalize();

		float retreatDist = Math.RandomFloat(minDist, minDist + 40);
		vector retreatPos = botPos + fromThreat * retreatDist;
		retreatPos = m_Nav.GetNearestNavmeshPos(retreatPos, 30);

		vector buildingPos = FindBuildingCover(threatPos, 80.0);
		if (buildingPos != "0 0 0")
		{
			float buildingDist = vector.Distance(botPos, buildingPos);
			float retreatTargetDist = vector.Distance(botPos, retreatPos);

			if (buildingDist < retreatTargetDist)
				return buildingPos;
		}

		return retreatPos;
	}

	vector FindFlankPosition(vector enemyPos, float flankDist = 40.0)
	{
		vector botPos = m_Bot.GetPosition();
		vector toEnemy = enemyPos - botPos;
		toEnemy.Normalize();

		vector perp = Vector(-toEnemy[2], 0, toEnemy[0]);

		if (Math.RandomFloat01() > 0.5)
			perp = perp * -1;

		vector flankPos = enemyPos + perp * flankDist;
		flankPos = m_Nav.GetNearestNavmeshPos(flankPos, 30);

		return flankPos;
	}

	vector FindFlankPositionCoordinated(vector enemyPos, bool goLeft, float flankDist = 40.0)
	{
		vector botPos = m_Bot.GetPosition();
		vector toEnemy = enemyPos - botPos;
		toEnemy.Normalize();

		vector perp = Vector(-toEnemy[2], 0, toEnemy[0]);

		if (!goLeft)
			perp = perp * -1;

		vector flankPos = enemyPos + perp * flankDist;
		flankPos = m_Nav.GetNearestNavmeshPos(flankPos, 30);

		return flankPos;
	}
};
