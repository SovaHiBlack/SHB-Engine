////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_anomaly_planner.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker anomaly planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CStalker;

class CStalkerAnomalyPlanner : public CActionPlannerActionScript<CStalker>
{
private:
	typedef CActionPlannerActionScript<CStalker> inherited;

public:
	CStalkerAnomalyPlanner(CStalker* object = 0, pcstr action_name = "");
	virtual				~CStalkerAnomalyPlanner( );
	virtual	void		setup(CStalker* object, CPropertyStorage* storage);
	virtual	void		update( );
	void		add_evaluators( );
	void		add_actions( );
};
