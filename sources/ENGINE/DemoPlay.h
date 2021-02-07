#pragma once

#include "effector.h"

class COMotion;
struct SAnimParams;

class ENGINE_API CDemoPlay : public CEffectorCam
{
	COMotion*						m_pMotion;
	SAnimParams*					m_MParam;

	xr_vector<Fmatrix>				seq;
	int								m_count;
	float							fStartTime;
	float							fSpeed;
	u32								dwCyclesLeft;

	// statistics
	bool							stat_started;
	CTimer							stat_Timer_frame;
	CTimer							stat_Timer_total;
	u32								stat_StartFrame;
	xr_vector<float>				stat_table;

	void			stat_Start		( );
	void			stat_Stop		( );

public:
	virtual BOOL	Process			(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect);

					CDemoPlay		(const char* name, float ms, u32 cycles, float life_time = 60 * 60 * 1000);
	virtual			~CDemoPlay		( );
};
