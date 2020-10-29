////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_kill_wounded_actions.h
//	Created 	: 25.05.2006
//  Modified 	: 25.05.2006
//	Author		: Dmitriy Iassenev
//	Description : Stalker kill wounded action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionReachWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionReachWounded : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

public:
						CStalkerActionReachWounded	(CStalker *object, const char* action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionAimWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionAimWounded : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

private:
//	float				m_speed;

public:
						CStalkerActionAimWounded	(CStalker *object, const char* action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPrepareWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionPrepareWounded : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

public:
						CStalkerActionPrepareWounded(CStalker *object, const char* action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillWounded : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

public:
						CStalkerActionKillWounded	(CStalker *object, const char* action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPauseAfterKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionPauseAfterKill : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

public:
						CStalkerActionPauseAfterKill(CStalker *object, const char* action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};
