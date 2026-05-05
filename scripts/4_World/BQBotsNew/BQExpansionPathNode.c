class bqCombatPathNode
{
	vector m_Position;
	float m_Radius;
	int m_Flags;
	ref set<bqCombatPathNode> m_Neighbours = new set<bqCombatPathNode>();

	int Count()
	{
		return m_Neighbours.Count();
	}

	bqCombatPathNode Get(int index)
	{
		return m_Neighbours[index];
	}

	int Find(bqCombatPathNode node)
	{
		return m_Neighbours.Find(node);
	}

	bool Contains(bqCombatPathNode node)
	{
		return m_Neighbours.Find(node) != -1;
	}

	void Add(bqCombatPathNode node)
	{
		if (node == this)
			return;

		m_Neighbours.Insert(node);
		node.m_Neighbours.Insert(this);
	}

	void Remove(bqCombatPathNode node)
	{
		if (node == this)
			return;

		int idx = m_Neighbours.Find(node);
		if (idx != -1)
			m_Neighbours.Remove(idx);

		idx = node.m_Neighbours.Find(this);
		if (idx != -1)
			node.m_Neighbours.Remove(idx);
	}
}