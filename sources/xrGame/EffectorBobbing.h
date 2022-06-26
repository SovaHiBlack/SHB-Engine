#pragma once

#include "CameraEffector.h"
#include "..\XR_3DA\cameramanager.h"

class CEffectorBobbing : public CEffectorCam  
{
	f32	fTime;
	Fvector	vAngleAmplitude;
	f32	fYAmplitude;
	f32	fSpeed;

	u32		dwMState;
	f32	fReminderFactor;
	bool	is_limping;
	bool	m_bZoomMode;

	f32	m_fAmplitudeRun;
	f32	m_fAmplitudeWalk;
	f32	m_fAmplitudeLimp;

	f32	m_fSpeedRun;
	f32	m_fSpeedWalk;
	f32	m_fSpeedLimp;

public:
			CEffectorBobbing	();
	virtual ~CEffectorBobbing	();
	virtual	BOOL	Process		(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n, f32& fFov, f32& fFar, f32& fAspect);
	void	SetState			(u32 st, bool limping, bool ZoomMode);
};
