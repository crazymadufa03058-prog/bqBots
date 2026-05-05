modded class PlayerBase
{
	static ref CF_DoublyLinkedNodes_WeakRef<PlayerBase> s_Expansion_AllPlayers = new CF_DoublyLinkedNodes_WeakRef<PlayerBase>();
	ref CF_DoublyLinkedNode_WeakRef<PlayerBase> m_Expansion_Node;
	ref ExpansionRPCManager m_Expansion_RPCManager = new ExpansionRPCManager();


	
	void Expansion_Init()
	{
	}

	bool Expansion_IsAI()
	{
		return false;
	}

	void Expansion_OnDangerousAreaEnterServer(EffectArea area, Trigger trigger)
	{
	}

	void Expansion_OnDangerousAreaExitServer(EffectArea area, Trigger trigger)
	{
	}

	vector Expansion_GetActualVelocity()
	{
		return "0 0 0";
	}


	float Expansion_GetHeadingAngle()
	{
		vector ori = GetOrientation();
		return ori[0];
	}

	vector Expansion_GetHeadingVector()
	{
		return GetDirection();
	}
	override float Expansion_GetMovementSpeed()
	{
		HumanCommandMove hcm = GetCommand_Move();
		if (hcm)
			return hcm.GetCurrentMovementSpeed();

		return 0.0;
	}

	float Expansion_GetMovementAngle()
	{
		return 0.0;
	}

	float Expansion_GetVisibility(float distance)
	{
		return 1.0;
	}

	float Expansion_GetVisibilityDistThreshold()
	{
		return 1.0;
	}
	bool Expansion_IsInSafeZone()
	{
		return false;
	}
}

modded class CrashBase
{
	static ref CF_DoublyLinkedNodes_WeakRef<CrashBase> s_Expansion_HeliCrashes = new CF_DoublyLinkedNodes_WeakRef<CrashBase>();
}

class ExpansionVehicle
{
	static ref CF_DoublyLinkedNodes_WeakRef<ExpansionVehicle> s_All = new CF_DoublyLinkedNodes_WeakRef<ExpansionVehicle>();
	EntityAI m_Entity;

	static bool Get(out ExpansionVehicle vehicle, Object obj)
	{
		vehicle = null;
		return false;
	}

	EntityAI GetEntity()
	{
		return m_Entity;
	}

	bool EngineIsOn()
	{
		return false;
	}

	bool IsHelicopter()
	{
		return false;
	}
}

class ExpansionDebugObject: Building
{
}
class ExpansionSoundSet
{
	static ExpansionSoundSet Register(string name)
	{
		return new ExpansionSoundSet();
	}

	void Play(Object target)
	{
	}
}

class ExpansionWorld: ExpansionGame
{
	static float AI_UPDATE_INTERVAL = 0.05;
	static ref ExpansionSoundSet s_eAI_UahUahUahM_SoundSet = ExpansionSoundSet.Register("Expansion_AI_UahUahUahM_SoundSet");
	static ref ExpansionSoundSet s_eAI_Heavy_Punch_SoundSet = ExpansionSoundSet.Register("Expansion_AI_Heavy_Punch_SoundSet");
	static int MAX_AI_UPDATE_BATCH = 20;

	CF_DoublyLinkedNode_WeakRef<eAIBase> m_CurrentAliveAI;
	int m_LastAIUpdateTime;

	void DbgAIUpdate()
	{
	}

	void eAI_Register(eAIBase ai)
	{
	}

	override void eAI_OnUpdate(bool doSim, float timeslice)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (eAIBase.s_eAI_FTO <= 0)
			return;

		int aiCount = eAIBase.s_eAI_Alive.m_Count;
		if (aiCount <= 0)
			return;

		int updateTime = GetGame().GetTime();
		if (m_LastAIUpdateTime == 0)
			m_LastAIUpdateTime = updateTime;

		float thresh = AI_UPDATE_INTERVAL / aiCount;
		float elapsed = (updateTime - m_LastAIUpdateTime) * 0.001;
		if (elapsed < thresh)
			return;

		m_LastAIUpdateTime = updateTime;

		int simulationPrecision = eAIBase.s_eAI_FTO;
		int maxBatch = Math.Min(aiCount, MAX_AI_UPDATE_BATCH);
		int updated = 0;
		int visited = 0;

		while (visited < aiCount && updated < maxBatch)
		{
			if (!m_CurrentAliveAI)
			{
				m_CurrentAliveAI = eAIBase.s_eAI_Alive.m_Head;
				if (!m_CurrentAliveAI)
					break;
			}

			eAIBase ai = m_CurrentAliveAI.m_Value;
			m_CurrentAliveAI = m_CurrentAliveAI.m_Next;
			visited++;

			if (!ai)
				continue;

			if (ai.m_eAI_LastUpdateTime == 0)
				ai.m_eAI_LastUpdateTime = updateTime;

			float pDt = (updateTime - ai.m_eAI_LastUpdateTime) * 0.001;
			if (pDt < AI_UPDATE_INTERVAL)
				continue;

			ai.eAI_SchedulerTick(pDt, simulationPrecision);

			updateTime = GetGame().GetTime();
			ai.m_eAI_LastUpdateTime = updateTime;
			updated++;
		}
	}
}

class ExpansionPrefab
{
	string ClassName;
	vector Position;

	bool CanSpawn()
	{
		return false;
	}

	Object Spawn(vector position, vector orientation)
	{
		return null;
	}
}

class ExpansionEntityStorageModule
{
	static bool SaveToFile(EntityAI entity, string fileName)
	{
		return false;
	}

	static bool RestoreFromFile(string fileName, out EntityAI entity, Object parent = null, Object root = null, bool deleteFile = false)
	{
		entity = null;
		return false;
	}
}

class ExpansionEffectAreaMergedCluster
{
	static const float IS_INSIDE_MARGIN = 1.0;
	ref array<ref ExpansionEffectAreaMergedCluster> m_Areas = new array<ref ExpansionEffectAreaMergedCluster>;
	vector m_Position;
	float m_Radius;
}

enum ExpansionNotificationType
{
	ACTIVITY
}

class ExpansionNotification
{
	void ExpansionNotification(string title = "", string text = "", string icon = "", int color = 0, float time = 0.0, ExpansionNotificationType type = ExpansionNotificationType.ACTIVITY)
	{
	}

	void Create(PlayerIdentity identity = null)
	{
	}

	void Info()
	{
	}

	void Error()
	{
		Print("[EAI][Notification]");
	}
}
class ExpansionHumanLoadout
{
	static bool Apply(PlayerBase player, string loadoutName)
	{
		return false;
	}
}

class ExpansionInventoryTools
{
	static bool IsTidyInProgress(Clothing clothing)
	{
		return false;
	}

	static bool Tidy(Clothing clothing)
	{
		return false;
	}
}

class ExpansionItemBaseModule
{
	static void SetLootable(EntityAI item, bool state)
	{
	}
}

class ExpansionItemSpawnHelper
{
	static EntityAI Clone(EntityAI src, bool useInventoryLocation, InventoryLocation location, bool keepUnlootable = false)
	{
		return src;
	}
}
modded class EffectArea
{
	ref ExpansionEffectAreaMergedCluster m_Expansion_MergedCluster;
}

modded class EmoteManager
{
	int Expansion_GetCurrentGesture()
	{
		return m_CurrentGestureID;
	}
}
