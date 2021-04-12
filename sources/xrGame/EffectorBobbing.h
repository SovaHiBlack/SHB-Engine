#pragma once

#include "CameraEffector.h"
#include "..\ENGINE\CameraManager.h"

class CEffectorBobbing : public CEffectorCam
{
	float									fTime;
	Fvector3								vAngleAmplitude;
	float									fYAmplitude;
	float									fSpeed;

	u32										dwMState;
	float									fReminderFactor;
	bool									is_limping;
	bool									m_bZoomMode;

	float									m_fAmplitudeRun;
	float									m_fAmplitudeWalk;
	float									m_fAmplitudeLimp;

	float									m_fSpeedRun;
	float									m_fSpeedWalk;
	float									m_fSpeedLimp;

public:
					CEffectorBobbing		( );
	virtual			~CEffectorBobbing		( );
	virtual BOOL	Process					(Fvector3& delta_p, Fvector3& delta_d, Fvector3& delta_n, float& fFov, float& fFar, float& fAspect);
	void			SetState				(u32 st, bool limping, bool ZoomMode);
};
