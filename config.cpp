class CfgPatches
{
	class bqbots
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};

class CfgVehicles
{
	class SurvivorM_Boris;
	class bqBots_Boris : SurvivorM_Boris
	{
		scope = 2;
		displayName = "bqBots Boris";
	};
};

class CfgMods
{
	class bqbots
	{
		dir = "bqBots";
		name = "bqBots";
		author = "Codex";
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};

		class defs
		{
			class engineScriptModule
            {
                files[] = {"bqbots/scripts/1_Core"};
            };
			class gameScriptModule
			{
				value = "";
				files[] = {"bqbots/scripts/3_Game"};
			};

			class worldScriptModule
			{
				value = "";
				files[] = {"bqbots/scripts/4_World"};
			};

			class missionScriptModule
			{
				value = "";
				files[] = {"bqbots/scripts/5_Mission"};
			};
		};
	};
};
