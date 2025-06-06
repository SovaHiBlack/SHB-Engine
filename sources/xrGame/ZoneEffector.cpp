#include "stdafx.h"
#include "ZoneEffector.h"
#include "level.h"
#include "clsid_game.h"
#include "..\XR_3DA\Object.h"
#include "..\XR_3DA\cameramanager.h"
#include "Actor.h"
#include "ActorEffector.h"
#include "postprocessanimator.h"

CZoneEffector::CZoneEffector( )
{
	radius = 1.0f;
	m_pp_effector = NULL;
	m_pActor = NULL;
	m_factor = 0.1f;
}

CZoneEffector::~CZoneEffector( )
{
	Stop( );
}

void CZoneEffector::Load(pcstr section)
{
	VERIFY2(pSettings->line_exist(section, "ppe_file"), section);
	m_pp_fname = pSettings->r_string(section, "ppe_file");
	r_min_perc = pSettings->r_float(section, "radius_min");
	r_max_perc = pSettings->r_float(section, "radius_max");
	VERIFY(r_min_perc <= r_max_perc);
}

void CZoneEffector::Activate( )
{
	m_pActor = smart_cast<CActor*>(Level( ).CurrentEntity( ));
	if (!m_pActor)
	{
		return;
	}

	m_pp_effector = xr_new<CPostprocessAnimatorLerp>( );
	m_pp_effector->SetType(EEffectorPPType(u32(u64(this) & u32(-1))));
	m_pp_effector->SetCyclic(true);
	m_pp_effector->SetFactorFunc(GET_KOEFF_FUNC(this, &CZoneEffector::GetFactor));
	m_pp_effector->Load(*m_pp_fname);
	m_pActor->Cameras( ).AddPPEffector(m_pp_effector);
}

void CZoneEffector::Stop( )
{
	if (!m_pp_effector)
	{
		return;
	}

	m_pActor->Cameras( ).RemovePPEffector(EEffectorPPType(u32(u64(this) & u32(-1))));
	m_pp_effector = NULL;
	m_pActor = NULL;
}

void CZoneEffector::Update(f32 dist)
{
	f32 min_r = radius * r_min_perc;
	f32 max_r = radius * r_max_perc;

	bool camera_on_actor = (Level( ).CurrentEntity( ) && (Level( ).CurrentEntity( )->CLS_ID == CLSID_OBJECT_ACTOR));

	if (m_pp_effector)
	{
		if ((dist > max_r) || !camera_on_actor || (m_pActor && !m_pActor->g_Alive( )))	Stop( );
	}
	else
	{
		if ((dist < max_r) && camera_on_actor)	Activate( );
	}

	if (m_pp_effector)
	{
		m_factor = (max_r - dist) / (max_r - min_r);
		clamp(m_factor, 0.01f, 1.0f);
	}
}

f32 CZoneEffector::GetFactor( )
{
	return m_factor;
}

void CZoneEffector::SetRadius(f32 r)
{
	radius = r;
}
