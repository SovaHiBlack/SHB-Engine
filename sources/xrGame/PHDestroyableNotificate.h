#pragma once

class CSE_Abstract;
class CPHShellHolder;

class CPHDestroyableNotificate
{
public:
	virtual CPHDestroyableNotificate*	cast_phdestroyable_notificate	( )
	{
		return this;
	}
	virtual CPHShellHolder*				PPhysicsShellHolder				( ) = 0;
	virtual void						spawn_init						( )
	{ }
	void								spawn_notificate				(CSE_Abstract*);
};
