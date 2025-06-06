////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_by_sound_actions.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger by sound actions classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundListenTo
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundListenTo : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionDangerBySoundListenTo(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundCheck
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundCheck : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionDangerBySoundCheck(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundTakeCover
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundTakeCover : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionDangerBySoundTakeCover(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundLookOut
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundLookOut : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionDangerBySoundLookOut(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundLookAround
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundLookAround : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionDangerBySoundLookAround(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};
