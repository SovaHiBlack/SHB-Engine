#ifndef SoundRender_EmitterH
#define SoundRender_EmitterH
#pragma once

#include "soundrender.h"
#include "soundrender_environment.h"

class CSoundRender_Emitter		:	public CSound_emitter
{
	f32						starting_delay;
public:
	enum State
	{
		stStopped		= 0,

		stStartingDelayed,
		stStartingLoopedDelayed,

		stStarting,
		stStartingLooped,

		stPlaying,
		stPlayingLooped,

		stSimulating,
		stSimulatingLooped,

		stFORCEDWORD	= u32(-1)
	};
public:
#ifdef DEBUG
	u32							dbg_ID;
#endif

	CSoundRender_Target*		target;
	CSoundRender_Source*		source;
	ref_sound_data_ptr			owner_data;

	f32						priority_scale;
	f32						smooth_volume;
	f32 						occluder_volume;		// USER
	f32						fade_volume;
	Fvector						occluder	[3];

	State						state;
	u32							position;
	CSound_params				p_source;
	CSoundRender_Environment	e_current;
	CSoundRender_Environment	e_target;

	int							iPaused;
	BOOL						bMoved;
	BOOL						b2D;
	BOOL						bStopping;
	BOOL						bRewind;
	u32							dwTimeStarted;			// time of "Start"
	u32							dwTimeToStop;			// time to "Stop"
	u32							dwTimeToPropagade;

	u32							marker;
	void						i_stop					();
public:
	void						Event_Propagade			();
	void						Event_ReleaseOwner		();
	BOOL						isPlaying				(void)					{ return state!=stStopped; }

	virtual BOOL				is_2D					()						{ return b2D; }
	virtual void				switch_to_2D			();
	virtual void				switch_to_3D			();
	virtual void				set_position			(const Fvector &pos)	{ p_source.position	= pos; bMoved=TRUE;					}
	virtual void				set_frequency			(f32 scale)			{ VERIFY(_valid(scale));			p_source.freq=scale;}
	virtual void				set_range				(f32 min, f32 max)	{ VERIFY(_valid(min)&&_valid(max));	p_source.min_distance=min; p_source.max_distance=max;}
	virtual void				set_volume				(f32 vol)				{ VERIFY(_valid(vol));				p_source.volume=vol;}
	virtual void				set_priority			(f32 p)				{ priority_scale = p;									}
	virtual	const CSound_params* get_params				( )						{ return &p_source;										}

	void						fill_block				(void*	ptr, u32 size);
	void						fill_data				(u8*	ptr, u32 offset, u32 size);

	f32						priority				();
	void						start					(ref_sound* _owner, BOOL _loop, f32 delay);
	void						cancel					();						// manager forces out of rendering
	void						update					(f32 dt);
	BOOL						update_culling			(f32 dt);
	void						update_environment		(f32 dt);
	void						rewind					();
	virtual void				stop					(BOOL bDeffered);
	void						pause					(BOOL bVal, int id);

	virtual u32					play_time				( );

	CSoundRender_Emitter		();
	~CSoundRender_Emitter		();
};
#endif