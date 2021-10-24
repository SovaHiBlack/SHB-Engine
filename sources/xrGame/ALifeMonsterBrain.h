//	Module 		: ALifeMonsterBrain.h
//	Description : ALife monster brain class

#pragma once

#include "game_graph_space.h"
#include "Server_space.h"
#include "ALife_space.h"
#include "script_export_space.h"

class CSE_ALifeMonsterAbstract;
class CALifeMonsterMovementManager;
class CSE_ALifeSmartZone;
class CNetPacket;

class CALifeMonsterBrain
{
public:
	using object_type = CSE_ALifeMonsterAbstract;
	using movement_manager_type = CALifeMonsterMovementManager;

private:
	object_type* m_object;
	movement_manager_type* m_movement_manager;
	bool								m_can_choose_alife_tasks;

public:
	CSE_ALifeSmartZone* m_smart_terrain;
	ALife::_TIME_ID						m_last_search_time;
	ALife::_TIME_ID						m_time_interval;

	// sad, but true
	void						select_task( );

private:
	void						process_task( );
	void						default_behaviour( );
	inline bool						can_choose_alife_tasks( ) const;

public:
	CALifeMonsterBrain(object_type* object);
	virtual								~CALifeMonsterBrain( );

	void						on_state_write(CNetPacket& packet);
	void						on_state_read(CNetPacket& packet);
	void						on_register( );
	void						on_unregister( );
	void						on_location_change( );
	void						on_switch_online( );
	void						on_switch_offline( );

	void						update( );
	bool						perform_attack( );
	ALife::EMeetActionType		action_type(CSE_ALifeSchedulable* tpALifeSchedulable, const int& iGroupIndex, const bool& bMutualDetection);

	inline object_type& object( ) const;
	inline movement_manager_type& movement( ) const;
	inline CSE_ALifeSmartZone& smart_terrain( );
	inline void						can_choose_alife_tasks(bool value);

	static void script_register(lua_State*);
};

add_to_type_list(CALifeMonsterBrain)
#undef script_type_list
#define script_type_list save_type_list(CALifeMonsterBrain)

#include "ALifeMonsterBrain_inline.h"
