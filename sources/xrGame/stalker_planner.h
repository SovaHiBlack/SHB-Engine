////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_planner.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker planner class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_script.h"
#include "action_script_base.h"
#include "action_planner_action_script.h"

class CStalker;

class CStalkerPlanner : public CActionPlannerScript<CStalker> {
protected:
	typedef CActionPlannerScript<CStalker>			inherited;
	typedef CActionScriptBase<CStalker>				CAction;
	typedef GraphEngineSpace::_solver_value_type		_value_type;
	typedef GraphEngineSpace::_solver_condition_type	_condition_type;
	typedef CActionPlannerActionScript<CStalker>		CActionPlannerAction;

private:
	bool					m_affect_cover;
	CWorldState				m_alive_goal;
	CWorldState				m_dead_goal;

protected:
			void			add_evaluators		();
			void			add_actions			();
#ifdef LOG_ACTION
public:
	virtual pcstr			action2string		(const _action_id_type &action_id);
	virtual pcstr			property2string		(const _condition_type &property_id);
#endif

public:
							CStalkerPlanner		();
	virtual					~CStalkerPlanner	();
	virtual	void			setup				(CStalker*object);
	virtual	void			update				(u32 time_delta);
	IC		void			affect_cover		(bool value);
	IC		bool			affect_cover		() const;

#ifdef LOG_ACTION
	virtual	pcstr			object_name			() const;
#endif
};

#include "stalker_planner_inline.h"
