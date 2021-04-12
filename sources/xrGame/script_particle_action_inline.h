////////////////////////////////////////////////////////////////////////////
//	Module 		: script_particle_action_inline.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script particle action class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CScriptParticleAction::CScriptParticleAction	()
{
	m_caParticleToRun	= "";
	m_caBoneName		= "";
	m_tGoalType			= eGoalTypeDummy;
	m_bCompleted		= false;
	m_bStartedToPlay	= false;
	m_tpParticleSystem	= 0;
	m_tParticlePosition.set	(0,0,0);
	m_tParticleAngles.set	(0,0,0);
	m_tParticleVelocity.set	(0,0,0);
	m_bAutoRemove		= true;
}

inline	CScriptParticleAction::CScriptParticleAction	(const char* caPartcileToRun, const char* caBoneName, const CParticleParams &tParticleParams, bool bAutoRemove)
{
	SetBone				(caBoneName);
	SetPosition			(tParticleParams.m_tParticlePosition);
	SetAngles			(tParticleParams.m_tParticleAngles);
	SetVelocity			(tParticleParams.m_tParticleVelocity);
	SetParticle			(caPartcileToRun,bAutoRemove);
}

inline	CScriptParticleAction::CScriptParticleAction	(const char* caPartcileToRun, const CParticleParams &tParticleParams, bool bAutoRemove)
{
	SetParticle			(caPartcileToRun,bAutoRemove);
	SetPosition			(tParticleParams.m_tParticlePosition);
	SetAngles			(tParticleParams.m_tParticleAngles);
	SetVelocity			(tParticleParams.m_tParticleVelocity);
}

inline	void CScriptParticleAction::SetPosition			(const Fvector3& tPosition)
{
	m_tParticlePosition	= tPosition;
	m_tGoalType			= eGoalTypeParticlePosition;
	m_bStartedToPlay	= false;
	m_bCompleted		= false;
}

inline	void CScriptParticleAction::SetBone				(const char* caBoneName)
{
	m_caBoneName		= caBoneName;
	m_bStartedToPlay	= false;
	m_bCompleted		= false;
}

inline	void CScriptParticleAction::SetAngles			(const Fvector3& tAngleOffset)
{
	m_tParticleAngles	= tAngleOffset;
	m_bStartedToPlay	= false;
	m_bCompleted		= false;
}

inline	void CScriptParticleAction::SetVelocity			(const Fvector3& tVelocity)
{
	m_tParticleVelocity	= tVelocity;
	m_bStartedToPlay	= false;
	m_bCompleted		= false;
}

inline	void CScriptParticleAction::initialize			()
{ }
