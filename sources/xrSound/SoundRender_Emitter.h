#pragma once

#include "SoundRender.h"
#include "SoundRender_Environment.h"

class CSoundRender_Emitter		:	public CSound_emitter
{
	float						starting_delay;

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

		stFORCEDWORD	= unsigned int(-1)
	};

#ifdef DEBUG
	unsigned int							dbg_ID;
#endif // def DEBUG

	CSoundRender_Target*		target;
	CSoundRender_Source*		source;
	ref_sound_data_ptr			owner_data;

	float						priority_scale;
	float						smooth_volume;
	float 						occluder_volume;		// USER
	float						fade_volume;
	Fvector3					occluder	[3];

	State						state;
	unsigned int							position;
	CSound_params				p_source;
	CSoundRender_Environment	e_current;
	CSoundRender_Environment	e_target;

	int							iPaused;
	BOOL						bMoved;
	BOOL						b2D;
	BOOL						bStopping;
	BOOL						bRewind;
	unsigned int							dwTimeStarted;			// time of "Start"
	unsigned int							dwTimeToStop;			// time to "Stop"
	unsigned int							dwTimeToPropagade;

	unsigned int							marker;
	void						i_stop					();

	void						Event_Propagade			();
	void						Event_ReleaseOwner		();
	BOOL						isPlaying				( )						{ return state!=stStopped; }

	virtual BOOL				is_2D					()						{ return b2D; }
	virtual void				switch_to_2D			();
	virtual void				switch_to_3D			();
	virtual void				set_position			(const Fvector3& pos)	{ p_source.position	= pos; bMoved=TRUE;					}
	virtual void				set_frequency			(float scale)			{ VERIFY(_valid(scale));			p_source.freq=scale;}
	virtual void				set_range				(float min, float max)	{ VERIFY(_valid(min)&&_valid(max));	p_source.min_distance=min; p_source.max_distance=max;}
	virtual void				set_volume				(float vol)				{ VERIFY(_valid(vol));				p_source.volume=vol;}
	virtual void				set_priority			(float p)				{ priority_scale = p;									}
	virtual	const CSound_params* get_params				( )						{ return &p_source;										}

	void						fill_block				(void*	ptr, unsigned int size);
	void						fill_data				(unsigned char*	ptr, unsigned int offset, unsigned int size);

	float						priority				();
	void						start					(ref_sound* _owner, BOOL _loop, float delay);
	void						cancel					();						// manager forces out of rendering
	void						update					(float dt);
	BOOL						update_culling			(float dt);
	void						update_environment		(float dt);
	void						rewind					();
	virtual void				stop					(BOOL bDeffered);
	void						pause					(BOOL bVal, int id);

	virtual unsigned int					play_time				( );

	CSoundRender_Emitter		();
	~CSoundRender_Emitter		();
};
