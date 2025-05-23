//---------------------------------------------------------------------------
#include "stdafx.h"

#include "ParticleManager.h"
#include "particle_effect.h"
#include "particle_actions_collection.h"

using namespace PAPI;

// system
CParticleManager PM;
PARTICLES_API IParticleManager* PAPI::ParticleManager( )
{
	return &PM;
}

// 
CParticleManager::CParticleManager( )
{ }

CParticleManager::~CParticleManager( )
{ }

ParticleEffect* CParticleManager::GetEffectPtr(s32 effect_id)
{
	R_ASSERT(effect_id >= 0 && effect_id < (s32)effect_vec.size( ));
	return effect_vec[effect_id];
}

ParticleActions* CParticleManager::GetActionListPtr(s32 a_list_num)
{
	R_ASSERT(a_list_num >= 0 && a_list_num < (s32)alist_vec.size( ));
	return alist_vec[a_list_num];
}

// create
s32 CParticleManager::CreateEffect(u32 max_particles)
{
	s32 eff_id = -1;
	for (s32 i = 0; i < (s32)effect_vec.size( ); i++)
		if (!effect_vec[i])
		{
			eff_id = i; break;
		}

	if (eff_id < 0)
	{
		// Couldn't find a big enough gap. Reallocate.
		eff_id = effect_vec.size( );
		effect_vec.push_back(0);
	}

	effect_vec[eff_id] = xr_new<ParticleEffect>(max_particles);

	return eff_id;
}
void CParticleManager::DestroyEffect(s32 effect_id)
{
	R_ASSERT(effect_id >= 0 && effect_id < (s32)effect_vec.size( ));
	xr_delete(effect_vec[effect_id]);
}
s32	CParticleManager::CreateActionList( )
{
	s32 list_id = -1;
	for (s32 i = 0; i < (s32)alist_vec.size( ); i++)
		if (!alist_vec[i])
		{
			list_id = i; break;
		}

	if (list_id < 0)
	{
		// Couldn't find a big enough gap. Reallocate.
		list_id = alist_vec.size( );
		alist_vec.push_back(0);
	}

	alist_vec[list_id] = xr_new<ParticleActions>( );

	return list_id;
}
void CParticleManager::DestroyActionList(s32 alist_id)
{
	R_ASSERT(alist_id >= 0 && alist_id < (s32)alist_vec.size( ));
	xr_delete(alist_vec[alist_id]);
}

// control
void CParticleManager::PlayEffect(s32 effect_id, s32 alist_id)
{
	// effect
//    ParticleEffect* pe		= GetEffectPtr(effect_id);
	// Execute the specified action list.
	ParticleActions* pa = GetActionListPtr(alist_id);
	if (pa == NULL)		return; // ERROR
	// Step through all the actions in the action list.
	for (PAVecIt it = pa->begin( ); it != pa->end( ); it++)
	{
		switch ((*it)->type)
		{
			case PASourceID: 	static_cast<PASource*>(*it)->m_Flags.set(PASource::flSilent, FALSE); break;
			case PAExplosionID: static_cast<PAExplosion*>(*it)->age = 0.f; break;
			case PATurbulenceID:static_cast<PATurbulence*>(*it)->age = 0.f; break;
		}
	}
}

void CParticleManager::StopEffect(s32 effect_id, s32 alist_id, BOOL deffered)
{
	// Execute the specified action list.
	ParticleActions* pa = GetActionListPtr(alist_id);
	if (pa == NULL)		return; // ERROR
	// Step through all the actions in the action list.
	for (PAVecIt it = pa->begin( ); it != pa->end( ); it++)
	{
		switch ((*it)->type)
		{
			case PASourceID: static_cast<PASource*>(*it)->m_Flags.set(PASource::flSilent, TRUE);		break;
		}
	}
	if (!deffered)
	{
		// effect
		ParticleEffect* pe = GetEffectPtr(effect_id);
		pe->p_count = 0;
	}
}

// update&render
void CParticleManager::Update(s32 effect_id, s32 alist_id, f32 dt)
{
	ParticleEffect* pe = GetEffectPtr(effect_id);
	ParticleActions* pa = GetActionListPtr(alist_id);
	// Step through all the actions in the action list.
	for (PAVecIt it = pa->begin( ); it != pa->end( ); it++)
		(*it)->Execute(pe, dt);
}
void CParticleManager::Render(s32 effect_id)
{
	//    ParticleEffect* pe	= GetEffectPtr(effect_id);
}
void CParticleManager::Transform(s32 alist_id, const fMatrix4x4& full, const fVector3& vel)
{
	// Execute the specified action list.
	ParticleActions* pa = GetActionListPtr(alist_id);

	if (pa == NULL)
	{
		return;
	} // ERROR

	fMatrix4x4 mT;
	mT.translate(full.c);

	// Step through all the actions in the action list.
	for (PAVecIt it = pa->begin( ); it != pa->end( ); it++)
	{
		BOOL r = (*it)->m_Flags.is(ParticleAction::ALLOW_ROTATE);
		const fMatrix4x4& m = r ? full : mT;
		(*it)->Transform(m);
		switch ((*it)->type)
		{
			case PASourceID:
			{
				static_cast<PASource*>(*it)->parent_vel = pVector(vel.x, vel.y, vel.z) * static_cast<PASource*>(*it)->parent_motion;
			}
			break;
		}
	}
}

