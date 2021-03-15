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
	float			m_fov_deg;
	float			m_aspect;
	float			m_far_plane;
	Fvector3		m_view_position;
	Fvector3		m_view_direction;
	Fvector3		m_view_normal;

public:
					car_memory				(CCar *object);

	virtual	void	reload					(const char* section);

	virtual	BOOL	feel_vision_isRelevant	(CObject *object);
	virtual	void	camera					(
		Fvector3& position,
		Fvector3& direction,
		Fvector3& normal,
						float &field_of_view,
						float &aspect_ratio,
						float &near_plane,
						float &far_plane
					);
			void	set_camera(
						const Fvector3& position,
						const Fvector3& direction,
						const Fvector3& normal
					);
};
