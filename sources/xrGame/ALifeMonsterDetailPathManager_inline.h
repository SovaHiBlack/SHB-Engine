//	Module 		: ALifeMonsterDetailPathManager_inline.h
//	Description : ALife monster detail path manager class inline functions

#pragma once

inline CALifeMonsterDetailPathManager::object_type& CALifeMonsterDetailPathManager::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}

inline void CALifeMonsterDetailPathManager::speed(const float& speed)
{
	VERIFY(_valid(speed));
	m_speed = speed;
}

inline const float& CALifeMonsterDetailPathManager::speed( ) const
{
	VERIFY(_valid(m_speed));
	return m_speed;
}

inline const CALifeMonsterDetailPathManager::PATH& CALifeMonsterDetailPathManager::path( ) const
{
	return m_path;
}

inline const float& CALifeMonsterDetailPathManager::walked_distance( ) const
{
	VERIFY(path( ).size( ) > 1);
	return m_walked_distance;
}
