////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_patrol_path_manager_inline.h
//	Created 	: 01.11.2005
//  Modified 	: 22.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster patrol path manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CALifeMonsterPatrolPathManager::object_type &CALifeMonsterPatrolPathManager::object	() const
{
	VERIFY		(m_object);
	return		(*m_object);
}

inline	void CALifeMonsterPatrolPathManager::path					(const CPatrolPath *path)
{
	m_actual				= m_actual && (m_path == path);
	m_path					= path;
}

inline	void CALifeMonsterPatrolPathManager::path					(const char* path_name)
{
	path					(CSharedString(path_name));
}

inline	bool CALifeMonsterPatrolPathManager::actual				() const
{
	return					(m_actual);
}

inline	bool CALifeMonsterPatrolPathManager::completed			() const
{
	return					(actual() && m_completed);
}

inline	void CALifeMonsterPatrolPathManager::start_type			(const EPatrolStartType	&start_type)
{
	m_start_type			= start_type;
}

inline	void CALifeMonsterPatrolPathManager::route_type			(const EPatrolRouteType	&route_type)
{
	m_route_type			= route_type;
}

inline	const CALifeMonsterPatrolPathManager::EPatrolStartType &CALifeMonsterPatrolPathManager::start_type	() const
{
	return					(m_start_type);
}

inline	const CALifeMonsterPatrolPathManager::EPatrolRouteType &CALifeMonsterPatrolPathManager::route_type	() const
{
	return					(m_route_type);
}

inline	const CPatrolPath &CALifeMonsterPatrolPathManager::path	() const
{
	VERIFY					(m_path);
	return					(*m_path);
}

inline	void CALifeMonsterPatrolPathManager::start_vertex_index	(const u32 &start_vertex_index)
{
	m_start_vertex_index	= start_vertex_index;
}

inline	bool CALifeMonsterPatrolPathManager::use_randomness		() const
{
	return					(m_use_randomness);
}

inline	void CALifeMonsterPatrolPathManager::use_randomness		(const bool &use_randomness)
{
	m_use_randomness		= use_randomness;
}
