////////////////////////////////////////////////////////////////////////////
//	Module 		: particle_params_inline.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Particle parameters class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CParticleParams::CParticleParams	(const Fvector3& tPositionOffset, const Fvector3& tAnglesOffset, const Fvector3& tVelocity)
{
	m_tParticlePosition	= tPositionOffset;
	m_tParticleAngles	= tAnglesOffset;
	m_tParticleVelocity	= tVelocity;
}

inline	void CParticleParams::initialize	()
{ }
