////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_action_base.h
//	Created 	: 25.03.2004
//  Modified 	: 12.01.2007
//	Author		: Dmitriy Iassenev
//	Description : Stalker base combat action 
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_base_action.h"

class CStalkerActionCombatBase : public CStalkerActionBase
{
protected:
	typedef CStalkerActionBase inherited;

public:
	CStalkerActionCombatBase(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		finalize( );
	void		select_queue_params(const f32& distance, u32& min_queue_size, u32& max_queue_size, u32& min_queue_interval, u32& max_queue_interval) const;
	bool		fire_make_sense( ) const;
	void		fire( );
	void		aim_ready( );
	void		aim_ready_force_full( );
	void		play_panic_sound(u32 max_start_time = 0, u32 min_start_time = 0, u32 max_stop_time = 0, u32 min_stop_time = 0, u32 id = u32(-1));
	void		play_attack_sound(u32 max_start_time = 0, u32 min_start_time = 0, u32 max_stop_time = 0, u32 min_stop_time = 0, u32 id = u32(-1));
	void		play_start_search_sound(u32 max_start_time = 0, u32 min_start_time = 0, u32 max_stop_time = 0, u32 min_stop_time = 0, u32 id = u32(-1));
};
