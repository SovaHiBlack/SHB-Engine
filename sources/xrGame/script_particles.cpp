////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_particles.h"
#include "..\XR_3DA\objectanimator.h"

CScriptParticlesCustom::CScriptParticlesCustom(CScriptParticles* owner, pcstr caParticlesName):CParticlesObject(caParticlesName,FALSE,true)
{
//	Msg							("CScriptParticlesCustom: 0x%08x",*(int*)&owner);
	m_owner						= owner;
	m_animator					= 0;
}

CScriptParticlesCustom::~CScriptParticlesCustom()
{
//	Msg							("~CScriptParticlesCustom: 0x%08x",*(int*)&m_owner);
	xr_delete					(m_animator);
}

void CScriptParticlesCustom::PSI_internal_delete()
{
	m_owner->m_particles					= NULL;
	CParticlesObject::PSI_internal_delete	();
}

void CScriptParticlesCustom::PSI_destroy()
{
	m_owner->m_particles			= NULL;
	CParticlesObject::PSI_destroy	();
}

void CScriptParticlesCustom::shedule_Update(u32 _dt)
{
	CParticlesObject::shedule_Update(_dt);
	if (m_animator){
		f32 dt				= f32(_dt)/1000.f;
		Fvector prev_pos		= m_animator->XFORM().c;
		m_animator->Update		(dt);
		Fvector vel;
		vel.sub					(m_animator->XFORM().c,prev_pos).div(dt);
		UpdateParent			(m_animator->XFORM(),vel);
	}
}

void CScriptParticlesCustom::LoadPath(pcstr caPathName)
{
	if (!m_animator) m_animator	= xr_new<CObjectAnimator>();
	if ((0==m_animator->Name())||(0!=xr_strcmp(m_animator->Name(),caPathName))){
		m_animator->Clear		();
		m_animator->Load		(caPathName);
	}
}

void CScriptParticlesCustom::StartPath(bool looped)
{
	VERIFY						(m_animator);
	m_animator->Play			(looped);
}

void CScriptParticlesCustom::PausePath(bool val)
{
	VERIFY						(m_animator);
	m_animator->Pause			(val);
}

void CScriptParticlesCustom::StopPath()
{
	VERIFY						(m_animator);
	m_animator->Stop			();
}

CScriptParticles::CScriptParticles(pcstr caParticlesName)
{
	m_particles					= xr_new<CScriptParticlesCustom>(this, caParticlesName);
}

CScriptParticles::~CScriptParticles()
{
	if(m_particles)
	{
		// destroy particles
		m_particles->PSI_destroy	();
		m_particles					= 0;
	}
}

void CScriptParticles::Play()
{
	VERIFY						(m_particles);
	m_particles->Play			();
}

void CScriptParticles::PlayAtPos(const Fvector &position)
{
	VERIFY						(m_particles);
	m_particles->play_at_pos	(position);
}

void CScriptParticles::Stop		()
{
	VERIFY						(m_particles);
	m_particles->Stop			(FALSE);
}

void CScriptParticles::StopDeffered()
{
	VERIFY						(m_particles);
	m_particles->Stop			(TRUE);
}

void CScriptParticles::MoveTo	(const Fvector &pos, const Fvector& vel)
{
	VERIFY						(m_particles);
	fMatrix4x4						XF;
	XF.translate				(pos);
	m_particles->UpdateParent	(XF,vel);
}

bool CScriptParticles::IsPlaying() const
{
	VERIFY						(m_particles);
	return m_particles->IsPlaying();
}

bool CScriptParticles::IsLooped	() const
{
	VERIFY						(m_particles);
	return m_particles->IsLooped();
}

void CScriptParticles::LoadPath(pcstr caPathName)
{
	VERIFY						(m_particles);
	m_particles->LoadPath		(caPathName);
}

void CScriptParticles::StartPath(bool looped)
{
	m_particles->StartPath		(looped);
}

void CScriptParticles::StopPath	()
{
	m_particles->StopPath		();
}

void CScriptParticles::PausePath(bool val)
{
	m_particles->PausePath		(val);
}
using namespace luabind;

#pragma optimize("s",on)
void CScriptParticles::script_register(lua_State* L)
{
	module(L)
		[
			class_<CScriptParticles>("particles_object")
			.def(constructor<pcstr>( ))
		.def("play", &CScriptParticles::Play)
		.def("play_at_pos", &CScriptParticles::PlayAtPos)
		.def("stop", &CScriptParticles::Stop)
		.def("stop_deffered", &CScriptParticles::StopDeffered)

		.def("playing", &CScriptParticles::IsPlaying)
		.def("looped", &CScriptParticles::IsLooped)

		.def("move_to", &CScriptParticles::MoveTo)

		.def("load_path", &CScriptParticles::LoadPath)
		.def("start_path", &CScriptParticles::StartPath)
		.def("stop_path", &CScriptParticles::StopPath)
		.def("pause_path", &CScriptParticles::PausePath)
		];
}