// effect
void CParticleManager::RemoveParticle(s32 effect_id, u32 p_id)
{
	ParticleEffect* pe = GetEffectPtr(effect_id);
	pe->Remove(p_id);
}
void CParticleManager::SetMaxParticles(s32 effect_id, u32 max_particles)
{
	ParticleEffect* pe = GetEffectPtr(effect_id);
	pe->Resize(max_particles);
}
void CParticleManager::SetCallback(s32 effect_id, OnBirthParticleCB b, OnDeadParticleCB d, pVoid owner, u32 param)
{
	ParticleEffect* pe = GetEffectPtr(effect_id);
	pe->b_cb = b;
	pe->d_cb = d;
	pe->owner = owner;
	pe->param = param;
}
void CParticleManager::GetParticles(s32 effect_id, Particle*& particles, u32& cnt)
{
	ParticleEffect* pe = GetEffectPtr(effect_id);
	particles = pe->particles;
	cnt = pe->p_count;
}
u32	CParticleManager::GetParticlesCount(s32 effect_id)
{
	ParticleEffect* pe = GetEffectPtr(effect_id);
	return pe->p_count;
}

// action
ParticleAction* CParticleManager::CreateAction(PActionEnum type)
{
	ParticleAction* pa = 0;
	switch (type)
	{
		case PAAvoidID:				pa = xr_new<PAAvoid>( );				break;
		case PABounceID:    		pa = xr_new<PABounce>( );			break;
		case PACopyVertexBID:    	pa = xr_new<PACopyVertexB>( );		break;
		case PADampingID:    		pa = xr_new<PADamping>( );			break;
		case PAExplosionID:    		pa = xr_new<PAExplosion>( );			break;
		case PAFollowID:    		pa = xr_new<PAFollow>( );			break;
		case PAGravitateID:    		pa = xr_new<PAGravitate>( );			break;
		case PAGravityID:    		pa = xr_new<PAGravity>( );			break;
		case PAJetID:    			pa = xr_new<PAJet>( );				break;
		case PAKillOldID:    		pa = xr_new<PAKillOld>( );			break;
		case PAMatchVelocityID:    	pa = xr_new<PAMatchVelocity>( );		break;
		case PAMoveID:    			pa = xr_new<PAMove>( );				break;
		case PAOrbitLineID:    		pa = xr_new<PAOrbitLine>( );			break;
		case PAOrbitPointID:    	pa = xr_new<PAOrbitPoint>( );		break;
		case PARandomAccelID:    	pa = xr_new<PARandomAccel>( );		break;
		case PARandomDisplaceID:    pa = xr_new<PARandomDisplace>( );	break;
		case PARandomVelocityID:    pa = xr_new<PARandomVelocity>( );	break;
		case PARestoreID:    		pa = xr_new<PARestore>( );			break;
		case PASinkID:    			pa = xr_new<PASink>( );				break;
		case PASinkVelocityID:    	pa = xr_new<PASinkVelocity>( );		break;
		case PASourceID:    		pa = xr_new<PASource>( );			break;
		case PASpeedLimitID:    	pa = xr_new<PASpeedLimit>( );		break;
		case PATargetColorID:    	pa = xr_new<PATargetColor>( );		break;
		case PATargetSizeID:
		{
			pa = xr_new<PATargetSize>( );
		}
		break;
		case PATargetRotateID:
		{
			pa = xr_new<PATargetRotate>( );
		}
		break;
		case PATargetRotateDID:
		{
			pa = xr_new<PATargetRotate>( );
		}
		break;
		case PATargetVelocityID:
		{
			pa = xr_new<PATargetVelocity>( );
		}
		break;
		case PATargetVelocityDID:
		{
			pa = xr_new<PATargetVelocity>( );
		}
		break;
		case PAVortexID:
		{
			pa = xr_new<PAVortex>( );
		}
		break;
		case PATurbulenceID:
		{
			pa = xr_new<PATurbulence>( );
		}
		break;
		case PAScatterID:
		{
			pa = xr_new<PAScatter>( );
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}

	pa->type = type;
	return pa;
}

u32 CParticleManager::LoadActions(s32 alist_id, IReader& R)
{
	// Execute the specified action list.
	ParticleActions* pa = GetActionListPtr(alist_id);
	pa->clear( );
	if (R.length( ))
	{
		u32 cnt = R.r_u32( );
		for (u32 k = 0; k < cnt; k++)
		{
			ParticleAction* act = CreateAction((PActionEnum)R.r_u32( ));
			act->Load(R);
			pa->append(act);
		}
	}

	return pa->size( );
}

void CParticleManager::SaveActions(s32 alist_id, IWriter& W)
{
	// Execute the specified action list.
	ParticleActions* pa = GetActionListPtr(alist_id);
	W.w_u32(pa->size( ));
	for (PAVecIt it = pa->begin( ); it != pa->end( ); it++)
	{
		(*it)->Save(W);
	}
}
