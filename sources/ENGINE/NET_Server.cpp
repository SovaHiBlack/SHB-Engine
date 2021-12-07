#include "stdafx.h"

#include "NET_Server.h"

ENGINE_API CClientID BroadcastCID(0xffffffff);

void ip_address::set(const char* src_string)
{
	unsigned int buff[4];
	int cnt = sscanf(src_string, "%d.%d.%d.%d", &buff[0], &buff[1], &buff[2], &buff[3]);
	if (cnt == 4)
	{
		m_data.a1 = unsigned char(buff[0] & 0xff);
		m_data.a2 = unsigned char(buff[1] & 0xff);
		m_data.a3 = unsigned char(buff[2] & 0xff);
		m_data.a4 = unsigned char(buff[3] & 0xff);
	}
	else
	{
		Msg("! Bad ipAddress format [%s]", src_string);
		m_data.data = 0;
	}
}

xr_string ip_address::to_string( ) const
{
	string128 res;
	sprintf_s(res, sizeof(res), "%d.%d.%d.%d", m_data.a1, m_data.a2, m_data.a3, m_data.a4);
	return res;
}

IClient::IClient(CTimer* timer) : server(nullptr)
{
	dwTime_LastUpdate = 0;
	flags.bLocal = FALSE;
	flags.bConnected = FALSE;
	flags.bReconnect = FALSE;
	flags.bVerified = TRUE;
}

IClient::~IClient( )
{ }

IClient* IPureServer::ID_to_client(CClientID ID, bool ScanAll)
{
	if (0 == ID.value( ))
	{
		return nullptr;
	}

	csPlayers.Enter( );

	for (unsigned int client = 0; client < net_Players.size( ); ++client)
	{
		if (net_Players[client]->ID == ID)
		{
			csPlayers.Leave( );
			return net_Players[client];
		}
	}

	if (ScanAll)
	{
		for (unsigned int client = 0; client < net_Players_disconnected.size( ); ++client)
		{
			if (net_Players_disconnected[client]->ID == ID)
			{
				csPlayers.Leave( );
				return net_Players_disconnected[client];
			}
		}
	}

	csPlayers.Leave( );

	return nullptr;
}

IPureServer::IPureServer(CTimer* timer)
{
	device_timer = timer;
	SV_Client = nullptr;
}

IPureServer::~IPureServer( )
{
	SV_Client = nullptr;
}

IPureServer::EConnect IPureServer::Connect(const char* options)
{
	connect_options = options;
	return ErrNoError;
}

void IPureServer::Disconnect( )
{ }

void IPureServer::SendTo_LL(CClientID ID, void* data, unsigned int size, unsigned int dwFlags, unsigned int dwTimeout)
{
	FATAL("");
}

void IPureServer::SendTo(CClientID ID, CNetPacket& P, unsigned int dwFlags, unsigned int dwTimeout)
{
	SendTo_LL(ID, P.B.data, P.B.count, dwFlags, dwTimeout);
}

void IPureServer::SendBroadcast_LL(CClientID exclude, void* data, unsigned int size, unsigned int dwFlags)
{
	csPlayers.Enter( );

	for (unsigned int i = 0; i < net_Players.size( ); i++)
	{
		IClient* player = net_Players[i];

		if (player->ID == exclude)
		{
			continue;
		}

		if (!player->flags.bConnected)
		{
			continue;
		}

		SendTo_LL(player->ID, data, size, dwFlags);
	}

	csPlayers.Leave( );
}

void IPureServer::SendBroadcast(CClientID exclude, CNetPacket& P, unsigned int dwFlags)
{
	// Perform broadcasting
	SendBroadcast_LL(exclude, P.B.data, P.B.count, dwFlags);
}

unsigned int IPureServer::OnMessage(CNetPacket& P, CClientID sender)	// Non-Zero means broadcasting with "flags" as returned
{
	return 0;
}
