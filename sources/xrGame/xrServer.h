// xrServer.h: interface for the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\xrNetServer\net_server.h"
#include "game_sv_base.h"
#include "id_generator.h"

#ifdef DEBUG
//. #define SLOW_VERIFY_ENTITIES
#endif


class CSE_Abstract;

const u32	NET_Latency = 50;		// time in (ms)

// t-defs
typedef xr_map<u16, CSE_Abstract*>	xrS_entities;

class xrClientData : public IClient
{
public:
	CSE_Abstract* owner;
	BOOL					net_Ready;
	BOOL					net_Accepted;

	BOOL					net_PassUpdates;
	u32						net_LastMoveUpdateTime;

	game_PlayerState* ps;
	struct
	{
		u8						m_maxPingWarnings;
		u32						m_dwLastMaxPingWarningTime;
	}m_ping_warn;
	struct
	{
		BOOL					m_has_admin_rights;
		u32						m_dwLoginTime;
	}m_admin_rights;

	xrClientData( );
	virtual					~xrClientData( );
	virtual void			Clear( );
};


// main
struct	svs_respawn
{
	u32		timestamp;
	u16		phantom;
};
IC bool operator < (const svs_respawn& A, const svs_respawn& B)
{
	return A.timestamp < B.timestamp;
}

class xrServer : public IPureServer
{
private:
	xrS_entities				entities;
	xr_multiset<svs_respawn>	q_respawn;

	u16							m_iCurUpdatePacket;
	xr_vector<CNetPacket>		m_aUpdatePackets;

	struct DelayedPacket
	{
		CClientID		SenderID;
		CNetPacket		Packet;
		bool operator == (const DelayedPacket& other)
		{
			return SenderID == other.SenderID;
		}
	};

	xrCriticalSection			DelayedPackestCS;
	xr_deque<DelayedPacket>		m_aDelayedPackets;
	void						ProceedDelayedPackets( );
	void						AddDelayedPacket(CNetPacket& Packet, CClientID Sender);
	u32							OnDelayedMessage(CNetPacket& P, CClientID sender);			// Non-Zero means broadcasting with "flags" as returned

	void						SendUpdatesToAll( );

private:
	typedef
		CID_Generator<
		u32,		// time identifier type
		u8,			// compressed id type 
		u16,		// id type
		u8,			// block id type
		u16,		// chunk id type
		0,			// min value
		u16(-2),	// max value
		256,		// block size
		u16(-1)		// invalid id
		> id_generator_type;

private:
	id_generator_type		m_tID_Generator;

protected:
	void					Server_Client_Check(IClient* CL);
	void					PerformCheckClientsForMaxPing( );

public:
	game_sv_GameState* game;

	void					Export_game_type(IClient* CL);
	void					Perform_game_export( );
	BOOL					PerformRP(CSE_Abstract* E);
	void					PerformMigration(CSE_Abstract* E, xrClientData* from, xrClientData* to);

	IC void					clear_ids( )
	{
		m_tID_Generator = id_generator_type( );
	}
	IC u16					PerformIDgen(u16 ID)
	{
		return				(m_tID_Generator.tfGetID(ID));
	}
	IC void					FreeID(u16 ID, u32 time)
	{
		return				(m_tID_Generator.vfFreeID(ID, time));
	}

	void					Perform_connect_spawn(CSE_Abstract* E, xrClientData* to, CNetPacket& P);
	void					Perform_transfer(CNetPacket& PR, CNetPacket& PT, CSE_Abstract* what, CSE_Abstract* from, CSE_Abstract* to);
	void					Perform_reject(CSE_Abstract* what, CSE_Abstract* from, int delta);
	void					Perform_destroy(CSE_Abstract* tpSE_Abstract, u32 mode);

