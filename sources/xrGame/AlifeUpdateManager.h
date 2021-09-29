//	Module 		: AlifeUpdateManager.h
//	Description : ALife Simulator update manager

#pragma once

#include "AlifeSwitchManager.h"
#include "alife_surge_manager.h"
#include "alife_storage_manager.h"

namespace Restriction
{
	enum ERestrictorTypes;
}

class CAlifeUpdateManager : public CAlifeSwitchManager, public CALifeSurgeManager, public CALifeStorageManager, public ISheduled
{
private:
	bool				m_first_time;

protected:
	unsigned __int64					m_max_process_time;
	float				m_update_monster_factor;
	unsigned int					m_objects_per_update;
	bool				m_changing_level;

public:
	void __stdcall	update( );

protected:
	void		new_game(const char* save_name);
	void		init_ef_storage( ) const;
	virtual void		reload(const char* section);

public:
	CAlifeUpdateManager(CServer* server, const char* section);
	virtual 			~CAlifeUpdateManager( );
	virtual	CSharedString	shedule_Name( ) const
	{
		return CSharedString("alife_simulator");
	}
	virtual float		shedule_Scale( );
	virtual void		shedule_Update(unsigned int dt);
	virtual bool		shedule_Needed( )
	{
		return true;
	}
	void		update_switch( );
	void		update_scheduled(bool init_ef = true);
	void		load(const char* game_name = 0, bool no_assert = false, bool new_only = false);
	bool		load_game(const char* game_name, bool no_assert = false);
	inline float		update_monster_factor( ) const;
	bool		change_level(CNetPacket& net_packet);
	void		set_process_time(int microseconds);
	void		objects_per_update(const unsigned int& objects_per_update);
	void		set_switch_online(ALife::_OBJECT_ID id, bool value);
	void		set_switch_offline(ALife::_OBJECT_ID id, bool value);
	void		set_interactive(ALife::_OBJECT_ID id, bool value);
	void		jump_to_level(const char* level_name) const;
	void		teleport_object(ALife::_OBJECT_ID id, GameGraph::_GRAPH_ID game_vertex_id, unsigned int level_vertex_id, const Fvector3& position);
	void		add_restriction(ALife::_OBJECT_ID id, ALife::_OBJECT_ID restriction_id, const Restriction::ERestrictorTypes& restriction_type);
	void		remove_restriction(ALife::_OBJECT_ID id, ALife::_OBJECT_ID restriction_id, const Restriction::ERestrictorTypes& restriction_type);
	void		remove_all_restrictions(ALife::_OBJECT_ID id, const Restriction::ERestrictorTypes& restriction_type);
};

#include "AlifeUpdateManager_inline.h"
