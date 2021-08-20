// SleepEffector.h:		эффектор, который запускается во время сна актера

#pragma once

#include "..\ENGINE\CameraManager.h"
#include "..\ENGINE\EffectorCam.h"
#include "..\ENGINE\EffectorPostProcess.h"

#define SLEEP_EFFECTOR_TYPE_ID			8
#define FATIGUE_EFFECTOR_TYPE_ID		33

class CSleepEffectorPP : public CEffectorPostProcess
{
	using inherited						= CEffectorPostProcess;

	SPostProcessInfo								state;			// current state
	float								m_total;		// total PP time
	float								m_attack;		// attack time in percents	[0..1]
	float								m_release;		// release time in percents	[0..1]

public:
					CSleepEffectorPP	(const SPostProcessInfo& ppi, float life_time, float attack_time = 0.0f, float release_time = 0.0f);
	virtual BOOL	Process				(SPostProcessInfo& pp);

	//текущий статус сна
	enum SLEEP_STATE
	{
		BEGIN_SLEEP,
		BEFORE_SLEEPING,
		SLEEPING,
		AWAKING
	};
	SLEEP_STATE							m_eSleepState;
};

struct SSleepEffector
{
	SPostProcessInfo					ppi;
	float								time;
	float								time_attack;
	float								time_release;
};
