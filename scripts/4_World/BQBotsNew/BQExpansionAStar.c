class bqCombatAStar<Class NodeType>
{
	static float Heuristic(vector a, vector b)
	{
		return Math.AbsFloat(a[0] - b[0]) + Math.AbsFloat(a[1] - b[1]) + Math.AbsFloat(a[2] - b[2]);
	}

	static void Perform(NodeType start, NodeType goal, PGFilter filter, inout array<NodeType> path)
	{
		if (!start || !goal)
			return;

		bqCombatPriorityQueue<NodeType> queue = new bqCombatPriorityQueue<NodeType>();
		queue.Enqueue(start, 0.0);

		map<NodeType, NodeType> mappedPath();
		map<NodeType, float> cost();

		int includeFlags = filter.GetIncludeFlags();
		int excludeFlags = filter.GetExcludeFlags();

		NodeType current = null;
		NodeType next = null;

		mappedPath[start] = null;
		cost[start] = 0.0;

		while (queue.Count() > 0)
		{
			current = queue.Dequeue();
			if (current == goal)
				break;

			foreach (auto neighbour : current.m_Neighbours)
			{
				Class.CastTo(next, neighbour);
				if (!next)
					continue;

				if ((includeFlags & next.m_Flags) == 0)
					continue;

				if ((excludeFlags & next.m_Flags) != 0)
					continue;

				float newCost = cost[current] + vector.DistanceSq(goal.m_Position, next.m_Position);
				if ((!cost.Contains(next)) || newCost < cost[next])
				{
					cost[next] = newCost;
					float priority = newCost + Heuristic(next.m_Position, goal.m_Position);
					queue.Enqueue(next, priority);
					mappedPath[next] = current;
				}
			}
		}

		while (current)
		{
			path.Insert(current);
			current = mappedPath[current];
		}

		if (path.Count() <= 1)
			path.Clear();
	}
}