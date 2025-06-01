#include "stdafx.h"
#include "controlled_actor.h"
#include "../../Actor.h"
#include "../../..\XR_3DA\CameraBase.h"
#include "../../xr_level_controller.h"
#include "../../level.h"
#include "ai_monster_utils.h"
#include "../../inventory.h"

#define SPEED_MIN		0.5f 
#define SPEED_MAX		4.f
#define EPS_ANGLE		1 * PI / 180
#define MAX_DIST		PI

void CControlledActor::reinit()
{
	inherited::reinit();
	reset();
}

void CControlledActor::release()
{
	inherited::release();
	reset();
}

void CControlledActor::frame_update()
{
	if (is_controlling() && m_need_turn) {
		update_turn();
	}
}

void CControlledActor::install(CActor *a)
{
	inherited::install(a);
	m_need_turn = true;
}

void CControlledActor::install()
{
	inherited::install();
	m_need_turn = true;
}

void CControlledActor::look_point(const fVector3& point)
{
	m_target_point						= point;
}

void CControlledActor::update_turn()
{
	// get yaw and pitch to target
	f32 cam_target_yaw;
	f32 cam_target_pitch;
	
	fVector3	P;
	fVector3	D;
	fVector3	N;
	m_actor->cam_Active()->Get				(P,D,N);
	fVector3().sub(m_target_point, P).getHP	(cam_target_yaw, cam_target_pitch);

	// get yaw and pitch of current cam direction
	f32								cam_current_yaw;
	f32								cam_current_pitch;
	D.getHP								(cam_current_yaw, cam_current_pitch);	

	// YAW
	f32								speed_factor = angle_difference(cam_current_yaw, cam_target_yaw) / MAX_DIST;
	clamp								(speed_factor, 0.f, 1.f);
	if (speed_factor > 0.5f)			speed_factor = 1.f - speed_factor;
	
	f32								speed;
	if (fsimilar(cam_current_yaw, cam_target_yaw, EPS_ANGLE)) {
		m_turned_yaw	= true;
	} else {
		speed	= SPEED_MIN + speed_factor * (SPEED_MAX - SPEED_MIN);
	
		if (from_right(cam_target_yaw,cam_current_yaw)) 
			m_actor->cam_Active()->Move	(kLEFT,		speed * Device.fTimeDelta);
		else 
			m_actor->cam_Active()->Move	(kRIGHT,	speed * Device.fTimeDelta);
	}

	// PITCH
	speed_factor						= angle_difference(cam_current_pitch, cam_target_pitch) / MAX_DIST;
	clamp								(speed_factor, 0.f, 1.f);
	if (speed_factor > 0.5f)			speed_factor = 1.f - speed_factor;

	if (fsimilar(cam_current_pitch, cam_target_pitch, EPS_ANGLE)) {
		m_turned_pitch	= true;
	} else {
		speed	= SPEED_MIN + speed_factor * (SPEED_MAX - SPEED_MIN);

		if (from_right(cam_target_pitch,cam_current_pitch)) 
			m_actor->cam_Active()->Move	(kDOWN,		speed * Device.fTimeDelta);
		else 
			m_actor->cam_Active()->Move	(kUP,		speed * Device.fTimeDelta);
	}
}

void CControlledActor::reset()
{
	m_turned_yaw		= false;
	m_turned_pitch		= false;
}

bool CControlledActor::is_turning()
{
	return (!m_turned_yaw || !m_turned_pitch);
}

bool CControlledActor::authorized(s32 cmd)
{
	if (cmd == kWPN_1)		return true;
	if (cmd == kWPN_FIRE) {
		if (m_actor->inventory().GetActiveSlot() == 0) return true;
	}

	return false;
}
