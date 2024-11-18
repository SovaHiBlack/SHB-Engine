#include "stdafx.h"
#include "EffectorShotX.h"
#include "..\XR_3DA\CameraBase.h"

CCameraShotEffectorX::CCameraShotEffectorX(f32 max_angle, f32 relax_speed, f32 max_angle_horz, f32 step_angle_horz, f32 angle_frac) : CCameraShotEffector(max_angle, relax_speed, max_angle_horz, step_angle_horz, angle_frac)
{ }

CCameraShotEffectorX::~CCameraShotEffectorX( )
{ }

BOOL CCameraShotEffectorX::Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect)
{
	return TRUE;
}

void CCameraShotEffectorX::GetDeltaAngle(fVector3& delta_angle)
{
	delta_angle.x = 0.0f;
	delta_angle.y = 0.0f;
	delta_angle.z = 0.0f;
}

void CCameraShotEffectorX::Shot(f32 angle)
{
	f32 fAC_Old = fAngleVert;
	f32 fAH_Old = fAngleHorz;
	inherited::Shot(angle);
	f32 dAC = fAngleVert - fAC_Old;
	f32 dAH = fAngleHorz - fAH_Old;

	UpdateActorCamera(-dAC, -dAH);
}

void CCameraShotEffectorX::Clear( )
{
	fAngleVert = -EPSILON_7;
	fAngleHorz = 0.0f;
}

void CCameraShotEffectorX::UpdateActorCamera(f32 dPitch, f32 dYaw)
{
	if (!m_pActor)
	{
		return;
	}

	CCameraBase* pACam = m_pActor->cam_FirstEye( );
	if (!pACam)
	{
		return;
	}

	if (pACam->bClampPitch)
	{
		while (pACam->pitch < pACam->lim_pitch[0])
		{
			pACam->pitch += PI_MUL_2;
		}

		while (pACam->pitch > pACam->lim_pitch[1])
		{
			pACam->pitch -= PI_MUL_2;
		}
	}

	pACam->pitch += dPitch;
	pACam->yaw += dYaw;

	if (pACam->bClampYaw)
	{
		clamp(pACam->yaw, pACam->lim_yaw[0], pACam->lim_yaw[1]);
	}

	if (pACam->bClampPitch)
	{
		clamp(pACam->pitch, pACam->lim_pitch[0], pACam->lim_pitch[1]);
	}
}
