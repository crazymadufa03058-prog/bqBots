enum EBotEventType
{
	START,
	STOP,
	ENEMY_DETECTED,
	ENEMY_CONFIRMED,
	ENEMY_LOST,
	ENEMY_KILLED,
	TOOK_DAMAGE,
	FLANK_TIMER,
	TIMEOUT,
	HEARD_SOUND,
	RETREAT,
	RETREAT_COMPLETE
};

class BotEvent
{
	EBotEventType m_Type;
	EntityAI m_Target;
	vector m_Position;
	float m_Data;

	void BotEvent(EBotEventType type, EntityAI target = NULL, vector pos = "0 0 0", float data = 0)
	{
		m_Type = type;
		m_Target = target;
		m_Position = pos;
		m_Data = data;
	}
};
