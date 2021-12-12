//	Module 		: AgentEnemyManager_inline.h
//	Description : Agent enemy manager inline functions

#pragma once

inline CAgentEnemyManager::CAgentEnemyManager(CAgentManager* object)
{
	VERIFY(object);
	m_object = object;
	m_only_wounded_left = false;
	m_is_any_wounded = false;
}

inline CAgentManager& CAgentEnemyManager::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}

inline CAgentEnemyManager::ENEMIES& CAgentEnemyManager::enemies( )
{
	return m_enemies;
}
