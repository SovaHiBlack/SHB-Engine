#pragma once

#include "NET_Shared.h"

struct SClientConnectData
{
	CClientID			clientID;
	string64			name;
	string64			pass;
	unsigned int					process_id;

	SClientConnectData	( )
	{
		name[0] = pass[0] = 0;
		process_id = 0;
	}
};

class IPureServer;

struct ENGINE_API ip_address
{
	union
	{
		struct
		{
			unsigned char					a1;
			unsigned char					a2;
			unsigned char					a3;
			unsigned char					a4;
		};
		unsigned int						data;
	}							m_data;

	void		set				(const char* src_string);
	xr_string	to_string		( ) const;

	bool		operator ==		(const ip_address& other) const
	{
		return (m_data.data == other.m_data.data) || 
				((m_data.a1 == other.m_data.a1) && 
				(m_data.a2 == other.m_data.a2) && 
				(m_data.a3 == other.m_data.a3) && 
				(m_data.a4 == 0));
	}
};

class ENGINE_API IClient
{
public:
	struct Flags
	{
		unsigned int						bLocal : 1;
		unsigned int						bConnected : 1;
		unsigned int						bReconnect : 1;
		unsigned int						bVerified : 1;
	};

					IClient		(CTimer* timer);
	virtual			~IClient	( );

	CClientID					ID;
	string128					m_guid;
	CSharedString					name;
	CSharedString					pass;

	Flags						flags;	// local/host/normal
	unsigned int							dwTime_LastUpdate;

	ip_address					m_cAddress;
	DWORD						m_dwPort;
	unsigned int							process_id;

	IPureServer*				server;
};


inline bool operator == (IClient const* pClient, CClientID const& ID)
{
	return pClient->ID == ID;
}

class CServerInfo;

class ENGINE_API IPureServer
{
public:
	enum EConnect
	{
		ErrConnect,
		ErrNoLevel,
		ErrMax,
		ErrNoError								= ErrMax
	};

protected:
	CSharedString									connect_options;

	xrCriticalSection							csPlayers;
	xr_vector<IClient*>							net_Players;
	xr_vector<IClient*>							net_Players_disconnected;
	IClient*									SV_Client;

	CTimer*										device_timer;

	IClient*				ID_to_client		(CClientID ID, bool ScanAll = false);

	virtual IClient*		new_client			(SClientConnectData* cl_data) = 0;

public:
							IPureServer			(CTimer* timer);
	virtual					~IPureServer		( );

	virtual EConnect		Connect				(const char* session_name);
	virtual void			Disconnect			( );

	// send
	virtual void			SendTo_LL			(CClientID ID, void* data, unsigned int size, unsigned int dwFlags = DPNSEND_GUARANTEED, unsigned int dwTimeout = 0);
	void					SendTo				(CClientID ID, CNetPacket& P, unsigned int dwFlags = DPNSEND_GUARANTEED, unsigned int dwTimeout = 0);
	void					SendBroadcast_LL	(CClientID exclude, void* data, unsigned int size, unsigned int dwFlags = DPNSEND_GUARANTEED);
	void					SendBroadcast		(CClientID exclude, CNetPacket& P, unsigned int dwFlags = DPNSEND_GUARANTEED);

	// extended functionality
	virtual unsigned int				OnMessage			(CNetPacket& P, CClientID sender);	// Non-Zero means broadcasting with "flags" as returned
	virtual bool			OnCL_QueryHost		( )
	{
		return true;
	}

	virtual IClient*		client_Create		( ) = 0;							// create client info
	virtual void			client_Replicate	( ) = 0;							// replicate current state to client
	virtual void			client_Destroy		(IClient* C) = 0;					// destroy client info
	inline unsigned int				client_Count		( )
	{
		return net_Players.size( );
	}
	inline IClient*			client_Get			(unsigned int num)
	{
		return net_Players[num];
	}

	inline int				GetPort				( )
	{
		return 0;
	}

	virtual bool			Check_ServerAccess	(IClient* CL, string512& reason)
	{
		return true;
	}

	virtual void			GetServerInfo		(CServerInfo* si)
	{ }
	IClient*				GetServerClient		( )
	{
		return SV_Client;
	}
	const CSharedString&		GetConnectOptions	( ) const
	{
		return connect_options;
	}
};
