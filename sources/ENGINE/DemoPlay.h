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
	U32								dwCyclesLeft;

	// statistics
	bool							stat_started;
	CTimer							stat_Timer_frame;
	CTimer							stat_Timer_total;
	U32								stat_StartFrame;
	xr_vector<float>				stat_table;

	void			stat_Start		( );
	void			stat_Stop		( );

public:
	virtual BOOL	Process			(Fvector3& p, Fvector3& d, Fvector3& n, F32& fFov, F32& fFar, F32& fAspect);

					CDemoPlay		(const char* name, F32 ms, U32 cycles, F32 life_time = 60 * 60 * 1000);
	virtual			~CDemoPlay		( );
};
