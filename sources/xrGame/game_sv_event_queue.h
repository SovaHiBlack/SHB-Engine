#pragma once

#include "..\ENGINE\NetPacket.h"

struct GameEvent
{
	U16			type;
	u32			time;
	CClientID	sender;
	CNetPacket	P;
};

class GameEventQueue
{
	xrCriticalSection		cs;
	xr_deque<GameEvent*>	ready;
	xr_vector<GameEvent*>	unused;

public:
	GameEventQueue();
	~GameEventQueue();

	GameEvent*			Create	();
	GameEvent*			Create	(CNetPacket& P, U16 type, u32 time, CClientID clientID);
	GameEvent*			Retreive();
	void				Release	();
};
