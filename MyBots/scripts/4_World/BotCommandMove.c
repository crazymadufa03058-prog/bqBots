// Серверный командный скрипт движения бота через HumanCommandScript.
class BotCommandMove extends HumanCommandScript
{
	protected Human  m_Human;
	protected vector m_TargetPos;
	protected float  m_MoveSpeed;
	protected bool   m_HasTarget;

	void BotCommandMove(Human pHuman)
	{
		m_Human     = pHuman;
		m_HasTarget = false;
		m_MoveSpeed = 1.0;
		m_TargetPos = "0 0 0";
	}

	void SetTarget(vector pos, float speed)
	{
		m_TargetPos = pos;
		m_MoveSpeed = speed;
		m_HasTarget = true;
	}

	void ClearTarget()
	{
		m_HasTarget = false;
	}

	override int GetCurrentMovement()
	{
		if (!m_HasTarget || m_MoveSpeed <= 0)
			return DayZPlayerConstants.MOVEMENT_IDLE;
		if (m_MoveSpeed >= 2.0)
			return DayZPlayerConstants.MOVEMENT_RUN;
		return DayZPlayerConstants.MOVEMENT_WALK;
	}

	override int GetCurrentStance()
	{
		return DayZPlayerConstants.STANCEIDX_ERECT;
	}

	// Вызывается ДО обработки анимации — задаём направление
	override void PreAnimUpdate(float pDt)
	{
		if (!m_HasTarget || !m_Human)
			return;

		vector curPos = m_Human.GetPosition();
		vector toTarget = m_TargetPos - curPos;
		toTarget[1] = 0;

		if (toTarget.Length() < 0.3)
			return;

		toTarget = toTarget.Normalized();
		float heading = Math.Atan2(toTarget[0], toTarget[2]);
		SetHeading(heading);
	}

	// Вызывается ПОСЛЕ анимации, ДО физики — задаём смещение
	override void PrePhysUpdate(float pDt)
	{
		if (!m_HasTarget || m_MoveSpeed <= 0 || !m_Human)
		{
			PrePhys_SetTranslation("0 0 0");
			return;
		}

		vector curPos = m_Human.GetPosition();
		vector toTarget = m_TargetPos - curPos;
		toTarget[1] = 0;
		float dist = toTarget.Length();

		if (dist < 0.3)
		{
			m_HasTarget = false;
			PrePhys_SetTranslation("0 0 0");
			SetFlagFinished(true);
			return;
		}

		float speedMS;
		if (m_MoveSpeed >= 2.0)
			speedMS = 5.5;
		else
			speedMS = 1.5;

		float step = Math.Min(speedMS * pDt, dist);
		PrePhys_SetTranslation(Vector(0, 0, step));
	}

	// Возвращает true = команда продолжает работать
	override bool PostPhysUpdate(float pDt)
	{
		return m_HasTarget;
	}
};