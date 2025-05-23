#include "stdafx.h"

#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Source.h"

void CSoundRender_Emitter::start(ref_sound* _owner, BOOL _loop, f32 delay)
{
	starting_delay			= delay;

    VERIFY					(_owner);
	owner_data				= _owner->_p;			VERIFY(owner_data);
	source					= (CSoundRender_Source*)owner_data->handle;
	p_source.position.set	(0,0,0);
	p_source.min_distance	= source->m_fMinDist;	// DS3D_DEFAULTMINDISTANCE;
	p_source.max_distance	= source->m_fMaxDist;	// 300.f;
	p_source.base_volume	= source->m_fBaseVolume;// 1.f
	p_source.volume			= 1.f; 					// 1.f
	p_source.freq			= 1.f;
	p_source.max_ai_distance= source->m_fMaxAIDist;	// 300.f;

    if (fis_zero(delay, EPSILON_3)){
		state				= _loop?stStartingLooped:stStarting;
    }else{
		state				= _loop?stStartingLoopedDelayed:stStartingDelayed;
		dwTimeToPropagade	= SoundRender->Timer.GetElapsed_ms();
	}
	bStopping				=	FALSE;
	bRewind					=	FALSE;
}

void CSoundRender_Emitter::i_stop()
{
	bRewind					=	FALSE;
	if (target)	SoundRender->i_stop		(this);
	if (owner_data){
		Event_ReleaseOwner		(); 
		VERIFY(this==owner_data->feedback);
		owner_data->feedback	= NULL;
		owner_data				= NULL;
	}
	state = stStopped;
}

void CSoundRender_Emitter::stop	(BOOL bDeffered)
{
	if (bDeffered)			bStopping=TRUE;
	else					i_stop();
}

void CSoundRender_Emitter::rewind()
{
	bStopping					=	FALSE;

	u32 dwTime					=	SoundRender->Timer.GetElapsed_ms();
	u32 dwDiff					=	dwTime-dwTimeStarted;
	dwTimeStarted				+=	dwDiff;
	dwTimeToStop				+=	dwDiff;
	dwTimeToPropagade			=	dwTime;

	position					=	0;
	bRewind						=	TRUE;
}

void CSoundRender_Emitter::pause(BOOL bVal, int id)
{
	if (bVal)
	{
		if (0==iPaused)	iPaused	= id;
	}else
	{
		if (id==iPaused)iPaused	= 0;
	}
}

void CSoundRender_Emitter::cancel()
{
	// Msg		("- %10s : %3d[%1.4f] : %s","cancel",dbg_ID,priority(),source->fname);
	switch	(state) 
	{
	case stPlaying:
		// switch to: SIMULATE
		state					=	stSimulating;		// switch state
		SoundRender->i_stop		(this);
		break;
	case stPlayingLooped:
		// switch to: SIMULATE
		state					=	stSimulatingLooped;	// switch state
		SoundRender->i_stop		(this);
		break;
	default:
		FATAL					("Non playing ref_sound forced out of render queue");
		break;
	}
}
