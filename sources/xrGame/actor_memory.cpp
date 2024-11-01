////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_memory.cpp
//	Created 	: 15.09.2005
//  Modified 	: 15.09.2005
//	Author		: Dmitriy Iassenev
//	Description : actor memory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "actor_memory.h"
#include "actor.h"
#include "..\XR_3DA\camerabase.h"
#include "gamepersistent.h"

CActorMemory::CActorMemory					(CActor *actor) :
	inherited		(
		actor,
		100
	),
	m_actor			(actor)
{
	VERIFY			(m_actor);
}

BOOL CActorMemory::feel_vision_isRelevant	(CObject* O)
{
	CEntityAlive	*entity_alive = smart_cast<CEntityAlive*>(O);
	if (!entity_alive)
		return		(FALSE);

	return			(TRUE);
}

void CActorMemory::camera					(
	fVector3& position,
	fVector3& direction,
	fVector3& normal,
	f32& field_of_view,
	f32& aspect_ratio,
	f32& near_plane,
	f32& far_plane
	)
{
	CCameraBase		&camera = *m_actor->cam_Active();
	camera.Get		(position,direction,normal);
	field_of_view	= deg2rad(camera.f_fov);
	aspect_ratio	= camera.f_aspect;
	near_plane		= .1f;
	far_plane		= g_pGamePersistent->Environment().CurrentEnv.far_plane;
}
