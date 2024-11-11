// CameraBase.cpp: implementation of the CCameraBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "igame_level.h"

#include "CameraBase.h"

CCameraBase::CCameraBase(CObject* p, u32 flags)
{
	m_Flags.assign(flags);
	vPosition.set(0.0f, 0.0f, 0.0f);
	vDirection.set(0.0f, 0.0f, 1.0f);
	vNormal.set(0.0f, 1.0f, 0.0f);
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	lim_yaw.set(0.0f, 0.0f);
	lim_pitch.set(0.0f, 0.0f);
	lim_roll.set(0.0f, 0.0f);
	bClampYaw = false;
	bClampPitch = false;
	bClampRoll = false;
	SetParent(p);
	f_fov = 90.0f;
	f_aspect = 1.0f;
	tag = 0;
}

CCameraBase::~CCameraBase( )
{ }

void CCameraBase::Load(pcstr section)
{
	rot_speed = pSettings->r_fvector3(section, "rot_speed");

	lim_yaw = pSettings->r_fvector2(section, "lim_yaw");
	lim_pitch = pSettings->r_fvector2(section, "lim_pitch");

	bClampPitch = (0 != lim_pitch[0]) || (0 != lim_pitch[1]);
	bClampYaw = (0 != lim_yaw[0]) || (0 != lim_yaw[1]);

	if (bClampPitch)
	{
		pitch = (lim_pitch[0] + lim_pitch[1]) * 0.5f;
	}

	if (bClampYaw)
	{
		yaw = (lim_yaw[0] + lim_yaw[1]) * 0.5f;
	}
}

IC f32 AClamp(fVector2& l, f32 v)
{
	return ((2 * v - l[0] - l[1]) / (l[1] - l[0]));
}

f32 CCameraBase::CheckLimYaw( )
{
	if (bClampYaw)
	{
		return AClamp(lim_yaw, yaw);
	}
	else
	{
		return 0.0f;
	}
}

f32 CCameraBase::CheckLimPitch( )
{
	if (bClampYaw)
	{
		return AClamp(lim_pitch, pitch);
	}
	else
	{
		return 0.0f;
	}
}

f32 CCameraBase::CheckLimRoll( )
{
	if (bClampYaw)
	{
		return AClamp(lim_roll, roll);
	}
	else
	{
		return 0.0f;
	}
}
