#pragma once

#include "..\xrCore\xrSyncronize.h"

class ENGINE_API	CEvent;
typedef CEvent*		EVENT;

//---------------------------------------------------------------------
class ENGINE_API	IEventReceiver
{
public:
	virtual void	OnEvent(EVENT E, u64 P1, u64 P2) = 0;
};
//---------------------------------------------------------------------
class ENGINE_API	CEventAPI
{
	struct Deferred
	{
		EVENT		E;
		u64			P1;
		u64			P2;
	};
private:
	xr_vector<EVENT>		Events;
	xr_vector<Deferred>		Events_Deferred;
	xrCriticalSection		CS;
public:
	EVENT	Create			(pcstr N);
	void	Destroy			(EVENT& E);

	EVENT	Handler_Attach	(pcstr N, IEventReceiver* H);
	void	Handler_Detach	(EVENT& E, IEventReceiver* H);

	void	Signal			(EVENT	E, u64 P1=0, u64 P2=0);
	void	Signal			(pcstr E, u64 P1=0, u64 P2=0);
	void	Defer			(EVENT	E, u64 P1=0, u64 P2=0);
	void	Defer			(pcstr E, u64 P1=0, u64 P2=0);

	void	OnFrame			();
	void	Dump			();
	BOOL	Peek			(pcstr EName);

	void	_destroy		();
};
