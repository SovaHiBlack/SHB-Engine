#pragma once

#include "NET_Shared.h"

struct ip_address;

class ENGINE_API INetQueue
{
	xrCriticalSection				cs;
	xr_deque<CNetPacket*>			ready;
	xr_vector<CNetPacket*>			unused;

public:
					INetQueue		( );
					~INetQueue		( );

	CNetPacket*		CreateGet		( );
	void			CreateCommit	(CNetPacket*);

	CNetPacket*		Retreive		( );
	void			Release			( );
};

class ENGINE_API IPureClient
{
	enum ConnectionState
	{
		EnmConnectionFails							= 0,
		EnmConnectionWait							= -1,
		EnmConnectionCompleted						= 1
	};

protected:
	CTimer*											device_timer;

	ConnectionState									net_Connected;
	INetQueue										net_Queue;

public:
								IPureClient			(CTimer* tm);
	virtual						~IPureClient		( );

	BOOL						Connect				(const char* server_name);
	void						Disconnect			( );


	BOOL						net_isFails_Connect	( )
	{
		return net_Connected == EnmConnectionFails;
	}
	const char*					net_SessionName		( )
	{
		return nullptr;
	}

	// receive
	inline virtual CNetPacket*	net_msg_Retreive	( )
	{
		return net_Queue.Retreive( );
	}
	inline void					net_msg_Release		( )
	{
		net_Queue.Release( );
	}

	// send
	virtual	void				Send				(CNetPacket& P, u32 dwFlags = DPNSEND_GUARANTEED, u32 dwTimeout = 0);
	virtual void				OnMessage			(void* data, u32 size);
	virtual void				OnConnectRejected	( )
	{ }

	// time management
	inline u32					timeServer			( )
	{
		return device_timer->GetElapsed_ms( );
	}
	inline u32					timeServer_Async	( )
	{
		return device_timer->GetElapsed_ms( );
	}

	BOOL											net_Syncronised;
	virtual BOOL				net_IsSyncronised	( )
	{
		return net_Syncronised;
	}
	virtual void				OnSessionTerminate	(const char* reason)
	{ }
};
