////////////////////////////////////////////////////////////////////////////
//	Module 		: car_memory.h
//	Created 	: 11.06.2007
//  Modified 	: 11.06.2007
//	Author		: Dmitriy Iassenev
//	Description : car memory
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "vision_client.h"

class CCar;

class car_memory : public vision_client {
private:
	typedef vision_client					inherited;

private:
	CCar			*m_object;
	f32				m_fov_deg;
	f32				m_aspect;
	f32				m_far_plane;
	fVector3			m_view_position;
	fVector3			m_view_direction;
	fVector3			m_view_normal;

public:
					car_memory				(CCar *object);

	virtual	void	reload					(pcstr section);

	virtual	BOOL	feel_vision_isRelevant	(CObject *object);
	virtual	void	camera					(
		fVector3& position,
		fVector3& direction,
		fVector3& normal,
		f32& field_of_view,
		f32& aspect_ratio,
		f32& near_plane,
		f32& far_plane
					);
			void	set_camera(
						const Fvector &position,
						const Fvector &direction,
						const Fvector &normal
					);
};
