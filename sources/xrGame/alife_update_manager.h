////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_update_manager.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator update manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_switch_manager.h"
#include "alife_surge_manager.h"
#include "alife_storage_manager.h"

namespace RestrictionSpace {
	enum ERestrictorTypes;
}

class CALifeUpdateManager :
	public CALifeSwitchManager,
	public CALifeSurgeManager,
	public CALifeStorageManager,
	public ISheduled
{
private:
	bool				m_first_time;

protected:
	u64					m_max_process_time;
	f32				m_update_monster_factor;
	u32					m_objects_per_update;
	bool				m_changing_level;

public:
			void __stdcall	update				();

protected:
			void		new_game				(pcstr	save_name);
			void		init_ef_storage			() const;
	virtual	void		reload					(pcstr section);

public:
						CALifeUpdateManager		(xrServer *server, pcstr section);
	virtual 			~CALifeUpdateManager	();
	virtual	shared_str	shedule_Name			() const		{ return shared_str("alife_simulator"); };
	virtual f32		shedule_Scale			();
	virtual void		shedule_Update			(u32 dt);	
	virtual bool		shedule_Needed			()				{return true;};
			void		update_switch			();
			void		update_scheduled		(bool init_ef = true);
			void		load					(pcstr game_name = 0, bool no_assert = false, bool new_only = false);
			bool		load_game				(pcstr game_name, bool no_assert = false);
	IC		f32		update_monster_factor	() const;
			bool		change_level			(CNetPacket&net_packet);
			void		set_process_time		(s32 microseconds);
			void		objects_per_update		(const u32 &objects_per_update);
			void		set_switch_online		(ALife::_OBJECT_ID id, bool value);
			void		set_switch_offline		(ALife::_OBJECT_ID id, bool value);
			void		set_interactive			(ALife::_OBJECT_ID id, bool value);
			void		jump_to_level			(pcstr level_name) const;
			void		teleport_object			(ALife::_OBJECT_ID id, GameGraph::_GRAPH_ID game_vertex_id, u32 level_vertex_id, const fVector3& position);
			void		add_restriction			(ALife::_OBJECT_ID id, ALife::_OBJECT_ID restriction_id, const RestrictionSpace::ERestrictorTypes &restriction_type);
			void		remove_restriction		(ALife::_OBJECT_ID id, ALife::_OBJECT_ID restriction_id, const RestrictionSpace::ERestrictorTypes &restriction_type);
			void		remove_all_restrictions	(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes &restriction_type);
};

#include "alife_update_manager_inline.h"
