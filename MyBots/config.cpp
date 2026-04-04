class CfgPatches
{
	class MyBots
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Characters", "DZ_Characters_Heads"};
	};
};

class CfgMods
{
	class MyBots
	{
		dir = "MyBots";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "MyBots";
		author = "MyBots";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"MyBots/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"MyBots/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"MyBots/scripts/5_Mission"};
			};
		};
	};
};
