class BotAimController
{
	protected PlayerBase m_Bot;
	protected float m_BotSpreadX;
	protected float m_BotSpreadY;
	protected float m_BotSpreadTimer;
	protected int m_BotAnimVarRaised;
	protected int m_BotAnimVarAimX;
	protected int m_BotAnimVarAimY;
	static const float BOT_SPREAD_INTERVAL = 0.5;

	void BotAimController(PlayerBase bot)
	{
		m_Bot = bot;
		m_BotSpreadX = 0;
		m_BotSpreadY = 0;
		m_BotSpreadTimer = 0;
		m_BotAnimVarRaised = -1;
		m_BotAnimVarAimX = -1;
		m_BotAnimVarAimY = -1;
	}

	void UpdateAim(vector targetPos, float accuracy, float dt)
	{
		if (!m_Bot)
			return;

		// Разброс пересчитываем редко
		m_BotSpreadTimer -= dt;
		if (m_BotSpreadTimer <= 0)
		{
			float spread = (1.0 - accuracy) * 4.0;
			m_BotSpreadX = Math.RandomFloat(-spread, spread);
			m_BotSpreadY = Math.RandomFloat(-spread * 0.5, spread * 0.5);
			m_BotSpreadTimer = BOT_SPREAD_INTERVAL;
		}

		// Прицеливание теперь через AnimSetFloat в BotSurvivorBase.CommandHandler
	}

	float GetAngleToTarget(vector targetPos)
	{
		if (!m_Bot)
			return 999;

		vector botPos = m_Bot.GetPosition();
		vector toTarget = targetPos - botPos;
		toTarget[1] = 0;
		toTarget = toTarget.Normalized();

		float botYaw = m_Bot.GetOrientation()[0] * Math.DEG2RAD;
		vector botDir = Vector(Math.Sin(botYaw), 0, Math.Cos(botYaw));

		float dot = vector.Dot(botDir, toTarget);
		return Math.Acos(Math.Clamp(dot, -1, 1)) * Math.RAD2DEG;
	}

	bool IsAimedAtTarget(vector targetPos, float threshold = 25.0)
	{
		if (!m_Bot)
			return false;

		return GetAngleToTarget(targetPos) < threshold;
	}

	void RaiseWeapon(bool raise) {}

	void ClearOverrides() {}
};
