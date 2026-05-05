class bqCombatPriorityQueue<Class T>
{
	private ref array<ref Param2<ref T, float>> m_Elements = new array<ref Param2<ref T, float>>();

	int Count()
	{
		return m_Elements.Count();
	}

	void Enqueue(T item, float priority)
	{
		m_Elements.Insert(new Param2<ref T, float>(item, priority));
	}

	T Dequeue()
	{
		int bestIndex = 0;
		for (int i = 0; i < m_Elements.Count(); i++)
		{
			if (m_Elements[i].param2 < m_Elements[bestIndex].param2)
				bestIndex = i;
		}

		T bestItem = m_Elements[bestIndex].param1;
		m_Elements.Remove(bestIndex);
		return bestItem;
	}
}