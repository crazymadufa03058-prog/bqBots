class bqCombatNavMeshPolygon: bqCombatPathNode
{
	static const int INDEX_COUNT = 4;
	static const int EDGE_COUNT = 8;
	static const int VERTEX_COUNT = 4;

	static float s_Distance[INDEX_COUNT];
	static float s_Delta[INDEX_COUNT];
	static vector s_Position0[INDEX_COUNT];
	static vector s_Position1[INDEX_COUNT];

	int m_Indices[INDEX_COUNT];
	int m_Edges[EDGE_COUNT];
	vector m_Vertices[VERTEX_COUNT];
	float m_Plane[4];

	void bqCombatNavMeshPolygon(Object object, LOD lod, Selection selection)
	{
		string name = selection.GetName();
		int count = selection.GetVertexCount();
		if (count != VERTEX_COUNT)
			Print("[EAI] [navmesh] expected 4 vertices in path selection " + name);

		TStringArray flags();
		object.ConfigGetTextArray("NavMesh p" + name + " flags", flags);
		m_Flags = bqCombatPolyFlags.ToFlag(flags);

		m_Indices[0] = selection.GetLODVertexIndex(0);
		m_Indices[1] = selection.GetLODVertexIndex(1);
		m_Indices[2] = selection.GetLODVertexIndex(2);
		m_Indices[3] = selection.GetLODVertexIndex(3);

		m_Vertices[0] = lod.GetVertexPosition(m_Indices[0]);
		m_Vertices[1] = lod.GetVertexPosition(m_Indices[1]);
		m_Vertices[2] = lod.GetVertexPosition(m_Indices[2]);
		m_Vertices[3] = lod.GetVertexPosition(m_Indices[3]);

		FirstPhase_SortVertices(0);
		SecondPhase_SwapVertices(0, 1, 2, true);
		SecondPhase_SwapVertices(2, 3, 2, false);

		int j = INDEX_COUNT - 1;
		int i = 0;
		while (i < INDEX_COUNT)
		{
			int eA = (i * 2) + 0;
			int eB = (i * 2) + 1;
			m_Edges[eA] = m_Indices[j];
			m_Edges[eB] = m_Indices[i];
			j = i++;
		}

		vector xyz = (m_Vertices[1] - m_Vertices[0]) * (m_Vertices[2] - m_Vertices[1]);
		m_Plane[0] = xyz[0];
		m_Plane[1] = xyz[1];
		m_Plane[2] = xyz[2];
		m_Plane[3] = -vector.Dot(xyz, m_Vertices[0]);

		vector p0 = vector.Lerp(m_Vertices[0], m_Vertices[1], 0.5);
		vector p1 = vector.Lerp(m_Vertices[2], m_Vertices[3], 0.5);
		m_Position = vector.Lerp(p0, p1, 0.5);

		float d01 = vector.Distance(m_Vertices[0], m_Vertices[1]);
		float d12 = vector.Distance(m_Vertices[1], m_Vertices[2]);
		float d02 = vector.Distance(m_Vertices[0], m_Vertices[2]);
		float d13 = vector.Distance(m_Vertices[1], m_Vertices[3]);
		m_Radius = Math.Max(Math.Max(d01, d12), Math.Max(d02, d13)) * 0.5;
	}

	vector SamplePosition(vector position)
	{
		if (Math.AbsFloat(m_Plane[1]) > 0.0001)
			position[1] = (-(m_Plane[0] * position[0]) - (m_Plane[2] * position[2]) - m_Plane[3]) / m_Plane[1];

		if (dtDistancePtPolyEdgesSqr(position))
			return position;

		float dmin = float.MAX;
		int imin = -1;
		for (int i = 0; i < VERTEX_COUNT; ++i)
		{
			if (s_Distance[i] < dmin)
			{
				dmin = s_Distance[i];
				imin = i;
			}
		}

		if (imin < 0)
			return position;

		return vector.Lerp(s_Position0[imin], s_Position1[imin], s_Delta[imin]);
	}

	bool ShouldBeConnecting(bqCombatNavMeshPolygon other)
	{
		if (!other || this == other || m_Neighbours.Find(other) != -1)
			return false;

		for (int i = 0; i < INDEX_COUNT; i++)
		{
			for (int j = 0; j < INDEX_COUNT; j++)
			{
				if (m_Vertices[i] != other.m_Vertices[j])
					continue;

				int iN = i + 1;
				int iP = i - 1;
				int jN = j + 1;
				int jP = j - 1;

				if (iN >= INDEX_COUNT) iN = 0;
				if (jN >= INDEX_COUNT) jN = 0;
				if (iP < 0) iP = INDEX_COUNT - 1;
				if (jP < 0) jP = INDEX_COUNT - 1;

				if (m_Vertices[iN] == other.m_Vertices[jN]) return true;
				if (m_Vertices[iP] == other.m_Vertices[jP]) return true;
				if (m_Vertices[iN] == other.m_Vertices[jP]) return true;
				if (m_Vertices[iP] == other.m_Vertices[jN]) return true;
			}
		}

		return false;
	}

	void FirstPhase_SortVertices(int index)
	{
		vector sortedVerts[4];
		int sortedIndices[4];
		int inserted = 0;

		for (int i = 0; i < VERTEX_COUNT; i++)
			sortedVerts[i] = Vector(float.MIN, float.MIN, float.MIN);

		for (i = 0; i < VERTEX_COUNT; i++)
		{
			vector vertex = m_Vertices[i];
			int pos = inserted;
			for (int j = 0; j < VERTEX_COUNT; j++)
			{
				vector otherVertex = sortedVerts[j];
				if (vertex[index] >= otherVertex[index])
				{
					pos = j;
					break;
				}
			}

			for (j = VERTEX_COUNT - 1; j >= pos; j--)
			{
				if (j > 0)
				{
					sortedVerts[j] = sortedVerts[j - 1];
					sortedIndices[j] = sortedIndices[j - 1];
				}
			}

			sortedVerts[pos] = vertex;
			sortedIndices[pos] = m_Indices[i];
			inserted++;
		}

		for (i = 0; i < VERTEX_COUNT; i++)
		{
			j = VERTEX_COUNT - (i + 1);
			m_Vertices[j] = sortedVerts[i];
			m_Indices[j] = sortedIndices[i];
		}
	}

	void SecondPhase_SwapVertices(int a, int b, int index, bool higher)
	{
		vector va = m_Vertices[a];
		vector vb = m_Vertices[b];
		if ((higher && va[index] > vb[index]) || (!higher && va[index] < vb[index]))
		{
			m_Vertices[a] = vb;
			m_Vertices[b] = va;
			int t = m_Indices[a];
			m_Indices[a] = m_Indices[b];
			m_Indices[b] = t;
		}
	}

	void dtDistancePtSegSqr2D(vector pt, vector p, vector q, int index)
	{
		float pqx = q[0] - p[0];
		float pqz = q[2] - p[2];
		float dx = pt[0] - p[0];
		float dz = pt[2] - p[2];
		float d = pqx * pqx + pqz * pqz;
		float t = pqx * dx + pqz * dz;
		if (d > 0.0)
			t = t / d;
		t = Math.Clamp(t, 0.0, 1.0);
		dx = p[0] + t * pqx - pt[0];
		dz = p[2] + t * pqz - pt[2];
		s_Distance[index] = dx * dx + dz * dz;
		s_Delta[index] = t;
		s_Position0[index] = p;
		s_Position1[index] = q;
	}

	bool dtDistancePtPolyEdgesSqr(vector pt)
	{
		int i = 0;
		int j = VERTEX_COUNT - 1;
		bool c = false;

		while (i < VERTEX_COUNT)
		{
			vector vi = m_Vertices[i];
			vector vj = m_Vertices[j];
			if (((vi[2] > pt[2]) != (vj[2] > pt[2])) && (pt[0] < (vj[0] - vi[0]) * (pt[2] - vi[2]) / (vj[2] - vi[2]) + vi[0]))
				c = !c;

			dtDistancePtSegSqr2D(pt, vj, vi, j);
			j = i++;
		}

		return c;
	}
}