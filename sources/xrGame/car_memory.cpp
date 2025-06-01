////////////////////////////////////////////////////////////////////////////
//	Module 		: car_memory.cpp
//	Created 	: 11.06.2007
//  Modified 	: 11.06.2007
//	Author		: Dmitriy Iassenev
//	Description : car memory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "car_memory.h"
#include "car.h"
#include "Actor.h"

car_memory::car_memory					(CCar *object) :
	inherited		(
		object,
		100
	),
	m_object		(object)
{
	VERIFY				(m_object);
	m_view_position.set	(0,0,0);
	m_view_direction.set(0,0,1);
	m_view_normal.set	(0,1,0);
	m_far_plane			= 1.0f;
}

void car_memory::reload(pcstr section)
{
	inherited::reload	(section);
	m_fov_deg			= pSettings->r_float(section,"view_fov_deg");
	m_aspect			= pSettings->r_float(section,"view_aspect");
	m_far_plane			= pSettings->r_float(section,"view_far_plane");
}

BOOL car_memory::feel_vision_isRelevant	(CObject *object)
{
	return (NULL!=smart_cast<CActor*>(object));
//.	return			(FALSE);
}

void car_memory::camera					(
	fVector3& position,
	fVector3& direction,
	fVector3& normal,
	f32& field_of_view,
	f32& aspect_ratio,
	f32& near_plane,
	f32& far_plane
	)
{
	position		= m_view_position;
	direction		= m_view_direction;
	normal			= m_view_normal;

	field_of_view	= deg2rad(m_fov_deg);
	aspect_ratio	= m_aspect;
	near_plane		= .1f;
	far_plane		= m_far_plane;
}

void car_memory::set_camera(
						const fVector3& position,
						const fVector3& direction,
						const fVector3& normal
					)
{
	m_view_position		= position;
	m_view_direction	= direction;
	m_view_normal		= normal;
}
