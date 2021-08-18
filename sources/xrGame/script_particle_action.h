////////////////////////////////////////////////////////////////////////////
//	Module 		: script_particle_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script particle action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_abstract_action.h"
#include "script_export_space.h"
#include "particle_params.h"

class CParticlesObject;

class CScriptParticleAction : public CScriptAbstractAction {
public:
	enum EGoalType {
		eGoalTypeParticleAttached = u32(0),
		eGoalTypeParticlePosition,
		eGoalTypeDummy = u32(-1)
	};

	CSharedString			m_caParticleToRun;
	CSharedString			m_caBoneName;
	EGoalType			m_tGoalType;
	CParticlesObject	*m_tpParticleSystem;
	bool				m_bStartedToPlay;
	Fvector3				m_tParticlePosition;
	Fvector3				m_tParticleAngles;
	Fvector3				m_tParticleVelocity;
	bool				m_bAutoRemove;

	inline					CScriptParticleAction	();
	inline					CScriptParticleAction	(const char* caPartcileToRun, const char* caBoneName, const CParticleParams &tParticleParams = CParticleParams(), bool bAutoRemove = false);
	inline					CScriptParticleAction	(const char* caPartcileToRun, const CParticleParams &tParticleParams = CParticleParams(), bool bAutoRemove = false);
	virtual				~CScriptParticleAction	();
			void		SetParticle				(const char* caParticleToRun, bool bAutoRemove);
	inline		void		SetPosition				(const Fvector3& tPosition);
	inline		void		SetBone					(const char* caBoneName);
	inline		void		SetAngles				(const Fvector3& tAngleOffset);
	inline		void		SetVelocity				(const Fvector3& tVelocity);
	inline		void		initialize				();

	static void script_register(lua_State*);
};

add_to_type_list(CScriptParticleAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptParticleAction)

#include "script_particle_action_inline.h"
