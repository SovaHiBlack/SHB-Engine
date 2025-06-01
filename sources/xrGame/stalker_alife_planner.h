////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_planner.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CStalker;

class CStalkerALifePlanner : public CActionPlannerActionScript<CStalker> {
private:
	typedef CActionPlannerActionScript<CStalker> inherited;

public:
	CStalkerALifePlanner(CStalker* object = 0, pcstr action_name = "");
	virtual				~CStalkerALifePlanner	();
	virtual	void		setup(CStalker* object, CPropertyStorage* storage);
			void		add_evaluators			();
			void		add_actions				();
};
