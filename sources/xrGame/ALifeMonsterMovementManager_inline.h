//	Module 		: ALifeMonsterMovementManager_inline.h
//	Description : ALife monster movement manager class inline functions

#pragma once

inline CALifeMonsterMovementManager::object_type& CALifeMonsterMovementManager::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}

inline CALifeMonsterMovementManager::detail_path_type& CALifeMonsterMovementManager::detail( ) const
{
	VERIFY(m_detail);
	return *m_detail;
}

inline CALifeMonsterMovementManager::patrol_path_type& CALifeMonsterMovementManager::patrol( ) const
{
	VERIFY(m_patrol);
	return *m_patrol;
}

inline const CALifeMonsterMovementManager::EPathType& CALifeMonsterMovementManager::path_type( ) const
{
	return m_path_type;
}

inline void CALifeMonsterMovementManager::path_type(const EPathType& path_type)
{
	m_path_type = path_type;
}
