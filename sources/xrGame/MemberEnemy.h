//	Module 		: MemberEnemy.h
//	Description : Member enemy

#pragma once

#include "memory_space.h"

class CEntityAlive;

class CMemberEnemy
{
public:
	using squad_mask_type					= MemorySpace::squad_mask_type;
	using mask_type							= _flags<squad_mask_type>;

	const CEntityAlive*						m_object;
	mask_type								m_mask;
	mask_type								m_distribute_mask;
	float									m_probability;
	Fvector3								m_enemy_position;
	unsigned int							m_level_time;

	inline				CMemberEnemy		(const CEntityAlive* object, squad_mask_type mask);
	inline bool			operator==			(const CEntityAlive* object) const;
	inline bool			operator<			(const CMemberEnemy& enemy) const;
};

#include "MemberEnemy_inline.h"
