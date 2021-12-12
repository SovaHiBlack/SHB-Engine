#pragma once

#include "particle_actions.h"

namespace PAPI
{
	class CParticleManager : public IParticleManager
	{
		// These are static because all threads access the same effects
		// All accesses to these should be locked
		using ParticleEffectVec							= xr_vector<SParticleEffect*>;
		ParticleEffectVec								effect_vec;

		using ParticleActionsVec						= xr_vector<CParticleActions*>;
		ParticleActionsVec								alist_vec;

	public:
									CParticleManager	( );
		virtual						~CParticleManager	( );

		// Return an index into the list of particle effects where
		SParticleEffect*			GetEffectPtr		(int effect_id);
		CParticleActions*			GetActionListPtr	(int alist_id);

		// Create&Destroy
		virtual int					CreateEffect		(unsigned int max_particles);
		virtual void				DestroyEffect		(int effect_id);
		virtual int					CreateActionList	( );
		virtual void				DestroyActionList	(int alist_id);

		// Control
		virtual void				PlayEffect			(int effect_id, int alist_id);
		virtual void				StopEffect			(int effect_id, int alist_id, BOOL deffered = TRUE);

		// Update&Render
		virtual void				Update				(int effect_id, int alist_id, float dt);
		virtual void				Render				(int effect_id);
		virtual void				Transform			(int alist_id, const Fmatrix& m, const Fvector3& velocity);

		// Effect
		virtual void				RemoveParticle		(int effect_id, unsigned int p_id);
		virtual void				SetMaxParticles		(int effect_id, unsigned int max_particles);
		virtual void				SetCallback			(int effect_id, OnBirthParticleCB b, OnDeadParticleCB d, void* owner, unsigned int param);
		virtual void				GetParticles		(int effect_id, SParticle*& particles, unsigned int& cnt);
		virtual unsigned int		GetParticlesCount	(int effect_id);

		// Action
		virtual SParticleAction*	CreateAction		(EParticleAction action_id);
		virtual unsigned int		LoadActions			(int alist_id, IReader& R);
		virtual void				SaveActions			(int alist_id, IWriter& W);
	};
};
