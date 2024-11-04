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
	fVector3			m_tParticlePosition;
	fVector3			m_tParticleAngles;
	fVector3			m_tParticleVelocity;

public:
	IC				CParticleParams(const fVector3& tPositionOffset = fVector3( ).set(0.0f, 0.0f, 0.0f), const fVector3& tAnglesOffset = fVector3( ).set(0.0f, 0.0f, 0.0f), const fVector3& tVelocity = fVector3( ).set(0.0f, 0.0f, 0.0f));
	virtual			~CParticleParams	();
	IC		void	initialize			();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CParticleParams)
#undef script_type_list
#define script_type_list save_type_list(CParticleParams)

#include "particle_params_inline.h"