#include "stdafx.h"

#include "CameraBase.h"

CCameraBase::CCameraBase(CObject* p, unsigned int flags)
{
	m_Flags.assign		(flags);
	vPosition.set		(0.0f, 0.0f, 0.0f);
	vDirection.set		(0.0f, 0.0f, 1.0f);
	vNormal.set			(0.0f, 1.0f, 0.0f);
	yaw					= 0.0f;
	pitch				= 0.0f;
	roll				= 0.0f;
	lim_yaw.set			(0.0f, 0.0f);
	lim_pitch.set		(0.0f, 0.0f);
	lim_roll.set		(0.0f, 0.0f);
	bClampYaw			= false;
	bClampPitch			= false;
	bClampRoll			= false;
	SetParent			(p);
	f_fov				= 90.0f;
	f_aspect			= 1.0f;
	tag					= 0;
}

CCameraBase::~CCameraBase( )
{ }

void CCameraBase::Load(const char* section)
{
	rot_speed			= pSettings->r_fvector3(section, "rot_speed");
	lim_yaw				= pSettings->r_fvector2(section, "lim_yaw");
	lim_pitch			= pSettings->r_fvector2(section, "lim_pitch");

	bClampPitch			= (0 != lim_pitch[0]) || (0 != lim_pitch[1]);
	if (bClampPitch)
	{
		pitch			= (lim_pitch[0] + lim_pitch[1]) * 0.5f;
	}

	bClampYaw			= (0 != lim_yaw[0]) || (0 != lim_yaw[1]);
	if (bClampYaw)
	{
		yaw				= (lim_yaw[0] + lim_yaw[1]) * 0.5f;
	}
}

inline float AClamp(Fvector2& l, float v)
{
	return (2 * v - l[0] - l[1]) / (l[1] - l[0]);
}
