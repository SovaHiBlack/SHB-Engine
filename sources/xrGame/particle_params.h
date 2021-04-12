////////////////////////////////////////////////////////////////////////////
//	Module 		: particle_params.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Particle parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

class CParticleParams {
public:
	Fvector3			m_tParticlePosition;
	Fvector3			m_tParticleAngles;
	Fvector3			m_tParticleVelocity;

public:
	inline				CParticleParams		(const Fvector3& tPositionOffset = Fvector3().set(0,0,0), const Fvector3& tAnglesOffset = Fvector3().set(0,0,0), const Fvector3& tVelocity = Fvector3().set(0,0,0));
	virtual			~CParticleParams	();
	inline		void	initialize			();

public:
	static void script_register(lua_State*);
};

add_to_type_list(CParticleParams)
#undef script_type_list
#define script_type_list save_type_list(CParticleParams)

#include "particle_params_inline.h"