	CSE_Abstract* Process_spawn(CNetPacket& P, CClientID sender, BOOL bSpawnWithClientsMainEntityAsParent = FALSE, CSE_Abstract* tpExistedEntity = 0);
	void					Process_update(CNetPacket& P, CClientID sender);
	void					Process_save(CNetPacket& P, CClientID sender);
	void					Process_event(CNetPacket& P, CClientID sender);
	void					Process_event_ownership(CNetPacket& P, CClientID sender, u32 time, u16 ID, BOOL bForced = FALSE);
	bool					Process_event_reject(CNetPacket& P, const CClientID sender, const u32 time, const u16 id_parent, const u16 id_entity, bool send_message = true);
	void					Process_event_destroy(CNetPacket& P, CClientID sender, u32 time, u16 ID, CNetPacket* pEPack);

	xrClientData* SelectBestClientToMigrateTo(CSE_Abstract* E, BOOL bForceAnother = FALSE);
	void					SendConnectResult(IClient* CL, u8 res, u8 res1, pstr ResultStr);

	void					AttachNewClient(IClient* CL);
	virtual void			OnBuildVersionRespond(IClient* CL, CNetPacket& P);

protected:
	bool					CheckAdminRights(const shared_str& user, const shared_str& pass, string512 reason);
	virtual IClient* new_client(SClientConnectData* cl_data);

	virtual bool			Check_ServerAccess(IClient* CL, string512& reason)
	{
		return true;
	}

	virtual bool			NeedToCheckClient_GameSpy_CDKey(IClient* CL)
	{
		return false;
	}
	virtual void			Check_GameSpy_CDKey_Success(IClient* CL);

	virtual bool			NeedToCheckClient_BuildVersion(IClient* CL);
	virtual void			Check_BuildVersion_Success(IClient* CL);

	void					SendConnectionData(IClient* CL);
	void					OnChatMessage(CNetPacket* P, xrClientData* CL);

public:
	// constr / destr
	xrServer( );
	virtual ~xrServer( );

	// extended functionality
	virtual u32				OnMessage(CNetPacket& P, CClientID sender);	// Non-Zero means broadcasting with "flags" as returned
	virtual void			OnCL_Connected(IClient* CL);
	virtual void			OnCL_Disconnected(IClient* CL);
	virtual bool			OnCL_QueryHost( );
	virtual void			SendTo_LL(CClientID ID, void* data, u32 size, u32 dwFlags = DPNSEND_GUARANTEED, u32 dwTimeout = 0);

	virtual IClient* client_Create( );								// create client info
	virtual void			client_Replicate( );								// replicate current state to client
	virtual IClient* client_Find_Get(CClientID ID);					// Find earlier disconnected client
	virtual void			client_Destroy(IClient* C);					// destroy client info

	// utilities
	CSE_Abstract* entity_Create(pcstr name);
	void					entity_Destroy(CSE_Abstract*& P);
	u32						GetEntitiesNum( )
	{
		return entities.size( );
	}
	CSE_Abstract* GetEntity(u32 Num);

	IC void					clients_Lock( )
	{
		csPlayers.Enter( );
	}
	IC void					clients_Unlock( )
	{
		csPlayers.Leave( );
	}

	xrClientData* ID_to_client(CClientID ID, bool ScanAll = false)
	{
		return (xrClientData*)(IPureServer::ID_to_client(ID, ScanAll));
	}
	CSE_Abstract* ID_to_entity(u16 ID);

	// main
	virtual EConnect		Connect(shared_str& session_name);
	virtual void			Disconnect( );
	virtual void			Update( );
	void					SLS_Default( );
	void					SLS_Clear( );
	void					SLS_Save(IWriter& fs);
	void					SLS_Load(IReader& fs);
	shared_str		level_name(const shared_str& server_options) const;

	void					create_direct_client( );

	virtual void			Assign_ServerType(string512& res)
	{ }

	virtual bool			HasProtected( )
	{
		return false;
	}

	virtual void			GetServerInfo(CServerInfo* si);

public:
	xr_string				ent_name_safe(u16 eid);

#ifdef DEBUG
	bool			verify_entities( ) const;
	void			verify_entity(const CSE_Abstract* entity) const;
#endif

};
