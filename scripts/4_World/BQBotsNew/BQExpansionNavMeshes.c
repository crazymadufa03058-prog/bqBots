class bqCombatNavMeshes
{
	static bqCombatNavMesh Get(Object object)
	{
		bqCombatModelInfo info = bqCombatModelInfo.Get(object);
		if (!info)
			return null;

		return info.NavMesh;
	}
}