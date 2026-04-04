class BotCommandCombat extends HumanCommandScript
{
	protected Human      m_Human;
	protected PlayerBase m_Player;
	protected bool       m_Valid;
	protected float      m_StunTimer;
	protected int        m_VarRaised;
	protected bool       m_WantRaised;
	protected float      m_DesiredAimX;
	protected float      m_DesiredAimY;

	void BotCommandCombat(Human pHuman)
	{
		m_Human     = pHuman;
		m_Player    = PlayerBase.Cast(pHuman);
		m_Valid     = true;
		m_StunTimer = 0;
		m_WantRaised = false;
		m_DesiredAimX = 0;
		m_DesiredAimY = 0;

		HumanAnimInterface anim = m_Human.GetAnimInterface();
		if (anim)
		{
			m_VarRaised = anim.BindVariableBool("Raised");
			Print("[MyBots][CMD] BindVars: Raised=" + m_VarRaised);
		}
		else
		{
			m_VarRaised = -1;
			Print("[MyBots][CMD][ERROR] GetAnimInterface() returned NULL");
		}
	}

	void Invalidate()
	{
		m_Valid = false;
	}

	void SetStun(float seconds)
	{
		m_StunTimer = seconds;
	}

	void SetRaised(bool raised)
	{
		m_WantRaised = raised;
	}

	void SetAim(float aimX, float aimY)
	{
		m_DesiredAimX = aimX;
		m_DesiredAimY = aimY;
	}

	override int GetCurrentStance()
	{
		return DayZPlayerConstants.STANCEIDX_RAISEDERECT;
	}

	override int GetCurrentMovement()
	{
		return DayZPlayerConstants.MOVEMENT_IDLE;
	}

	override void OnActivate()
	{
		Print("[MyBots][CMD_DEBUG] OnActivate: C++ активировал HumanCommandScript для " + m_Player.GetType());
		m_WantRaised = true;
		m_DesiredAimX = 0;
		m_DesiredAimY = 0;
	}

	override void PreAnimUpdate(float pDt)
	{
		if (m_VarRaised >= 0)
			PreAnim_SetBool(m_VarRaised, m_WantRaised);
	}

	override void PrePhysUpdate(float pDt)
	{
		PrePhys_SetTranslation("0 0 0");
		// Каждый кадр сбрасываем Aim к целевому значению — это предотвращает накопление отдачи
		if (m_Player)
		{
			m_Player.AnimSetFloat(13, m_DesiredAimX);
			m_Player.AnimSetFloat(12, m_DesiredAimY);
		}
	}

	override bool PostPhysUpdate(float pDt)
	{
		return true;
	}

	override void OnDeactivate()
	{
		m_WantRaised = false;
		m_DesiredAimX = 0;
		m_DesiredAimY = 0;
	}

	bool CanFire()
	{
		if (!m_Valid)
			return false;
		if (m_StunTimer > 0)
			return false;
		return true;
	}
};
