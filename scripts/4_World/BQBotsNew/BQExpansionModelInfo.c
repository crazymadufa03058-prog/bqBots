class bqCombatModelInfo
{
	static ref map<string, ref bqCombatModelInfo> s_All = new map<string, ref bqCombatModelInfo>();

	static bqCombatModelInfo Get(Object object)
	{
		if (!object)
			return null;

		string modelName = object.ConfigGetString("model");
		if (modelName == string.Empty)
		{
			modelName = object.GetDebugNameNative();
			int index = modelName.IndexOf(":") + 2;
			int length = modelName.Length() - index;
			if (index < 0 || length < 3)
				return null;
			modelName = modelName.Substring(index, length);
		}

		bqCombatModelInfo info;
		if (!s_All.Find(modelName, info))
		{
			info = new bqCombatModelInfo(object);
			s_All.Insert(modelName, info);
		}

		return info;
	}

	static void Clear()
	{
		s_All.Clear();
	}

	bool CanClimb = true;
	ref bqCombatNavMesh NavMesh = null;
	ref array<LOD> LODs = new array<LOD>();

	void bqCombatModelInfo(Object object)
	{
		object.GetLODS(LODs);
		foreach (LOD lod : LODs)
		{
			if (!lod)
				continue;

			string lodName = lod.GetName(object);
			for (int property = 0; property < lod.GetPropertyCount(); property++)
			{
				string propertyName = lod.GetPropertyName(property);
				propertyName.ToLower();
				if (propertyName == "canclimb")
					CanClimb = lod.GetPropertyValue(property) == "1";
			}

			if (lodName == "paths")
			{
				NavMesh = new bqCombatNavMesh();
				NavMesh.Generate(object, lod);
				if (!NavMesh.IsValid())
					NavMesh = null;
			}
		}
	}
}