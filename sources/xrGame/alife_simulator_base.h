////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_base.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator base class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "random32.h"
#include "ALife_space.h"
#include "game_graph_space.h"
#include "object_interfaces.h"

class CServer;
class CALifeSimulatorHeader;
class CALifeTimeManager;
class CALifeSpawnRegistry;
class CALifeObjectRegistry;
class CALifeGraphRegistry;
class CALifeScheduleRegistry;
class CALifeStoryRegistry;
class CALifeSmartTerrainRegistry;
class CALifeGroupRegistry;
class CALifeRegistryContainer;

class CSE_Abstract;
class CSE_ALifeObject;
class CSE_ALifeDynamicObject;
class CSE_ALifeGroupAbstract;
class CSE_ALifeCreatureAbstract;

class CAlifeSimulatorBase : public IPureDestroyableObject
{
protected:
	CServer* m_server;
	CALifeSimulatorHeader* m_header;
	CALifeTimeManager* m_time_manager;
	CALifeSpawnRegistry* m_spawns;
	CALifeObjectRegistry* m_objects;
	CALifeGraphRegistry* m_graph_objects;
	CALifeScheduleRegistry* m_scheduled;
	CALifeStoryRegistry* m_story_objects;
	CALifeSmartTerrainRegistry* m_smart_terrains;
	CALifeGroupRegistry* m_groups;
	CALifeRegistryContainer* m_registry_container;
	CRandom32									m_random;
	bool										m_initialized;
	CSharedString* m_server_command_line;
	bool										m_can_register_objects;
	// temp
	ALife::SCHEDULE_P_VECTOR					m_tpaCombatGroups[2];

	inline CALifeSimulatorHeader& header( );
	inline CALifeTimeManager& time( );
	inline CALifeSpawnRegistry& spawns( );
	inline CALifeObjectRegistry& objects( );
	inline CALifeStoryRegistry& story_objects( );
	inline CALifeSmartTerrainRegistry& smart_terrains( );
	inline CALifeGroupRegistry& groups( );
	inline void								can_register_objects(const bool& value);
	inline const bool& can_register_objects( ) const;

public:
	inline CALifeGraphRegistry& graph( );
	inline CALifeScheduleRegistry& scheduled( );
	inline CALifeTimeManager& time_manager( );
	inline CALifeRegistryContainer& registry( ) const;

	CAlifeSimulatorBase(CServer* server, const char* section);
	virtual										~CAlifeSimulatorBase( );
	virtual void								destroy( );
	inline bool								initialized( ) const;
	inline const CALifeSimulatorHeader& header( ) const;
	inline const CALifeTimeManager& time( ) const;
	inline const CALifeSpawnRegistry& spawns( ) const;
	inline const CALifeObjectRegistry& objects( ) const;
	inline const CALifeGraphRegistry& graph( ) const;
	inline const CALifeScheduleRegistry& scheduled( ) const;
	inline const CALifeStoryRegistry& story_objects( ) const;
	inline const CALifeSmartTerrainRegistry& smart_terrains( ) const;
	inline const CALifeGroupRegistry& groups( ) const;
	inline CRandom32& random( );
	inline CServer& server( ) const;
	inline const CALifeTimeManager& time_manager( ) const;
	inline CSharedString* server_command_line( ) const;

	template <typename T>
	inline T& registry(T* t) const;

protected:
	void								unload( );
	virtual void								reload(const char* section);
	inline void								setup_command_line(CSharedString* command_line);
	void								assign_death_position(CSE_ALifeCreatureAbstract* tpALifeCreatureAbstract, GameGraph::_GRAPH_ID tGraphID, CSE_ALifeSchedulable* tpALifeSchedulable = 0);
	virtual void								setup_simulator(CSE_ALifeObject* object) = 0;

public:
	void								register_object(CSE_ALifeDynamicObject* object, bool add_object = false);
	void								unregister_object(CSE_ALifeDynamicObject* object, bool alife_query = true);
	void								release(CSE_Abstract* object, bool alife_query = true);
	void								create(CSE_ALifeDynamicObject*& object, CSE_ALifeDynamicObject* spawn_object, const ALife::_SPAWN_ID& spawn_id);
	void								create(CSE_ALifeObject* object);
	CSE_Abstract* create(CSE_ALifeGroupAbstract* object, CSE_ALifeDynamicObject* j);
	CSE_Abstract* spawn_item(const char* section, const Fvector3& position, u32 level_vertex_id, GameGraph::_GRAPH_ID game_vertex_id, U16 parent_id, bool registration = true);
	void								append_item_vector(ALife::OBJECT_VECTOR& tObjectVector, ALife::ITEM_P_VECTOR& tItemList);
	CSharedString							level_name( ) const;
	void								on_death(CSE_Abstract* killed, CSE_Abstract* killer);

	ALife::ITEM_P_VECTOR						m_temp_item_vector;
};

#include "alife_simulator_base_inline.h"
