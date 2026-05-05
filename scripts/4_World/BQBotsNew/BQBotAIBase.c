class bqBots_Boris : eAIBase
{
	void bqBots_Boris()
	{
	}

	// Compatibility overload for old BQ callers. The actual path owner is ExpansionPathHandler in eAIBase.
	void OverrideTargetPosition(vector targetPos, bool isFinal = true, float maxDistance = 1.0, bool allowJumpClimb = true, int timeoutMs = 0)
	{
		super.OverrideTargetPosition(targetPos, isFinal, maxDistance, allowJumpClimb);
		if (timeoutMs > 0)
			eAI_SetPositionOverrideTimeoutMS(timeoutMs);
	}

	override void OverrideTargetPosition(eAITarget target, bool isFinal = true)
	{
		super.OverrideTargetPosition(target, isFinal);
	}
	void OverrideTargetPosition(PlayerBase target, bool isFinal = true, float maxDistance = 1.0, bool allowJumpClimb = true, int timeoutMs = 0)
	{
		if (!target)
		{
			ClearOverrideTargetPosition();
			return;
		}

		OverrideTargetPosition(target.GetPosition(), isFinal, maxDistance, allowJumpClimb, timeoutMs);
	}

	void ClearOverrideTargetPosition()
	{
		ExpansionPathHandler pathFinding = GetPathFinding();
		if (pathFinding)
		{
			pathFinding.StopOverride();
			pathFinding.SetTarget(GetPosition(), 1.0, true);
			pathFinding.ResetUnreachable();
		}

		m_eAI_TargetPosition = GetPosition();
		m_eAI_TargetPositionIsFinal = true;
		eAI_SetPositionOverrideTimeoutMS(0);
	}

	bool HasOverrideTargetPosition()
	{
		return m_eAI_TargetPosition != vector.Zero;
	}

	vector GetOverrideTargetPosition()
	{
		return m_eAI_TargetPosition;
	}

	bool IsTargetPositionFinal()
	{
		return m_eAI_TargetPositionIsFinal;
	}

	bool HasMovementDirectionOverride()
	{
		return false;
	}

	float GetMovementDirectionOverride()
	{
		return 0.0;
	}

	bool HasMovementSpeedOverride()
	{
		return false;
	}

	float GetMovementSpeedOverride()
	{
		return 0.0;
	}
	override bool IsRaised()
	{
		return super.IsRaised();
	}

	override bool IsWeaponRaiseCompleted()
	{
		return super.IsWeaponRaiseCompleted();
	}
	PlayerBase eAI_GetTarget()
	{
		return BQBot_GetCombatTarget();
	}

	float eAI_GetThreatToSelf(bool ignoreLOS = false)
	{
		return BQBot_GetThreatToSelf(ignoreLOS);
	}


	ExpansionPathHandler BQBot_GetExpansionPathFinding()
	{
		return GetPathFinding();
	}

	void BQBot_ExpansionMovementTick(float pDt)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!GetGroup())
			eAIGroup.GetGroupByLeader(this);
		if (!GetGroup())
			return;

		if (GetMovementSpeedLimit() <= 0)
			SetMovementSpeedLimits(3, 3);


		if (!m_eAI_CommandMove)
			return;

		if (!GetCommand_Move() && !GetCommand_Climb() && !GetCommand_Ladder() && !GetCommand_Vehicle() && !IsSwimming() && !IsFalling() && !IsUnconscious())
			StartCommand_Move();

		GetMovementState(m_MovementState);
		m_eAI_CurrentCommandID = m_MovementState.m_CommandTypeId;

		if (GetCommand_Move())
		{
			m_eAI_CommandMove.PreAnimUpdate(pDt);
		}
	}
}

class bqBots_Boris_AI : bqBots_Boris
{
	void bqBots_Boris_AI()
	{
	}
}