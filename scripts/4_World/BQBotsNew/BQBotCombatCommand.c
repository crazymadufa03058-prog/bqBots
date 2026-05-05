class BQBotCombatCommand extends HumanCommandScript
{
	protected Human m_Human;
	protected bqBots_Boris m_Player;
	protected int m_VarRaised;
	protected bool m_WantRaised;
	protected bool m_Valid;
	protected bool m_TurnFeetWanted;

	void BQBotCombatCommand(Human pHuman)
	{
		m_Human = pHuman;
		m_Player = bqBots_Boris.Cast(pHuman);
		m_WantRaised = false;
		m_Valid = true;

		HumanAnimInterface anim = m_Human.GetAnimInterface();
		if (anim)
		{
			m_VarRaised = anim.BindVariableBool("Raised");
		}
		else
		{
			m_VarRaised = -1;
		}
	}

	void Invalidate()
	{
		m_Valid = false;
	}

	void SetRaised(bool raised)
	{
		m_WantRaised = raised;
	}

	override int GetCurrentStance()
	{
		return DayZPlayerConstants.STANCEIDX_RAISEDERECT;
	}

	override int GetCurrentMovement()
	{
		if (!m_Player)
			return DayZPlayerConstants.MOVEMENT_IDLE;

		float moveSpeed = 0.0;
		float moveAngle = 0.0;
		bool hasMoveIntent = m_Player.BQBot_GetMoveIntent(moveSpeed, moveAngle);
		if (!hasMoveIntent)
		{
			HumanInputController hic = m_Player.GetInputController();
			if (!hic)
				return DayZPlayerConstants.MOVEMENT_IDLE;

			vector moveDir = "0 0 0";
			hic.GetMovement(moveSpeed, moveDir);
		}

		if (moveSpeed >= 2.5)
			return DayZPlayerConstants.MOVEMENT_SPRINT;
		if (moveSpeed >= 1.5)
			return DayZPlayerConstants.MOVEMENT_RUN;
		if (moveSpeed >= 0.5)
			return DayZPlayerConstants.MOVEMENT_WALK;

		return DayZPlayerConstants.MOVEMENT_IDLE;
	}

	override void OnActivate()
	{
		m_WantRaised = true;
		m_TurnFeetWanted = false;
	}

	override void PreAnimUpdate(float pDt)
	{
		if (m_VarRaised >= 0)
			PreAnim_SetBool(m_VarRaised, m_WantRaised);

		// BQBot Phase 1: pose raise is centralized in BQBotCommandMove.PreAnimUpdate
		// via OverrideRaise(ONE_FRAME). This command only manages the PreAnim bool
		// (m_VarRaised) — no HIC override here.

		if (m_Player)
		{
			float aimYaw = 0.0;
			bool wantFeetTurn = false;
			if (m_Player.BQBot_GetAimHeading(aimYaw, wantFeetTurn))
			{
				m_TurnFeetWanted = false;
			}
			else
			{
				m_TurnFeetWanted = false;
			}
		}
		else
		{
			m_TurnFeetWanted = false;
		}
	}

	override void PrePhysUpdate(float pDt)
	{
		// Keep engine translation intact so movement overrides can move the bot
		// while combat pose command is active.
	}

	override bool PostPhysUpdate(float pDt)
	{
		return m_Valid;
	}

	override void OnDeactivate()
	{
		m_WantRaised = false;
		m_TurnFeetWanted = false;
	}
}
