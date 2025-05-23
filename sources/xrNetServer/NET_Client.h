#pragma once

#include "net_shared.h"

struct ip_address;

class XRNETSERVER_API INetQueue
{
	xrCriticalSection		cs;
	xr_deque<CNetPacket*>	ready;
	xr_vector<CNetPacket*>	unused;

public:
	INetQueue();
	~INetQueue();

	CNetPacket*			CreateGet	();
	void				CreateCommit(CNetPacket*);

	CNetPacket*			Retreive();
	void				Release	();
};

class XRNETSERVER_API IPureClient
{
	enum ConnectionState
	{
		EnmConnectionFails=0,
		EnmConnectionWait=-1,
		EnmConnectionCompleted=1
	};

protected:
	CTimer*					device_timer;

protected:
	ConnectionState			net_Connected;

	INetQueue				net_Queue;

public:
	IPureClient				(CTimer* tm);
	virtual ~IPureClient	();

	BOOL					Connect					(pcstr server_name);
	void					Disconnect				();


	BOOL					net_isFails_Connect		()	{ return net_Connected==EnmConnectionFails;}
	pcstr					net_SessionName			()	{ return nullptr; }

	// receive
	IC virtual	CNetPacket*	net_msg_Retreive		()	{ return net_Queue.Retreive();	}
	IC void					net_msg_Release			()	{ net_Queue.Release();			}

	// send
	virtual	void			Send					(CNetPacket& P, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	virtual void			OnMessage				(pvoid data, u32 size);
	virtual void			OnConnectRejected		()	{};

	// time management
	IC u32					timeServer				( )	{ return device_timer->GetElapsed_ms( ); }
	IC u32					timeServer_Async		( )	{ return device_timer->GetElapsed_ms( ); }

	BOOL					net_Syncronised;
	virtual	BOOL			net_IsSyncronised( )
	{
		return net_Syncronised;
	}
	virtual void			OnSessionTerminate		(pcstr reason){};
};
