// CDemoPlay.h: interface for the CDemoPlay class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "effector.h"

// refs
class	COMotion;
struct	SAnimParams;

// class
class ENGINE_API CDemoPlay :	public CEffectorCam
{
    COMotion*			m_pMotion			;
    SAnimParams*		m_MParam			;

	xr_vector<Fmatrix>	seq					;
	int					m_count				;
	F32				fStartTime			;
	F32				fSpeed				;
	u32					dwCyclesLeft		;

	// statistics
	BOOL				stat_started		;
	CTimer				stat_Timer_frame	;
	CTimer				stat_Timer_total	;
	u32					stat_StartFrame		;
	xr_vector<F32>	stat_table			;
	
	void				stat_Start	()		;
	void				stat_Stop	()		;
public:
	virtual	BOOL		Process		(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect);

						CDemoPlay	(const char *name, F32 ms, u32 cycles, F32 life_time=60*60*1000);
	virtual				~CDemoPlay	();
};
