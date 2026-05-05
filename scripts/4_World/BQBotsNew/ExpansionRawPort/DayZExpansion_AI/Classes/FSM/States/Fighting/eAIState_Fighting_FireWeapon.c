class eAIState_Fighting_FireWeapon: eAIState
{
	float m_Time;
	eAITarget m_Target;
	Weapon_Base m_Weapon;

	override void OnEntry(string Event, ExpansionState From)
	{
		string fromName = "NULL";
		if (From)
			fromName = From.GetName();

		string targetStr = "NULL";
		if (m_Target && m_Target.GetEntity())
			targetStr = m_Target.GetEntity().ToString() + " pos=" + m_Target.GetPosition().ToString();

		Print("[EAI] [state_enter] bot=" + unit.ToString() + " state=eAIState_Fighting_FireWeapon target=" + targetStr + " distSq=" + unit.m_eAI_DistanceToTargetSq.ToString());
		string fwEntry = "bot=" + unit.ToString();
		fwEntry = fwEntry + " from=" + fromName;
		fwEntry = fwEntry + " target=" + targetStr;
		fwEntry = fwEntry + " weapon=" + BQExpansionDiag.Obj(m_Weapon);
		fwEntry = fwEntry + " raised=" + BQExpansionDiag.B(unit.IsRaised());
		fwEntry = fwEntry + " raiseDone=" + BQExpansionDiag.B(unit.IsWeaponRaiseCompleted());
		fwEntry = fwEntry + " los=" + BQExpansionDiag.B(m_Target && m_Target.m_LOS);
		BQExpansionDiag.Log("FW_ENTRY", fwEntry);

		unit.eAI_RaiseWeapon(true);
		m_Time = 0;
		unit.m_eAI_LastFireTime = g_Game.GetTime();

		if (m_Weapon)
			unit.eAI_AdjustStance(m_Weapon, unit.m_eAI_DistanceToTargetSq);
	}

	override void OnExit(string Event, bool Aborted, ExpansionState To)
	{
		string toName = "NULL";
		if (To)
			toName = To.GetName();
		string fwExit = "bot=" + unit.ToString();
		fwExit = fwExit + " event=" + Event;
		fwExit = fwExit + " aborted=" + BQExpansionDiag.B(Aborted);
		fwExit = fwExit + " to=" + toName;
		fwExit = fwExit + " time=" + m_Time.ToString();
		BQExpansionDiag.Log("FW_EXIT", fwExit);
	}

	override int OnUpdate(float DeltaTime, int SimulationPrecision)
	{
		if (unit.eAI_IsChangingStance())
		{
			BQExpansionDiag.Log("FW_UPDATE", "WAIT reason=changing_stance bot=" + unit.ToString() + " time=" + m_Time.ToString());
			return CONTINUE;
		}

		if (!m_Target || !m_Target.GetEntity())
		{
			BQExpansionDiag.Log("FW_UPDATE", "EXIT reason=no_target bot=" + unit.ToString());
			return EXIT;
		}

		vector lowPosition = m_Target.GetPosition(false);
		vector aimPosition = lowPosition + m_Target.GetAimOffset();

		m_Time += DeltaTime;
		unit.OverrideTargetPosition(m_Target);

		string fwUpd = "bot=" + unit.ToString();
		fwUpd = fwUpd + " time=" + m_Time.ToString();
		fwUpd = fwUpd + " target=" + m_Target.GetEntity().ToString();
		fwUpd = fwUpd + " targetPos=" + m_Target.GetPosition().ToString();
		fwUpd = fwUpd + " aim=" + aimPosition.ToString();
		fwUpd = fwUpd + " targetLOS=" + BQExpansionDiag.B(m_Target.m_LOS);
		fwUpd = fwUpd + " raised=" + BQExpansionDiag.B(unit.IsRaised());
		fwUpd = fwUpd + " raiseDone=" + BQExpansionDiag.B(unit.IsWeaponRaiseCompleted());
		fwUpd = fwUpd + " distSq=" + unit.m_eAI_DistanceToTargetSq.ToString();
		BQExpansionDiag.Log("FW_UPDATE", fwUpd);

		if (!unit.IsRaised() || !unit.IsWeaponRaiseCompleted())
		{
			unit.eAI_RaiseWeapon(true);
			unit.LookAtPosition(aimPosition);
			unit.AimAtPosition(aimPosition);
			unit.BQBot_SetAimTarget(aimPosition);

			if (m_Time >= 0.5)
			{
				BQExpansionDiag.Log("FW_UPDATE", "EXIT reason=raise_timeout bot=" + unit.ToString());
				m_Time = 0;
				return EXIT;
			}

			return CONTINUE;
		}

		vector neck = unit.GetBonePositionWS(unit.GetBoneIndexByName("neck"));
		vector direction = vector.Direction(neck, aimPosition).Normalized();
		EntityAI weaponInHands = unit.GetHumanInventory().GetEntityInHands();

		unit.LookAtPosition(aimPosition);
		unit.AimAtPosition(aimPosition);
		unit.BQBot_SetAimTarget(aimPosition);

		float threshold;
		if (weaponInHands && weaponInHands.IsWeapon())
		{
			Weapon_Base wb = Weapon_Base.Cast(weaponInHands);
			if (wb && wb.ShootsExplosiveAmmo())
				threshold = 0.96875;
			else
				threshold = 0.875;
		}
		else
		{
			threshold = 0.875;
		}

		float aimDot = vector.Dot(unit.GetAimDirection(), direction);
		if (aimDot < threshold)
		{
			BQExpansionDiag.Log("FW_UPDATE", "WAIT reason=aim_dot bot=" + unit.ToString() + " dot=" + aimDot.ToString() + " threshold=" + threshold.ToString());
			if (m_Time >= 0.5)
			{
				BQExpansionDiag.Log("FW_UPDATE", "EXIT reason=aim_timeout bot=" + unit.ToString());
				m_Time = 0;
				return EXIT;
			}

			return CONTINUE;
		}

		Print("[EAI] [fire_weapon] bot=" + unit.ToString() + " targetPos=" + m_Target.GetPosition().ToString() + " distSq=" + unit.m_eAI_DistanceToTargetSq.ToString());
		BQExpansionDiag.Log("FW_FIRE", "bot=" + unit.ToString() + " dot=" + aimDot.ToString() + " threshold=" + threshold.ToString() + " weapon=" + BQExpansionDiag.Obj(weaponInHands));
		unit.TryFireWeapon();

		return EXIT;
	}

	int Guard()
	{
		if (unit.IsRestrained())
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=restrained bot=" + unit.ToString());
			return eAITransition.FAIL;
		}

		m_Target = unit.GetTarget();
		if (!m_Target)
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=no_target bot=" + unit.ToString());
			return eAITransition.FAIL;
		}

		EntityAI targetEntity = m_Target.GetEntity();
		ItemBase itemTarget;
		if (!targetEntity)
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=no_target_entity bot=" + unit.ToString());
			return eAITransition.FAIL;
		}
		else if (Class.CastTo(itemTarget, targetEntity) && !unit.BQ_EAIItemIsDanger(itemTarget))
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=item_not_danger bot=" + unit.ToString() + " target=" + targetEntity.ToString());
			return eAITransition.FAIL;
		}

		if (unit.IsFighting())
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=is_fighting_melee bot=" + unit.ToString());
			return eAITransition.FAIL;
		}

		if (!Class.CastTo(m_Weapon, unit.GetItemInHands()))
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=no_weapon_in_hands bot=" + unit.ToString() + " hands=" + BQExpansionDiag.Obj(unit.GetItemInHands()));
			return eAITransition.FAIL;
		}

		if (m_Weapon.IsDamageDestroyed())
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=weapon_destroyed bot=" + unit.ToString() + " weapon=" + m_Weapon.ToString());
			return eAITransition.FAIL;
		}

		if (!unit.CanRaiseWeapon())
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=cannot_raise bot=" + unit.ToString());
			return eAITransition.FAIL;
		}

		if (!m_Target.m_LOS)
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=no_los bot=" + unit.ToString() + " target=" + targetEntity.ToString());
			return eAITransition.FAIL;
		}

		if (!unit.BQ_EAIWeaponIsChambered(m_Weapon))
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=not_chambered bot=" + unit.ToString() + " weapon=" + m_Weapon.ToString());
			return eAITransition.FAIL;
		}

		WeaponManager weaponManager = unit.GetWeaponManager();
		if (weaponManager && weaponManager.CanUnjam(m_Weapon))
		{
			BQExpansionDiag.Log("FW_GUARD", "FAIL reason=can_unjam bot=" + unit.ToString() + " weapon=" + m_Weapon.ToString());
			return eAITransition.FAIL;
		}

		float minDist;
		if (m_Weapon.ShootsExplosiveAmmo())
			minDist = unit.BQ_EAIGetMinSafeFiringDistance(m_Weapon);

		if (unit.BQ_EAIIsExplosiveTarget(m_Target))
			minDist = Math.Max(m_Target.GetMinDistance(), minDist);

		if (minDist > 0.0)
		{
			float dist = m_Target.GetDistance();
			if (dist < minDist)
			{
				BQExpansionDiag.Log("FW_GUARD", "FAIL reason=min_safe_dist bot=" + unit.ToString() + " dist=" + dist.ToString() + " min=" + minDist.ToString());
				return eAITransition.FAIL;
			}

			if (unit.BQ_EAIHasFriendlyInBlastRadius(m_Target, minDist))
			{
				if (!unit.IsRaised())
					unit.eAI_RaiseWeapon(true);
				BQExpansionDiag.Log("FW_GUARD", "FAIL reason=friendly_blast_radius bot=" + unit.ToString());
				return eAITransition.FAIL;
			}
		}

		string fwGuardOk = "SUCCESS bot=" + unit.ToString();
		fwGuardOk = fwGuardOk + " target=" + targetEntity.ToString();
		fwGuardOk = fwGuardOk + " weapon=" + m_Weapon.ToString();
		fwGuardOk = fwGuardOk + " dist=" + m_Target.GetDistance().ToString();
		fwGuardOk = fwGuardOk + " los=" + BQExpansionDiag.B(m_Target.m_LOS);
		BQExpansionDiag.Log("FW_GUARD", fwGuardOk);
		return eAITransition.SUCCESS;
	}
}
