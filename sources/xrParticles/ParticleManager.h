// ======================================= SHB-Engine© ============================================>>
// Project		: Particles
// Module		: ParticleManager.h
// Author		: Anahoret
// Description	: 
// ======================================= SovaHiBlack© ===========================================

#pragma once
#ifndef PARTICLES_PARTICLEMANAGER_H_INCLUDED
#define PARTICLES_PARTICLEMANAGER_H_INCLUDED
//-------------------------------------------------------------------------------------------------
#include "particle_actions.h"

namespace PAPI
{
	class CParticleManager : public IParticleManager
	{
		// These are static because all threads access the same effects.
		// All accesses to these should be locked.
		DEFINE_VECTOR(ParticleEffect*, ParticleEffectVec, ParticleEffectVecIt);
		DEFINE_VECTOR(ParticleActions*, ParticleActionsVec, ParticleActionsVecIt);
		ParticleEffectVec			effect_vec;
		ParticleActionsVec			alist_vec;

	public:
									CParticleManager	( );
		virtual						~CParticleManager	( );

		// Return an index into the list of particle effects where
		ParticleEffect*				GetEffectPtr		(s32 effect_id);
		ParticleActions*			GetActionListPtr	(s32 alist_id);

		// create&destroy
		virtual s32					CreateEffect		(u32 max_particles);
		virtual void				DestroyEffect		(s32 effect_id);
		virtual s32					CreateActionList	( );
		virtual void				DestroyActionList	(s32 alist_id);

		// control
		virtual void				PlayEffect			(s32 effect_id, s32 alist_id);
		virtual void				StopEffect			(s32 effect_id, s32 alist_id, BOOL deffered = TRUE);

		// update&render
		virtual void				Update				(s32 effect_id, s32 alist_id, f32 dt);
		virtual void				Render				(s32 effect_id);
		virtual void				Transform			(s32 alist_id, const fMatrix4x4& m, const fVector3& velocity);

		// effect
		virtual void				RemoveParticle		(s32 effect_id, u32 p_id);
		virtual void				SetMaxParticles		(s32 effect_id, u32 max_particles);
		virtual void				SetCallback			(s32 effect_id, OnBirthParticleCB b, OnDeadParticleCB d, pVoid owner, u32 param);
		virtual void				GetParticles		(s32 effect_id, Particle*& particles, u32& cnt);
		virtual u32					GetParticlesCount	(s32 effect_id);

		// action
		virtual ParticleAction*		CreateAction		(PActionEnum action_id);
		virtual u32					LoadActions			(s32 alist_id, IReader& R);
		virtual void				SaveActions			(s32 alist_id, IWriter& W);
	};
};
//-------------------------------------------------------------------------------------------------
#endif // ndef PARTICLES_PARTICLEMANAGER_H_INCLUDED
