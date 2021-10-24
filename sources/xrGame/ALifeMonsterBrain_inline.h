//	Module 		: ALifeMonsterBrain_inline.h
//	Description : ALife monster brain class inline functions

#pragma once

inline CALifeMonsterBrain::object_type& CALifeMonsterBrain::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}

inline CALifeMonsterBrain::movement_manager_type& CALifeMonsterBrain::movement( ) const
{
	VERIFY(m_movement_manager);
	return *m_movement_manager;
}

inline bool CALifeMonsterBrain::can_choose_alife_tasks( ) const
{
	return m_can_choose_alife_tasks;
}

inline void CALifeMonsterBrain::can_choose_alife_tasks(bool value)
{
	m_can_choose_alife_tasks = value;
}
