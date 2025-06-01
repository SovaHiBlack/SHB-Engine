////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_kill_wounded_planner.h
//	Created 	: 25.05.2006
//  Modified 	: 25.05.2006
//	Author		: Dmitriy Iassenev
//	Description : Stalker kill wounded planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CStalker;

class CStalkerKillWoundedPlanner : public CActionPlannerActionScript<CStalker> {
private:
	typedef CActionPlannerActionScript<CStalker>	inherited;

private:
			void	add_evaluators				();
			void	add_actions					();

public:
					CStalkerKillWoundedPlanner	(CStalker*object = 0, pcstr action_name = "");
	virtual			~CStalkerKillWoundedPlanner	();
	virtual	void	setup						(CStalker*object, CPropertyStorage *storage);
	virtual void	update						();
	virtual void	initialize					();
	virtual void	execute						();
	virtual void	finalize					();
};
