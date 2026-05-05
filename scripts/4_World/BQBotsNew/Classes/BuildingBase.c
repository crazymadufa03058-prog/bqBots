class ExpansionLadder
{
	string m_Name;
	int m_Index;
	string m_Type;
	int m_ConCount;
	int m_ConDirCount;
	vector m_Con[2];
	vector m_ConDir[2];

	void ExpansionLadder(string name, int index, string type)
	{
		m_Name = name;
		m_Index = index;
		m_Type = type;
	}

	static void InsertVertex(vector vertex, inout vector target[2], inout int count)
	{
		if (count == 0)
		{
			target[count++] = vertex;
		}
		else
		{
			vector v0 = target[0];
			if (vertex[1] < v0[1])
			{
				target[0] = vertex;
				target[1] = v0;
			}
			else
			{
				target[1] = vertex;
			}
		}
	}
}
/**
 * BuildingBase.c
 *
 * DayZ Expansion Mod
 * www.dayzexpansion.com
 * В© 2022 DayZ Expansion Mod Team
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-nd/4.0/.
 *
*/

modded class BuildingBase
{
		ref ExpansionNetsyncData m_Expansion_NetsyncData;
	static ref TStringArray s_eAI_PreventClimb;
	static ref map<string, ref map<int, ref ExpansionLadder>> s_Expansion_BuildingsWithLadders = new map<string, ref map<int, ref ExpansionLadder>>;

	ref map<int, int> m_eAI_LastDoorInteractionTime = new map<int, int>;
	ref map<int, ref eAIDoorTargetInformation> m_eAI_DoorTargetInformation;
	bool m_eAI_PreventClimb;
	ref map<int, float> m_eAI_DoorAnimationTime;
	ref map<int, ref ExpansionLadder> m_Expansion_Ladders;
	int m_Expansion_LaddersCount = -1;


	override bool NameOverride(out string output)
	{
		if (m_Expansion_NetsyncData && m_Expansion_NetsyncData.Get(0, output))
			return true;
		else
			return super.NameOverride(output);
	}


		void eAI_InitPreventClimbDefaults()
	{
		if (s_eAI_PreventClimb)
			return;
		s_eAI_PreventClimb = {
			"land_boat_", "land_busstop", "land_camp_house", "land_city_firestation", "land_container",
			"land_garage", "land_guardhouse", "land_house", "land_lighthouse", "land_misc_polytunnel",
			"land_misc_greenhouse", "land_mil_airfield_hq", "land_mil_atc", "land_mil_barracks",
			"land_mil_guardbox", "land_mil_guardhouse", "land_mil_guardshed", "land_mil_tent",
			"land_office1", "land_office2", "land_rail_station", "land_shed", "land_slum_house2",
			"land_slum_house6", "land_tenement", "land_train_wagon_box", "land_village_healthcare",
			"land_village_store", "land_wall_fenforest_gate", "land_wall_gate_feng$", "land_wall_gate_feng_open$",
			"land_wall_gate_fenr$", "land_wall_gate_fenr_open$", "land_wall_gate_opendam", "land_wall_gate_wood",
			"land_water_station", "land_workshop", "sign"
		};
		s_eAI_PreventClimb.InsertAll(GetExpansionSettings().GetAI().PreventClimb);
	}

	override void DeferredInit()
	{
		super.DeferredInit();
		eAI_InitPreventClimbDefaults();
		string type = GetType();
		type.ToLower();
		int len = type.Length();
		foreach (string preventClimb: s_eAI_PreventClimb)
		{
			int lastChar = preventClimb.Length() - 1;
			if (lastChar < 0)
				continue;
			if (preventClimb[lastChar] == "$")
			{
				preventClimb = preventClimb.Substring(0, lastChar);
				int index = preventClimb.IndexOf(type);
				if (index > -1 && index == preventClimb.Length() - len)
				{
					m_eAI_PreventClimb = true;
					break;
				}
			}
			else if (type.IndexOf(preventClimb) >= 0)
			{
				m_eAI_PreventClimb = true;
				break;
			}
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
	}
	eAIDoorTargetInformation eAI_GetDoorTargetInformation(int doorIndex, vector position)
	{
		if (!m_eAI_DoorTargetInformation)
			m_eAI_DoorTargetInformation = new map<int, ref eAIDoorTargetInformation>;

		eAIDoorTargetInformation info;
		if (!m_eAI_DoorTargetInformation.Find(doorIndex, info))
		{
			info = new eAIDoorTargetInformation(this, doorIndex, position);
			m_eAI_DoorTargetInformation[doorIndex] = info;
		}

		return info;
	}

	float eAI_GetDoorAnimationTime(int doorIndex)
	{
		float time;
		if (!m_eAI_DoorAnimationTime)
			m_eAI_DoorAnimationTime = new map<int, float>;

		if (!m_eAI_DoorAnimationTime.Find(doorIndex, time))
		{
			string path = CFG_VEHICLESPATH + " " + GetType() + " Doors";
			int count = g_Game.ConfigGetChildrenCount(path);
			if (doorIndex < count)
			{
				string name;
				g_Game.ConfigGetChildName(path, doorIndex, name);
				time = g_Game.ConfigGetFloat(path + " " + name + " animPeriod");
			}
			time = Math.Max(time, 1.0);
			m_eAI_DoorAnimationTime[doorIndex] = time;
		}

		return time;
	}

	vector eAI_GetRoamingPosition(vector position)
	{
		vector destinationPosition = GetPosition();
		vector minMax[2];
		if (!Expansion_IsEnterable() && GetCollisionBox(minMax))
		{
			minMax[0][1] = 0.0;
			minMax[1][1] = 0.0;
			float extension = vector.Distance(minMax[0], minMax[1]) * 0.5 + 1.0;
			position[1] = destinationPosition[1];
			vector dir = vector.Direction(position, destinationPosition);
			destinationPosition = destinationPosition - dir.Normalized() * extension;
		}
		return destinationPosition;
	}

	bool Expansion_IsEnterable()
	{
		if (GetDoorCount() == 0)
			return false;

		string type = GetType();
		type.ToLower();
		if (type.IndexOf("land_wreck_") == 0)
		{
			if (type.IndexOf("land_wreck_caravan") == 0)
				return true;
			if (type.IndexOf("land_wreck_v3s") == 0)
				return true;
			return false;
		}

		return true;
	}

	bool Expansion_IsWreck()
	{
		string type = GetType();
		type.ToLower();
		return type.IndexOf("land_wreck_") == 0;
	}

	bool Expansion_IsUndergroundEntrance()
	{
		return false;
	}

	int Expansion_GetLaddersCount()
	{
		if (m_Expansion_LaddersCount != -1)
			return m_Expansion_LaddersCount;

		string type = GetType();
		if (s_Expansion_BuildingsWithLadders.Find(type, m_Expansion_Ladders))
		{
			m_Expansion_LaddersCount = m_Expansion_Ladders.Count();
			return m_Expansion_LaddersCount;
		}

		m_Expansion_Ladders = new map<int, ref ExpansionLadder>;
		s_Expansion_BuildingsWithLadders[type] = m_Expansion_Ladders;
		m_Expansion_LaddersCount = 0;

		LOD memory = GetLODByName(LOD.NAME_MEMORY);
		if (!memory)
			return m_Expansion_LaddersCount;

		array<Selection> selections = {};
		if (!memory.GetSelections(selections))
			return m_Expansion_LaddersCount;

		LOD geometry = GetLODByName(LOD.NAME_GEOMETRY);
		string ladderType = "metal";
		if (geometry)
		{
			for (int i = 0; i < geometry.GetPropertyCount(); i++)
			{
				if (geometry.GetPropertyName(i) == "laddertype")
				{
					ladderType = geometry.GetPropertyValue(i);
					break;
				}
			}
		}

		ExpansionLadder currentLadder;
		foreach (Selection selection: selections)
		{
			string name = selection.GetName();
			if (name.IndexOf("ladder") != 0)
				continue;

			string ladderName;
			int ladderIndex = -1;
			int underscoreIndex = name.IndexOf("_");
			if (underscoreIndex > 6)
			{
				ladderName = name.Substring(0, underscoreIndex);
				ladderIndex = name.Substring(6, underscoreIndex - 6).ToInt();
			}
			else if (name.Length() > 6)
			{
				ladderName = name;
				ladderIndex = name.Substring(6, name.Length() - 6).ToInt();
			}

			if (ladderIndex <= -1)
				continue;

			if (!m_Expansion_Ladders.Find(ladderIndex, currentLadder))
			{
				currentLadder = new ExpansionLadder(ladderName, ladderIndex, ladderType);
				m_Expansion_Ladders[ladderIndex] = currentLadder;
			}

			int vertexCount = selection.GetVertexCount();
			for (int j = 0; j < vertexCount; j++)
			{
				int vertexIndex = selection.GetLODVertexIndex(j);
				if (vertexIndex <= -1)
					continue;

				vector vertex = memory.GetVertexPosition(vertexIndex);
				switch (name)
				{
					case ladderName + "_con":
						currentLadder.InsertVertex(vertex, currentLadder.m_Con, currentLadder.m_ConCount);
						break;
					case ladderName + "_con_dir":
						currentLadder.InsertVertex(vertex, currentLadder.m_ConDir, currentLadder.m_ConDirCount);
						break;
				}
			}
		}

		m_Expansion_LaddersCount = m_Expansion_Ladders.Count();
		return m_Expansion_LaddersCount;
	}
}
