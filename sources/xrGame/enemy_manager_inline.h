////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager_inline.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	u32	CEnemyManager::last_enemy_time								() const
{
	return						(m_last_enemy_time);
}

inline	const CEntityAlive *CEnemyManager::last_enemy					() const
{
	return						(m_last_enemy);
}

inline	CEnemyManager::USEFULE_CALLBACK &CEnemyManager::useful_callback	()
{
	return						(m_useful_callback);
}

inline	void CEnemyManager::enable_enemy_change							(const bool &value)
{
	m_enable_enemy_change		= value;
}

inline	bool CEnemyManager::enable_enemy_change							() const
{
	return						(m_enable_enemy_change);
}
