////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_storage_inline.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path storage inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CPatrolPathStorage::CPatrolPathStorage		()
{ }

inline	const CPatrolPathStorage::PATROL_REGISTRY &CPatrolPathStorage::patrol_paths	() const
{
	return			(m_registry);
}

inline	const CPatrolPath *CPatrolPathStorage::path	(CSharedString patrol_name, bool no_assert) const
{
	const_iterator	I = patrol_paths().find(patrol_name);
	if (I == patrol_paths().end()) {
		THROW3		(no_assert,"There is no patrol path",*patrol_name);
		return		(0);
	}
	return			((*I).second);
}
