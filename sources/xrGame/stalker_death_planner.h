////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_death_planner.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker death planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CStalker;

class CStalkerDeathPlanner : public CActionPlannerActionScript<CStalker> {
private:
	typedef CActionPlannerActionScript<CStalker> inherited;

public:
						CStalkerDeathPlanner	(CStalker*object = 0, pcstr action_name = "");
	virtual				~CStalkerDeathPlanner	();
	virtual	void		setup(CStalker* object, CPropertyStorage* storage);
			void		add_evaluators			();
			void		add_actions				();
};
