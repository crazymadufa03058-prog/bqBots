class bqCombatPathFilters
{
	protected static autoptr bqCombatPathFilters s_Instance;

	autoptr PGFilter m_PathFilter;
	autoptr PGFilter m_PathFilter_NoJumpClimb;
	autoptr PGFilter m_PathFilter_NoClosedDoors;
	autoptr PGFilter m_PathFilter_NoClosedDoors_NoJumpClimb;
	autoptr PGFilter m_PathFilter_Swimming;
	autoptr PGFilter m_PathFilter_NoJumpClimb_Swimming;
	autoptr PGFilter m_PathFilter_NoClosedDoors_Swimming;
	autoptr PGFilter m_PathFilter_NoClosedDoors_NoJumpClimb_Swimming;
	autoptr PGFilter m_BlockFilter;
	autoptr PGFilter m_AllFilter;

	private void bqCombatPathFilters()
	{
		m_PathFilter = new PGFilter();
		m_PathFilter_NoJumpClimb = new PGFilter();
		m_PathFilter_NoClosedDoors = new PGFilter();
		m_PathFilter_NoClosedDoors_NoJumpClimb = new PGFilter();
		m_PathFilter_Swimming = new PGFilter();
		m_PathFilter_NoJumpClimb_Swimming = new PGFilter();
		m_PathFilter_NoClosedDoors_Swimming = new PGFilter();
		m_PathFilter_NoClosedDoors_NoJumpClimb_Swimming = new PGFilter();
		m_BlockFilter = new PGFilter();
		m_AllFilter = new PGFilter();

		// DISABLED = closed doors, DOOR = opened doors, INSIDE = building interior.
		// SPECIAL covers vaulting/climbing (JUMP_OVER + JUMP_DOWN + CLIMB).
		// Match Expansion: include UNREACHABLE so path glue can mark bad targets instead of forcing a straight wall-run.
		int include = PGPolyFlags.UNREACHABLE | PGPolyFlags.DISABLED | PGPolyFlags.WALK | PGPolyFlags.DOOR | PGPolyFlags.INSIDE | PGPolyFlags.LADDER | PGPolyFlags.SPECIAL;
		int exclude = PGPolyFlags.CRAWL | PGPolyFlags.CROUCH | PGPolyFlags.SWIM_SEA | PGPolyFlags.SWIM;
		int exclusive = PGPolyFlags.NONE;

		int includeNoJC = include & ~PGPolyFlags.SPECIAL;
		int includeNoCD = include & ~PGPolyFlags.DISABLED;
		int includeNoCD_NoJC = includeNoJC & ~PGPolyFlags.DISABLED;
		int excludeNoJC = exclude | PGPolyFlags.SPECIAL;
		int excludeNoCD = exclude | PGPolyFlags.DISABLED;
		int excludeNoCD_NoJC = excludeNoJC | PGPolyFlags.DISABLED;

		int swim = PGPolyFlags.SWIM_SEA | PGPolyFlags.SWIM;
		int includeSwim = include | swim;
		int includeNoJC_Swim = includeNoJC | swim;
		int includeNoCD_Swim = includeSwim & ~PGPolyFlags.DISABLED;
		int includeNoCD_NoJC_Swim = includeNoJC_Swim & ~PGPolyFlags.DISABLED;
		int excludeSwim = exclude & ~swim;
		int excludeNoJC_Swim = excludeNoJC & ~swim;
		int excludeNoCD_Swim = excludeSwim | PGPolyFlags.DISABLED;
		int excludeNoCD_NoJC_Swim = excludeNoJC_Swim | PGPolyFlags.DISABLED;

		SetFilterCost(m_PathFilter, m_PathFilter_NoJumpClimb);
		SetFilterCost(m_PathFilter_NoClosedDoors, m_PathFilter_NoClosedDoors_NoJumpClimb);
		SetFilterCost(m_PathFilter_Swimming, m_PathFilter_NoJumpClimb_Swimming);
		SetFilterCost(m_PathFilter_NoClosedDoors_Swimming, m_PathFilter_NoClosedDoors_NoJumpClimb_Swimming);

		m_PathFilter.SetFlags(include, exclude, exclusive);
		m_PathFilter_NoClosedDoors.SetFlags(includeNoCD, excludeNoCD, exclusive);
		m_PathFilter_NoJumpClimb.SetFlags(includeNoJC, excludeNoJC, exclusive);
		m_PathFilter_NoClosedDoors_NoJumpClimb.SetFlags(includeNoCD_NoJC, excludeNoCD_NoJC, exclusive);
		m_PathFilter_Swimming.SetFlags(includeSwim, excludeSwim, exclusive);
		m_PathFilter_NoJumpClimb_Swimming.SetFlags(includeNoJC_Swim, excludeNoJC_Swim, exclusive);
		m_PathFilter_NoClosedDoors_Swimming.SetFlags(includeNoCD_Swim, excludeNoCD_Swim, exclusive);
		m_PathFilter_NoClosedDoors_NoJumpClimb_Swimming.SetFlags(includeNoCD_NoJC_Swim, excludeNoCD_NoJC_Swim, exclusive);

		// Block filter: same as path filter but without doors
		m_BlockFilter.SetFlags(include & ~(PGPolyFlags.DOOR | PGPolyFlags.DISABLED), exclude | PGPolyFlags.DOOR | PGPolyFlags.DISABLED, exclusive);

		// All filter: used only for sampling
		m_AllFilter.SetFlags(PGPolyFlags.ALL & ~(PGPolyFlags.CRAWL | PGPolyFlags.CROUCH), PGPolyFlags.CRAWL | PGPolyFlags.CROUCH, PGPolyFlags.NONE);
	}

	private void SetFilterCost(PGFilter filter, PGFilter filterNoJC)
	{
		// === EXPANSION AI STYLE COSTS - PRIORITY TO ROADS ===
		
		// Special areas - ladder highest priority
		filter.SetCost(PGAreaType.LADDER, 1.0);
		
		// Movement modes - crawl/crouch high cost (not included anyway)
		filter.SetCost(PGAreaType.CRAWL, 10.0);
		filter.SetCost(PGAreaType.CROUCH, 10.0);
		
		// Vault/climb - medium cost to discourage but allow
		filter.SetCost(PGAreaType.FENCE_WALL, 5.0);  // Vault over fence
		filter.SetCost(PGAreaType.JUMP, 10.0);    // Climb/jump
		
		// Water areas - differentiate depth
		filter.SetCost(PGAreaType.WATER, 5.0);           // Shallow water
		filter.SetCost(PGAreaType.WATER_DEEP, 10.0);         // Deep water (swim)
		filter.SetCost(PGAreaType.WATER_SEA, 5.0);        // Sea shallow
		filter.SetCost(PGAreaType.WATER_SEA_DEEP, 10.0);    // Sea deep
		
		// Doors - closed with high priority, opened with extreme penalty
		filter.SetCost(PGAreaType.DOOR_CLOSED, 4.0);     // Closed door - prefer path around
		filter.SetCost(PGAreaType.DOOR_OPENED, 10000.0);   // Open door - extreme penalty
		
		// === ROADWAY PRIORITY - KEY FIX FOR "STRAIGHT LINE" ===
		// ROADWAY: 2.0 (LOWER than TERRAIN 4.0) = prefers roads!
		filter.SetCost(PGAreaType.ROADWAY, 4.0);   // Match Expansion: roads not preferred
		// TREE: 1.0 - same as Expansion, trees are best path
		filter.SetCost(PGAreaType.TREE, 1.0);     
		
		// Objects - medium cost
		filter.SetCost(PGAreaType.OBJECTS_NOFFCON, 5.0);
		filter.SetCost(PGAreaType.OBJECTS, 5.0);
		
		// Terrain/Building - base cost (ROADWAY is now cheaper!)
		filter.SetCost(PGAreaType.TERRAIN, 4.0);
		filter.SetCost(PGAreaType.BUILDING, 4.0);
		
		// Special road-building combo - lowest cost (building on road)
		filter.SetCost(PGAreaType.ROADWAY_BUILDING, 1.0);
		
		// === NO JUMP/CLIMB VERSION ===
		filterNoJC.SetCost(PGAreaType.LADDER, 1.0);
		filterNoJC.SetCost(PGAreaType.CRAWL, 10.0);
		filterNoJC.SetCost(PGAreaType.CROUCH, 10.0);
		filterNoJC.SetCost(PGAreaType.FENCE_WALL, 1000.0);  // DISALLOW vault
		filterNoJC.SetCost(PGAreaType.JUMP, 1000.0);     // DISALLOW climb
		filterNoJC.SetCost(PGAreaType.WATER, 5.0);
		filterNoJC.SetCost(PGAreaType.WATER_DEEP, 10.0);
		filterNoJC.SetCost(PGAreaType.WATER_SEA, 5.0);
		filterNoJC.SetCost(PGAreaType.WATER_SEA_DEEP, 10.0);
		filterNoJC.SetCost(PGAreaType.DOOR_CLOSED, 4.0);
		filterNoJC.SetCost(PGAreaType.DOOR_OPENED, 10000.0);
		
		// ROADWAY priority in no-jump-climb mode too
		filterNoJC.SetCost(PGAreaType.ROADWAY, 4.0);
		filterNoJC.SetCost(PGAreaType.TREE, 1.0);
		filterNoJC.SetCost(PGAreaType.OBJECTS_NOFFCON, 5.0);
		filterNoJC.SetCost(PGAreaType.OBJECTS, 5.0);
		filterNoJC.SetCost(PGAreaType.TERRAIN, 4.0);
		filterNoJC.SetCost(PGAreaType.BUILDING, 4.0);
		filterNoJC.SetCost(PGAreaType.ROADWAY_BUILDING, 1.0);
	}

	static bqCombatPathFilters GetInstance()
	{
		if (!s_Instance)
			s_Instance = new bqCombatPathFilters();

		return s_Instance;
	}
}
