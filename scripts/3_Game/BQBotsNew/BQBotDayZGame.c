// bqBots: Expansion-style shortcuts on DayZGame so code written against
// Expansion AI patterns (GetGame().GetAIWorld(), etc.) works unchanged.
//
// Rationale: in vanilla DayZ GetAIWorld() lives on class World, reachable only
// via GetGame().GetWorld().GetAIWorld(). Expansion AI forks add the shortcut
// on CGame. We reproduce the same ergonomics here via modded DayZGame.
modded class DayZGame
{
	// Shortcut to the AI navmesh/pathgraph world.
	// Safe: delegates to GetWorld().GetAIWorld() under the hood.
	AIWorld BQBot_GetAIWorld()
	{
		World world = GetWorld();
		if (!world)
			return null;
		return world.GetAIWorld();
	}
}
