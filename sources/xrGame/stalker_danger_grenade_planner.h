////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_grenade_planner.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger grenade planner class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CStalker;

class CStalkerDangerGrenadePlanner : public CActionPlannerActionScript<CStalker>
{
private:
	typedef CActionPlannerActionScript<CStalker> inherited;

protected:
	void		add_evaluators( );
	void		add_actions( );

public:
	CStalkerDangerGrenadePlanner(CStalker* object = 0, pcstr action_name = "");
	virtual	void		setup(CStalker* object, CPropertyStorage* storage);
	virtual void		initialize( );
	virtual void		update( );
	virtual void		finalize( );
};
