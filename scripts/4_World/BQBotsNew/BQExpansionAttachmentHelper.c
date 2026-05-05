class bqCombatAttachmentHelper
{
	static Object FindBestPathParent(Object owner, array<ref RaycastRVResult> results)
	{
		Object best;
		Object obj;
		IEntity parent;
		if (owner)
			parent = owner.GetParent();

		foreach (RaycastRVResult result : results)
		{
			obj = result.parent;
			if (!obj)
				obj = result.obj;
			if (!obj || obj == owner)
				continue;

			if (!bqCombatNavMeshes.Get(obj))
				continue;

			if (obj == parent)
				return obj;

			best = obj;
		}

		return best;
	}

	static Object IsAttachment(Object owner, vector position, float radius)
	{
		vector start = position;
		vector end = position - Vector(0, radius, 0);

		RaycastRVParams params = new RaycastRVParams(start, end, owner, radius);
		params.sorted = true;
		params.type = ObjIntersectGeom;
		params.flags = CollisionFlags.ALLOBJECTS;

		array<ref RaycastRVResult> results = new array<ref RaycastRVResult>();
		if (DayZPhysics.RaycastRVProxy(params, results))
			return FindBestPathParent(owner, results);

		return null;
	}
}