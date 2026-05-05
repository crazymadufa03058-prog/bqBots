/**
 * ExpansionNetsyncData.c
 *
 * DayZ Expansion Mod
 * www.dayzexpansion.com
 * © 2022 DayZ Expansion Mod Team
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-nd/4.0/.
 *
*/

class ExpansionNetsyncData
{
	Object m_Object;
	ref TStringArray m_Data;
	ref ScriptInvoker SI_Receive;
	bool m_WasDataRequested;
	bool m_WasDataSent;

	void ExpansionNetsyncData(Object object)
	{
		m_Object = object;

		if (g_Game.IsClient())
		{
			SI_Receive = new ScriptInvoker();
		}
	}

	/**
	 * @brief Late client initialization for entities w/o netsynced vars (others will request netsync data in OnVariablesSynchronized)
	 * 
	 * @note if you take an item to hand, vanilla will create a temporary copy of the item on client only.
	 * We don't want to request netsync data for that item, since the RPC will go into nowhere on server.
	 * We can detect such an item by checking its network ID. If it's client only, net ID will be zero.
	 */


	void Set(int index, string value)
	{
		if (!m_Data)
			m_Data = new TStringArray;
		if (index < m_Data.Count())
			m_Data[index] = value;
		else
			m_Data.Insert(value);
	#ifdef DIAG_DEVELOPER
		EXTrace.Print(EXTrace.MISC, m_Object, "Set netsync data " + index + " '" + m_Data[index] + "'");
	#endif
	}

	bool Get(int index, out string output)
	{
		if (m_Data && index < m_Data.Count())
		{
			output = m_Data[index];
			return true;
		}

		return false;
	}



	//! Send netsync data to client
	void Send(PlayerIdentity recipient, ParamsReadContext ctx = null)
	{
		if (!m_Data || !m_Data.Count())
			return;

	#ifdef DIAG_DEVELOPER
		if (recipient)
			EXTrace.Print(EXTrace.MISC, m_Object, "Sending " + m_Data.Count() + " netsync data entries to " + recipient.GetId());
		else
			EXTrace.Print(EXTrace.MISC, m_Object, "Sending " + m_Data.Count() + " netsync data entries to players in object netbubble");
	#endif


		m_WasDataSent = true;
	}

	void Receive(PlayerIdentity sender, ParamsReadContext ctx)
	{
		int count;
		ctx.Read(count);

		string value;
		m_Data = new TStringArray;
		while (count)
		{
			ctx.Read(value);
		#ifdef DIAG_DEVELOPER
			EXTrace.Print(EXTrace.MISC, m_Object, "Received netsync data entry '" + value + "'");
		#endif
			m_Data.Insert(value);
			count--;
		}

		if (SI_Receive)
			SI_Receive.Invoke();

	#ifdef DIAG_DEVELOPER
		EXTrace.Print(EXTrace.MISC, m_Object, "Received " + m_Data.Count() + " netsync data entries");
	#endif
	}
}
