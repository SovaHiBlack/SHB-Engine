////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_planner.h
//	Created 	: 11.02.2005
//  Modified 	: 11.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CStalker;

class CStalkerDangerPlanner : public CActionPlannerActionScript<CStalker> {
private:
	typedef CActionPlannerActionScript<CStalker> inherited;

protected:
			void		add_evaluators			();
			void		add_actions				();

public:
	CStalkerDangerPlanner(CStalker* object = 0, pcstr action_name = "");
	virtual	void		setup(CStalker* object, CPropertyStorage* storage);
	virtual void		initialize				();
	virtual void		update					();
	virtual void		finalize				();
};
