////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_planner.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CStalker;

class CStalkerCombatPlanner : public CActionPlannerActionScript<CStalker>
{
public:
	enum
	{
		POST_COMBAT_WAIT_INTERVAL = 3000,
	};

private:
	typedef CActionPlannerActionScript<CStalker> inherited;

private:
	u32					m_last_level_time;
	u16					m_last_enemy_id;
	bool				m_last_wounded;

protected:
	void		add_evaluators( );
	void		add_actions( );

public:
	void xr_stdcall	on_best_cover_changed(const CCoverPoint* new_cover, const CCoverPoint* old_cover);

public:
	CStalkerCombatPlanner(CStalker* object = 0, pcstr action_name = "");
	virtual				~CStalkerCombatPlanner( );
	virtual	void		setup(CStalker* object, CPropertyStorage* storage);
	virtual void		update( );
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
	virtual	void		save(CNetPacket& packet);
	virtual	void		load(IReader& packet);
};
