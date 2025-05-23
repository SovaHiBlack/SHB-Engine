#include "stdafx.h"
#include "custommonster.h"
#include "movement_manager.h"

IC void conv_angle(f32& c)
{
	if (c<0)				c+=PI_MUL_2;
	else if (c>PI_MUL_2)	c-=PI_MUL_2;
}

void CCustomMonster::mk_rotation(fVector3& dir, SRotation& R)
{
	// parse yaw
	fVector3 DYaw;
	DYaw.set(dir.x,0.0f,dir.z); 
	DYaw.normalize_safe();
	clamp(DYaw.x,-0.9999999f,0.9999999f);
	clamp(DYaw.y,-0.9999999f,0.9999999f);
	clamp(DYaw.z,-0.9999999f,0.9999999f);
	if (DYaw.x>=0)	
		R.yaw = acosf(DYaw.z);
	else			
		R.yaw = 2*PI-acosf(DYaw.z);

	// parse pitch
	dir.normalize_safe	();
	R.pitch				= -asinf(dir.y);
}

void CCustomMonster::Exec_Look		(f32 dt )
{
	if (animation_movement_controlled())
		return;

	movement().m_body.current.yaw	= angle_normalize_signed	(movement().m_body.current.yaw);
	movement().m_body.current.pitch	= angle_normalize_signed	(movement().m_body.current.pitch);
	movement().m_body.target.yaw	= angle_normalize_signed	(movement().m_body.target.yaw);
	movement().m_body.target.pitch	= angle_normalize_signed	(movement().m_body.target.pitch);
	
	f32 pitch_speed				= get_custom_pitch_speed(movement().m_body.speed);
	angle_lerp_bounds				(movement().m_body.current.yaw,movement().m_body.target.yaw,movement().m_body.speed,dt);
	angle_lerp_bounds				(movement().m_body.current.pitch,movement().m_body.target.pitch,pitch_speed,dt);

	fVector3 P						= Position();
	XFORM().setHPB					(-NET_Last.o_model,-NET_Last.o_torso.pitch,0);
	Position()						= P;
}
