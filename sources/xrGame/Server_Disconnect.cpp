#include "stdafx.h"

#include "Server.h"

void CServer::Disconnect( )
{
	IPureServer::Disconnect		( );
	SLS_Clear					( );
	xr_delete					(game);
}
