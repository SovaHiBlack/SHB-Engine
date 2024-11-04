//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"

#include "ParticlesObject.h"
#include "..\XR_3DA\defines.h"
#include "..\XR_3DA\fbasicvisual.h"
#include "..\XR_3DA\ParticleCustom.h"
#include "..\XR_3DA\render.h"
#include "..\XR_3DA\IGame_Persistent.h"

const fVector3 zero_vel		= {0.0f,0.0f,0.0f};

CParticlesObject::CParticlesObject(pcstr p_name, BOOL bAutoRemove, bool destroy_on_game_load) : inherited(destroy_on_game_load)
{
	Init					(p_name,0,bAutoRemove);
}

void CParticlesObject::Init	(pcstr p_name, IRender_Sector* S, BOOL bAutoRemove)
{
	m_bLooped				= false;
	m_bStopping				= false;
	m_bAutoRemove			= bAutoRemove;
	f32 time_limit		= 0.0f;

	// create visual
	renderable.visual		= Render->model_CreateParticles(p_name);
	VERIFY					(renderable.visual);
	IParticleCustom* V		= smart_cast<IParticleCustom*>(renderable.visual);  VERIFY(V);
	time_limit				= V->GetTimeLimit();

	if(time_limit > 0.f)
	{
		m_iLifeTime			= iFloor(time_limit*1000.f);
	}
	else
	{
		if(bAutoRemove)
		{
			R_ASSERT3			(!m_bAutoRemove,"Can't set auto-remove flag for looped particle system.",p_name);
		}
		else
		{
			m_iLifeTime = 0;
			m_bLooped = true;
		}
	}

	// spatial
	spatial.type			= 0;
	spatial.sector			= S;
	
	// sheduled
	shedule.t_min			= 20;
	shedule.t_max			= 50;
	shedule_register		();

	dwLastTime				= Device.dwTimeGlobal;
	mt_dt					= 0;
}

//----------------------------------------------------
CParticlesObject::~CParticlesObject()
{
	VERIFY					(0==mt_dt);

//	we do not need this since CPS_Instance does it
//	shedule_unregister		();
}

void CParticlesObject::UpdateSpatial()
{
	// spatial	(+ workaround occasional bug inside particle-system)
	if (_valid(renderable.visual->vis.sphere))
	{
		fVector3	P;
		f32	R;
		renderable.xform.transform_tiny	(P,renderable.visual->vis.sphere.P);
		R								= renderable.visual->vis.sphere.R;
		if (0==spatial.type)	{
			// First 'valid' update - register
			spatial.type			= STYPE_RENDERABLE;
			spatial.sphere.set		(P,R);
			spatial_register		();
		} else {
			BOOL	bMove			= FALSE;
			if		(!P.similar(spatial.sphere.P, EPSILON_3 *10.f))		bMove	= TRUE;
			if		(!fsimilar(R,spatial.sphere.R,0.15f))			bMove	= TRUE;
			if		(bMove)			{
				spatial.sphere.set	(P, R);
				spatial_move		();
			}
		}
	}
}

const shared_str CParticlesObject::Name()
{
	IParticleCustom* V	= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	return (V) ? V->Name() : "";
}

//----------------------------------------------------
void CParticlesObject::Play		()
{
	IParticleCustom* V			= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->Play						();
	dwLastTime					= Device.dwTimeGlobal-33ul;
	mt_dt						= 0;
	PerformAllTheWork			(0);
	m_bStopping					= false;
}

void CParticlesObject::play_at_pos(const fVector3& pos, BOOL xform)
{
	IParticleCustom* V			= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	fMatrix4x4 m;
	m.translate		(pos);
	V->UpdateParent				(m,zero_vel,xform);
	V->Play						();
	dwLastTime					= Device.dwTimeGlobal-33ul;
	mt_dt						= 0;
	PerformAllTheWork			(0);
	m_bStopping					= false;
}

void CParticlesObject::Stop		(BOOL bDefferedStop)
{
	IParticleCustom* V			= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->Stop						(bDefferedStop);
	m_bStopping					= true;
}

void CParticlesObject::shedule_Update	(u32 _dt)
{
	inherited::shedule_Update		(_dt);

	// Update
	if (m_bDead)					return;
	u32 dt							= Device.dwTimeGlobal - dwLastTime;
	if (dt)							{
		if (0){//.psDeviceFlags.test(mtParticles))	{    //. AlexMX comment this line// NO UNCOMMENT - DON'T WORK PROPERLY
			mt_dt					= dt;
			fastdelegate::FastDelegate0<>		delegate	(this,&CParticlesObject::PerformAllTheWork_mt);
			Device.seqParallel.push_back		(delegate);
		} else {
			mt_dt					= 0;
			IParticleCustom* V		= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
			V->OnFrame				(dt);
		}
		dwLastTime					= Device.dwTimeGlobal;
	}
	UpdateSpatial					();
}

void CParticlesObject::PerformAllTheWork(u32 _dt)
{
	// Update
	u32 dt							= Device.dwTimeGlobal - dwLastTime;
	if (dt)							{
		IParticleCustom* V		= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
		V->OnFrame				(dt);
		dwLastTime				= Device.dwTimeGlobal;
	}
	UpdateSpatial					();
}

void CParticlesObject::PerformAllTheWork_mt()
{
	if (0==mt_dt)			return;	//???
	IParticleCustom* V		= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->OnFrame				(mt_dt);
	mt_dt					= 0;
}

void CParticlesObject::SetXFORM			(const fMatrix4x4& m)
{
	IParticleCustom* V	= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->UpdateParent		(m,zero_vel,TRUE);
	renderable.xform.set(m);
	UpdateSpatial		();
}

void CParticlesObject::UpdateParent		(const fMatrix4x4& m, const fVector3& vel)
{
	IParticleCustom* V	= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->UpdateParent		(m,vel,FALSE);
	UpdateSpatial		();
}

fVector3& CParticlesObject::Position		()
{
	return renderable.visual->vis.sphere.P;
}

f32 CParticlesObject::shedule_Scale		()
{ 
	return Device.vCameraPosition.distance_to(Position())/200.f; 
}

void CParticlesObject::renderable_Render	()
{
	VERIFY					(renderable.visual);
	u32 dt					= Device.dwTimeGlobal - dwLastTime;
	if (dt){
		IParticleCustom* V	= smart_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
		V->OnFrame			(dt);
		dwLastTime			= Device.dwTimeGlobal;
	}
	::Render->set_Transform	(&renderable.xform);
	::Render->add_Visual	(renderable.visual);
}

bool CParticlesObject::IsAutoRemove			()
{
	if(m_bAutoRemove) return true;
	else return false;
}

void CParticlesObject::SetAutoRemove		(bool auto_remove)
{
	VERIFY(m_bStopping || !IsLooped());
	m_bAutoRemove = auto_remove;
}

//играются ли партиклы, отличается от PSI_Alive, тем что после
//остановки Stop партиклы могут еще доигрывать анимацию IsPlaying = true
bool CParticlesObject::IsPlaying()
{
	IParticleCustom* V	= smart_cast<IParticleCustom*>(renderable.visual); 
	VERIFY(V);
	return !!V->IsPlaying();
}
