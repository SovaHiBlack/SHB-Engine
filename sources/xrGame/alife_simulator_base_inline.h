////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_base_inline.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator base inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline bool CAlifeSimulatorBase::initialized( ) const
{
	return m_initialized;
}

inline const CALifeSimulatorHeader& CAlifeSimulatorBase::header( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_header);
	return *m_header;
}

inline const CALifeTimeManager& CAlifeSimulatorBase::time( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_time_manager);
	return *m_time_manager;
}

inline const CALifeSpawnRegistry& CAlifeSimulatorBase::spawns( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_spawns);
	return *m_spawns;
}

inline const CALifeObjectRegistry& CAlifeSimulatorBase::objects( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_objects);
	return *m_objects;
}

inline const CALifeGraphRegistry& CAlifeSimulatorBase::graph( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_graph_objects);
	return *m_graph_objects;
}

inline const CALifeScheduleRegistry& CAlifeSimulatorBase::scheduled( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_scheduled);
	return *m_scheduled;
}

inline const CALifeStoryRegistry& CAlifeSimulatorBase::story_objects( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_story_objects);
	return *m_story_objects;
}

inline const CALifeSmartTerrainRegistry& CAlifeSimulatorBase::smart_terrains( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_smart_terrains);
	return *m_smart_terrains;
}

inline const CALifeGroupRegistry& CAlifeSimulatorBase::groups( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_groups);
	return *m_groups;
}

inline CALifeSimulatorHeader& CAlifeSimulatorBase::header( )
{
	VERIFY(initialized( ));
	VERIFY(m_header);
	return *m_header;
}

inline CALifeTimeManager& CAlifeSimulatorBase::time( )
{
	VERIFY(initialized( ));
	VERIFY(m_time_manager);
	return *m_time_manager;
}

inline CALifeSpawnRegistry& CAlifeSimulatorBase::spawns( )
{
	VERIFY(initialized( ));
	VERIFY(m_spawns);
	return *m_spawns;
}

inline CALifeObjectRegistry& CAlifeSimulatorBase::objects( )
{
	VERIFY(initialized( ));
	VERIFY(m_objects);
	return *m_objects;
}

inline CALifeGraphRegistry& CAlifeSimulatorBase::graph( )
{
	VERIFY(initialized( ));
	VERIFY(m_graph_objects);
	return *m_graph_objects;
}

inline CALifeScheduleRegistry& CAlifeSimulatorBase::scheduled( )
{
	VERIFY(initialized( ));
	VERIFY(m_scheduled);
	return *m_scheduled;
}

inline CALifeStoryRegistry& CAlifeSimulatorBase::story_objects( )
{
	VERIFY(initialized( ));
	VERIFY(m_story_objects);
	return *m_story_objects;
}

inline CALifeSmartTerrainRegistry& CAlifeSimulatorBase::smart_terrains( )
{
	VERIFY(initialized( ));
	VERIFY(m_smart_terrains);
	return *m_smart_terrains;
}

inline CALifeGroupRegistry& CAlifeSimulatorBase::groups( )
{
	VERIFY(initialized( ));
	VERIFY(m_groups);
	return *m_groups;
}

inline CALifeRegistryContainer& CAlifeSimulatorBase::registry( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_registry_container);
	return *m_registry_container;
}

inline CRandom32& CAlifeSimulatorBase::random( )
{
	return m_random;
}

inline void CAlifeSimulatorBase::setup_command_line(CSharedString* command_line)
{
	m_server_command_line = command_line;
}

inline CServer& CAlifeSimulatorBase::server( ) const
{
	VERIFY(m_server);
	return *m_server;
}

inline CALifeTimeManager& CAlifeSimulatorBase::time_manager( )
{
	VERIFY(initialized( ));
	VERIFY(m_time_manager);
	return *m_time_manager;
}

inline const CALifeTimeManager& CAlifeSimulatorBase::time_manager( ) const
{
	VERIFY(initialized( ));
	VERIFY(m_time_manager);
	return *m_time_manager;
}

inline CSharedString* CAlifeSimulatorBase::server_command_line( ) const
{
	VERIFY(m_server_command_line);
	return m_server_command_line;
}

template <typename T>
inline T& CAlifeSimulatorBase::registry(T* t) const
{
	return (registry( )(t));
}

inline void CAlifeSimulatorBase::can_register_objects(const bool& value)
{
	VERIFY(m_can_register_objects != value);
	m_can_register_objects = value;
}

inline const bool& CAlifeSimulatorBase::can_register_objects( ) const
{
	return m_can_register_objects;
}
