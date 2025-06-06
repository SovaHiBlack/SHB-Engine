#include "stdafx.h"

#include "CameraFirstEye.h"
#include "xr_level_controller.h"
#include "..\XR_3DA\Object.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraFirstEye::CCameraFirstEye(CObject* p, u32 flags) : CCameraBase(p, flags)
{ }

CCameraFirstEye::~CCameraFirstEye( )
{ }

void CCameraFirstEye::Load(pcstr section)
{
	inherited::Load(section);
	style = csFirstEye;
}

void CCameraFirstEye::Update(fVector3& point, fVector3& noise_dangle)
{
	fMatrix4x4 mR;
	fMatrix4x4 R;
	fMatrix4x4 rX;
	fMatrix4x4 rY;
	fMatrix4x4 rZ;
	rX.rotateX(noise_dangle.x);
	rY.rotateY(-noise_dangle.y);
	rZ.rotateZ(noise_dangle.z);
	R.mul_43(rY, rX);
	R.mulB_43(rZ);

	mR.identity( );
	fQuaternion Q;
	Q.rotationYawPitchRoll(roll, yaw, pitch);
	mR.rotation(Q);
	mR.transpose( );
	mR.mulB_43(R);

	vDirection.set(mR.k);
	vNormal.set(mR.j);

	if (m_Flags.is(flRelativeLink))
	{
		parent->XFORM( ).transform_dir(vDirection);
		parent->XFORM( ).transform_dir(vNormal);
	}

	vPosition.set(point);
}

void CCameraFirstEye::Move(s32 cmd, f32 val, f32 factor)
{
	if (bClampPitch)
	{
		while (pitch < lim_pitch[0])
		{
			pitch += PI_MUL_2;
		}

		while (pitch > lim_pitch[1])
		{
			pitch -= PI_MUL_2;
		}
	}

	switch (cmd)
	{
		case kDOWN:
		{
			pitch -= val ? val : (rot_speed.y * Device.fTimeDelta / factor);
		}
		break;
		case kUP:
		{
			pitch += val ? val : (rot_speed.y * Device.fTimeDelta / factor);
		}
		break;
		case kLEFT:
		{
			yaw -= val ? val : (rot_speed.x * Device.fTimeDelta / factor);
		}
		break;
		case kRIGHT:
		{
			yaw += val ? val : (rot_speed.x * Device.fTimeDelta / factor);
		}
		break;
	}

	if (bClampYaw)
	{
		clamp(yaw, lim_yaw[0], lim_yaw[1]);
	}

	if (bClampPitch)
	{
		clamp(pitch, lim_pitch[0], lim_pitch[1]);
	}
}

void CCameraFirstEye::OnActivate(CCameraBase* old_cam)
{
	if (old_cam && (m_Flags.is(flRelativeLink) == old_cam->m_Flags.is(flRelativeLink)))
	{
		yaw = (old_cam)->yaw;
	}
}
