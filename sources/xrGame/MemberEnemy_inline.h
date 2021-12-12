//	Module 		: MemberEnemy_inline.h
//	Description : Member enemy inline functions

#pragma once

inline CMemberEnemy::CMemberEnemy(const CEntityAlive* object, squad_mask_type mask)
{
	m_object = object;
	m_mask.assign(mask);
	m_probability = 1.0f;
	m_distribute_mask.zero( );
}

inline bool CMemberEnemy::operator== (const CEntityAlive* object) const
{
	return (m_object == object);
}

inline bool CMemberEnemy::operator< (const CMemberEnemy& enemy) const
{
	return (m_probability > enemy.m_probability);
}
