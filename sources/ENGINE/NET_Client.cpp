#include "stdafx.h"

#include "NET_Client.h"

INetQueue::INetQueue( )
{
	unused.reserve(128);
	for (int i = 0; i < 16; i++)
	{
		unused.push_back(xr_new<CNetPacket>( ));
	}
}

INetQueue::~INetQueue( )
{
	cs.Enter( );

	unsigned int it;
	for (it = 0; it < unused.size( ); it++)
	{
		xr_delete(unused[it]);
	}

	for (it = 0; it < ready.size( ); it++)
	{
		xr_delete(ready[it]);
	}

	cs.Leave( );
}

static unsigned int LastTimeCreate = 0;

void INetQueue::CreateCommit(CNetPacket* P)
{
	cs.Enter( );

	ready.push_back(P);

	cs.Leave( );
}

CNetPacket* INetQueue::CreateGet( )
{
	CNetPacket* P = nullptr;
	cs.Enter( );

	if (unused.empty( ))
	{
		P = xr_new<CNetPacket>( );
		LastTimeCreate = GetTickCount( );
	}
	else
	{
		P = unused.back( );
		unused.pop_back( );
	}

	cs.Leave( );
	return P;
}

CNetPacket* INetQueue::Retreive( )
{
	CNetPacket* P = nullptr;
	cs.Enter( );

	if (!ready.empty( ))
	{
		P = ready.front( );
	}
	else
	{
		unsigned int tmp_time = GetTickCount( ) - 60000;
		unsigned int size = unused.size( );
		if ((LastTimeCreate < tmp_time) && (size > 32))
		{
			xr_delete(unused.back( ));
			unused.pop_back( );
		}
	}

	cs.Leave( );
	return P;
}

void INetQueue::Release( )
{
	cs.Enter( );

	VERIFY(!ready.empty( ));

	unsigned int tmp_time = GetTickCount( ) - 60000;
	unsigned int size = unused.size( );
	if ((LastTimeCreate < tmp_time) && (size > 32))
	{
		xr_delete(ready.front( ));
	}
	else
	{
		unused.push_back(ready.front( ));
	}

	ready.pop_front( );

	cs.Leave( );
}

IPureClient::IPureClient(CTimer* timer)
{
	device_timer = timer;
	net_Syncronised = FALSE; //Добавил инициализацию сюда, тк она хз где была.
}

IPureClient::~IPureClient( )
{ }

BOOL IPureClient::Connect(const char* options)
{
	return TRUE;
}

void IPureClient::Disconnect( )
{
	net_Connected = EnmConnectionWait;
	net_Syncronised = FALSE;
}

void IPureClient::OnMessage(void* data, unsigned int size)
{
	// One of the messages - decompress it
	CNetPacket* P = net_Queue.CreateGet( );

	P->construct(data, size);
	P->timeReceive = timeServer_Async( );

	unsigned short tmp_type;
	P->r_begin(tmp_type);
	net_Queue.CreateCommit(P);
}

void IPureClient::Send(CNetPacket& packet, unsigned int dwFlags, unsigned int dwTimeout)
{
	FATAL("");
}
