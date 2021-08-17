////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_base_inline.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator base inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	bool								CALifeSimulatorBase::initialized			() const
{
	return						(m_initialized);
}

inline	const CALifeSimulatorHeader			&CALifeSimulatorBase::header				() const
{
	VERIFY						(initialized());
	VERIFY						(m_header);
	return						(*m_header);
}

inline	const CALifeTimeManager				&CALifeSimulatorBase::time					() const
{
	VERIFY						(initialized());
	VERIFY						(m_time_manager);
	return						(*m_time_manager);
}

inline	const CALifeSpawnRegistry			&CALifeSimulatorBase::spawns				() const
{
	VERIFY						(initialized());
	VERIFY						(m_spawns);
	return						(*m_spawns);
}

inline	const CALifeObjectRegistry			&CALifeSimulatorBase::objects				() const
{
	VERIFY						(initialized());
	VERIFY						(m_objects);
	return						(*m_objects);
}

inline	const CALifeGraphRegistry			&CALifeSimulatorBase::graph					() const
{
	VERIFY						(initialized());
	VERIFY						(m_graph_objects);
	return						(*m_graph_objects);
}

inline	const CALifeScheduleRegistry		&CALifeSimulatorBase::scheduled				() const
{
	VERIFY						(initialized());
	VERIFY						(m_scheduled);
	return						(*m_scheduled);
}

inline	const CALifeStoryRegistry			&CALifeSimulatorBase::story_objects			() const
{
	VERIFY						(initialized());
	VERIFY						(m_story_objects);
	return						(*m_story_objects);
}

inline	const CALifeSmartTerrainRegistry	&CALifeSimulatorBase::smart_terrains		() const
{
	VERIFY						(initialized());
	VERIFY						(m_smart_terrains);
	return						(*m_smart_terrains);
}

inline	const CALifeGroupRegistry			&CALifeSimulatorBase::groups				() const
{
	VERIFY						(initialized());
	VERIFY						(m_groups);
	return						(*m_groups);
}

inline	CALifeSimulatorHeader				&CALifeSimulatorBase::header				()
{
	VERIFY						(initialized());
	VERIFY						(m_header);
	return						(*m_header);
}

inline	CALifeTimeManager					&CALifeSimulatorBase::time					()
{
	VERIFY						(initialized());
	VERIFY						(m_time_manager);
	return						(*m_time_manager);
}

inline	CALifeSpawnRegistry					&CALifeSimulatorBase::spawns				()
{
	VERIFY						(initialized());
	VERIFY						(m_spawns);
	return						(*m_spawns);
}

inline	CALifeObjectRegistry				&CALifeSimulatorBase::objects				()
{
	VERIFY						(initialized());
	VERIFY						(m_objects);
	return						(*m_objects);
}

inline	CALifeGraphRegistry					&CALifeSimulatorBase::graph					()
{
	VERIFY						(initialized());
	VERIFY						(m_graph_objects);
	return						(*m_graph_objects);
}

inline	CALifeScheduleRegistry				&CALifeSimulatorBase::scheduled				()
{
	VERIFY						(initialized());
	VERIFY						(m_scheduled);
	return						(*m_scheduled);
}

inline	CALifeStoryRegistry					&CALifeSimulatorBase::story_objects			()
{
	VERIFY						(initialized());
	VERIFY						(m_story_objects);
	return						(*m_story_objects);
}

inline	CALifeSmartTerrainRegistry			&CALifeSimulatorBase::smart_terrains		()
{
	VERIFY						(initialized());
	VERIFY						(m_smart_terrains);
	return						(*m_smart_terrains);
}

inline	CALifeGroupRegistry					&CALifeSimulatorBase::groups				()
{
	VERIFY						(initialized());
	VERIFY						(m_groups);
	return						(*m_groups);
}

inline	CALifeRegistryContainer		&CALifeSimulatorBase::registry						() const
{
	VERIFY						(initialized());
	VERIFY						(m_registry_container);
	return						(*m_registry_container);
}

inline	CRandom32							&CALifeSimulatorBase::random				()
{
	return						(m_random);
}

inline	void								CALifeSimulatorBase::setup_command_line		(CSharedString* command_line)
{
	m_server_command_line		= command_line;
}

inline	CServer&CALifeSimulatorBase::server				() const
{
	VERIFY						(m_server);
	return						(*m_server);
}

inline	CALifeTimeManager					&CALifeSimulatorBase::time_manager			()
{
	VERIFY						(initialized());
	VERIFY						(m_time_manager);
	return						(*m_time_manager);
}

inline	const CALifeTimeManager				&CALifeSimulatorBase::time_manager			() const
{
	VERIFY						(initialized());
	VERIFY						(m_time_manager);
	return						(*m_time_manager);
}

inline	CSharedString* CALifeSimulatorBase::server_command_line	() const
{
	VERIFY						(m_server_command_line);
	return						(m_server_command_line);
}

template <typename T>
inline	T									&CALifeSimulatorBase::registry				(T *t) const
{
	return						(registry()(t));
}

inline	void								CALifeSimulatorBase::can_register_objects	(const bool &value)
{
	VERIFY						(m_can_register_objects != value);
	m_can_register_objects		= value;
}

inline	const bool							&CALifeSimulatorBase::can_register_objects	() const
{
	return						(m_can_register_objects);
}
