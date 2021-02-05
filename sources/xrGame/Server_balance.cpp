#include "stdafx.h"

#include "Server.h"

class xrClientData;
class CSE_Abstract;

xrClientData* CServer::SelectBestClientToMigrateTo	(CSE_Abstract* E, BOOL bForceAnother)
{
	return (xrClientData*)SV_Client;
}
