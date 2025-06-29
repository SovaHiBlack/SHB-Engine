#include "stdafx.h"
#include "..\XR_3DA\resourcemanager.h"
#include "HUDmanager.h"
#include "PHdynamicdata.h"
#include "Physics.h"
#include "level.h"
#include "..\XR_3DA\x_ray.h"
#include "..\XR_3DA\igame_persistent.h"
#include "PhysicsGamePars.h"
#include "AISpace.h"
#include "StringTable.h"
#include "xrServer.h"

extern	pureFrame* g_pNetProcessor;

BOOL CLevel::net_Start_client(pcstr options)
{
	return FALSE;
}

bool	CLevel::net_start_client1( )
{
	pApp->LoadBegin( );
	// name_of_server
	string64					name_of_server = "";
//	strcpy						(name_of_server,*m_caClientOptions);
	if (strchr(*m_caClientOptions, '/'))
		strncpy(name_of_server, *m_caClientOptions, strchr(*m_caClientOptions, '/') - *m_caClientOptions);

	if (strchr(name_of_server, '/'))	*strchr(name_of_server, '/') = 0;

	// Startup client
	string256					temp;
	sprintf_s(temp, "%s %s",
			  CStringTable( ).translate("st_client_connecting_to").c_str( ), name_of_server);

	g_pGamePersistent->LoadTitle(temp);
	return true;
}

bool	CLevel::net_start_client2( )
{
	Server->create_direct_client( );

	connected_to_server = Connect2Server(*m_caClientOptions);

	return true;
}

bool	CLevel::net_start_client3( )
{
	if (connected_to_server)
	{
		pcstr					level_name = NULL;
		level_name = ai( ).get_alife( ) ? *name( ) : Server->level_name(Server->GetConnectOptions( )).c_str( );

		// Determine internal level-ID
		s32						level_id = pApp->Level_ID(level_name);
		if (level_id < 0)
		{
			Disconnect( );
			pApp->LoadEnd( );
			connected_to_server = FALSE;
			m_name = level_name;
			m_connect_server_err = xrServer::ErrNoLevel;
			return				false;
		}
		pApp->Level_Set(level_id);
		m_name = level_name;
		// Load level
		R_ASSERT2(Load(level_id), "Loading failed.");

	}
	return true;
}

bool	CLevel::net_start_client4( )
{
	if (connected_to_server)
	{
		// Begin spawn
		g_pGamePersistent->LoadTitle("st_client_spawning");

		// Send physics to single or multithreaded mode
		LoadPhysicsGameParams( );
		ph_world = xr_new<CPHWorld>( );
		ph_world->Create( );

		// Send network to single or multithreaded mode
		// *note: release version always has "mt_*" enabled
		Device.seqFrameMT.Remove(g_pNetProcessor);
		Device.seqFrame.Remove(g_pNetProcessor);
		Device.seqFrameMT.Add(g_pNetProcessor, REG_PRIORITY_HIGH + 2);

		while (!game_configured)
		{
			ClientReceive( );
			if (Server)
			{
				Server->Update( );
			}

			Sleep(5);
		}
	}

	return true;
}

bool	CLevel::net_start_client5( )
{
	if (connected_to_server)
	{
		// HUD
		// Textures
		pHUD->Load( );
		g_pGamePersistent->LoadTitle("st_loading_textures");
		Device.Resources->DeferredLoad(FALSE);
		Device.Resources->DeferredUpload( );
		LL_CheckTextures( );
	}

	return true;
}

bool CLevel::net_start_client6( )
{
	if (connected_to_server)
	{
		// Sync
		if (g_hud)
		{
			g_hud->OnConnected( );
		}

		g_pGamePersistent->LoadTitle("st_client_synchronising");
		Device.PreCache(30);
		net_start_result_total = TRUE;
	}
	else
	{
		net_start_result_total = FALSE;
	}

	pApp->LoadEnd( );
	return true;
}
