//	Module 		: member_enemy.h
//	Description : Member enemy

#pragma once

#include "memory_space.h"

class CEntityAlive;

class CMemberEnemy {
public:
	typedef MemorySpace::squad_mask_type	squad_mask_type;
	typedef _flags<squad_mask_type>			mask_type;

	const CEntityAlive	*m_object;
	mask_type			m_mask;
	mask_type			m_distribute_mask;
	float				m_probability;
	Fvector3				m_enemy_position;
	u32					m_level_time;

	inline					CMemberEnemy		(const CEntityAlive *object, squad_mask_type mask);
	inline		bool		operator==			(const CEntityAlive *object) const;
	inline		bool		operator<			(const CMemberEnemy &enemy) const;
};

#include "member_enemy_inline.h"