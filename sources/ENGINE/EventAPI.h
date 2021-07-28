#pragma once

#include "..\Core\xrSyncronize.h"

class ENGINE_API CEvent;
using EVENT = CEvent*;

class ENGINE_API IEventReceiver
{
public:
	virtual void	OnEvent			(EVENT E, U64 P1, U64 P2) = 0;
};

class ENGINE_API CEventAPI
{
	struct Deferred
	{
		EVENT						E;
		U64							P1;
		U64							P2;
	};

private:
	xr_vector<EVENT>				Events;
	xr_vector<Deferred>				Events_Deferred;
	xrCriticalSection				CS;

public:
	EVENT			Create			(const char* N);
	void			Destroy			(EVENT& E);

	EVENT			Handler_Attach	(const char* N, IEventReceiver* H);
	void			Handler_Detach	(EVENT& E, IEventReceiver* H);

	void			Signal			(EVENT E, U64 P1 = 0, U64 P2 = 0);
	void			Signal			(const char* E, U64 P1 = 0, U64 P2 = 0);
	void			Defer			(EVENT E, U64 P1 = 0, U64 P2 = 0);
	void			Defer			(const char* E, U64 P1 = 0, U64 P2 = 0);

	void			OnFrame			( );
	void			Dump			( );
	BOOL			Peek			(const char* EName);

	void			_destroy		( );
};
