#pragma once

#include <depr/dplay8.h>// раньше небыло
#include "net_utils.h"
#include "net_messages.h"


#ifdef XR_NETSERVER_EXPORTS
	#define XRNETSERVER_API __declspec(dllexport)
#else
	#define XRNETSERVER_API __declspec(dllimport)
	#pragma comment(lib,	"xrNetServer.lib"	)
#endif


XRNETSERVER_API extern ClientID BroadcastCID;
