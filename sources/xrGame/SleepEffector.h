//////////////////////////////////////////////////////////////////////
// SleepEffector.h:		эффектор, который запускается во время сна
//						актера                           
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\XR_3DA\effectorPP.h"
#include "..\XR_3DA\effector.h"
#include "..\XR_3DA\cameramanager.h"

#define SLEEP_EFFECTOR_TYPE_ID 8

class CSleepEffectorPP : public CEffectorPP {
	typedef CEffectorPP inherited;	

	SPPInfo state;			// current state
	f32	m_total;		// total PP time
	f32	m_attack;		// attack time in percents	[0..1]
	f32	m_release;		// release time in percents	[0..1]

public:
	CSleepEffectorPP			(const SPPInfo &ppi, f32 life_time, f32 attack_time = 0.0f, f32 release_time = 0.0f);
	virtual	BOOL	Process		(SPPInfo& pp);
	
	//текущий статус сна
	enum SLEEP_STATE { BEGIN_SLEEP,
					   BEFORE_SLEEPING,
					   SLEEPING,
					   AWAKING};
	SLEEP_STATE m_eSleepState;
};

struct SSleepEffector
{
	SPPInfo	ppi;
	f32	time;
	f32	time_attack;
	f32	time_release;
};
