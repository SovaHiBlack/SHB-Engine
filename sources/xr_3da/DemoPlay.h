// DemoPlay.h: interface for the CDemoPlay class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "effector.h"

// refs
class COMotion;
struct SAnimParams;

// class
class ENGINE_API CDemoPlay : public CEffectorCam
{
	COMotion* m_pMotion;
	SAnimParams* m_MParam;

	xr_vector<fMatrix4x4>	seq;
	s32					m_count;
	f32				fStartTime;
	f32				fSpeed;
	u32					dwCyclesLeft;

	// statistics
	BOOL				stat_started;
	CTimer				stat_Timer_frame;
	CTimer				stat_Timer_total;
	u32					stat_StartFrame;
	xr_vector<f32>	stat_table;

	void				stat_Start( );
	void				stat_Stop( );

public:
	virtual	BOOL		Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect);

	CDemoPlay(pcstr name, f32 ms, u32 cycles, f32 life_time = 60 * 60 * 1000);
	virtual				~CDemoPlay( );
};
