////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_in_direction_actions.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger in direction actions classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionTakeCover
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionTakeCover : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

private:
	bool				m_direction_sight;

public:
	CStalkerActionDangerInDirectionTakeCover(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionLookOut
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionLookOut : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

private:
	CRandom32			m_crouch_look_out_random;

public:
	CStalkerActionDangerInDirectionLookOut(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionHoldPosition
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionHoldPosition : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionDangerInDirectionHoldPosition(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionDetour
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionDetour : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionDangerInDirectionDetour(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionSearch
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionSearch : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionDangerInDirectionSearch(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};
