#ifndef SoundRender_CoreH
#define SoundRender_CoreH
#pragma once
                                          
#include "SoundRender.h"
#include "SoundRender_Environment.h"
#include "SoundRender_Cache.h"
#include "soundrender_environment.h"

class CSoundRender_Core					: public CSound_manager_interface
{
    volatile BOOL						bLocked;
protected:
	virtual void						_create_data			( ref_sound_data& S, pcstr fName,	esound_type sound_type, int game_type);
	virtual void						_destroy_data			( ref_sound_data& S);
protected:
    BOOL								bListenerMoved;

	CSoundRender_Environment			e_current;
	CSoundRender_Environment			e_target;
public:
	typedef	std::pair<ref_sound_data_ptr, f32>	event;
	xr_vector<event>					s_events;
public:
	BOOL								bPresent;
	BOOL								bUserEnvironment;
    BOOL	 							bEAX;					// Boolean variable to indicate presence of EAX Extension 
    BOOL								bDeferredEAX;
    BOOL								bReady;

    WAVEFORMATEX						wfm;
	CTimer								Timer;
	u32									Timer_Value;
	u32									Timer_Delta;
	sound_event*						Handler;
protected:
	// Collider
	CDB::COLLIDER						geom_DB;

	CDB::MODEL*							geom_SOM;
	CDB::MODEL*							geom_MODEL;
	CDB::MODEL*							geom_ENV;

	// Containers
	xr_vector<CSoundRender_Source*>		s_sources;
	xr_vector<CSoundRender_Emitter*>	s_emitters;
	u32									s_emitters_u;			// emitter update marker
	xr_vector<CSoundRender_Target*>		s_targets;
	xr_vector<CSoundRender_Target*>		s_targets_defer;
	u32									s_targets_pu;			// parameters update
	SoundEnvironment_LIB*				s_environment;
	CSoundRender_Environment			s_user_environment;

	int									m_iPauseCounter;
public:
	// Cache
	CSoundRender_Cache					cache;
	u32									cache_bytes_per_line;
protected:
	virtual void						i_eax_set				(const GUID* guid, u32 prop, void* val, u32 sz)=0;
	virtual void						i_eax_get				(const GUID* guid, u32 prop, void* val, u32 sz)=0;
public:
										CSoundRender_Core		();
	virtual								~CSoundRender_Core		();

	// General
	virtual void  						_initialize				( u64 window )=0;
	virtual void						_clear					( )=0;
	virtual void						_restart				( );

	// Sound interface
			void						verify_refsound			( ref_sound& S);
	virtual void						create					( ref_sound& S, pcstr fName,			esound_type sound_type, int	game_type);
	virtual void						clone					( ref_sound& S, const ref_sound& from,	esound_type sound_type, int	game_type);
	virtual void						destroy					( ref_sound& S);
	virtual void						stop_emitters			( );
	virtual int							pause_emitters			( bool val );

	virtual void						play					( ref_sound& S, CObject* O,								u32 flags=0, f32 delay=0.f);
	virtual void						play_at_pos				( ref_sound& S, CObject* O,		const fVector3& pos,		u32 flags=0, f32 delay=0.f);
	virtual void						play_no_feedback		( ref_sound& S, CObject* O,	u32 flags=0, f32 delay=0.f, fVector3* pos=0, f32* vol=0, f32* freq=0, fVector2* range=0);
	virtual void						set_master_volume		(f32			f )=0;
	virtual void						set_geometry_env		( IReader*		I );
	virtual void						set_geometry_som		( IReader*		I );
	virtual void						set_geometry_occ		( CDB::MODEL*	M );
	virtual void						set_handler				( sound_event*	E );

	virtual void						update					( const fVector3& P, const fVector3& D, const fVector3& N );
	virtual void						update_events			( );
	virtual void						statistic				( CSound_stats*  dest, CSound_stats_ext*  ext );

	// listener
//	virtual const fVector3&				listener_position		( )=0;
	virtual void						update_listener			(const fVector3& P, const fVector3& D, const fVector3& N, f32 dt)=0;
	// eax listener
	void								i_eax_commit_setting	();
	void								i_eax_listener_set		(CSound_environment* E);
	void								i_eax_listener_get		(CSound_environment* E);

public:
	CSoundRender_Source*				i_create_source			(pcstr name				);
	void								i_destroy_source		( CSoundRender_Source*  S	);
	CSoundRender_Emitter*				i_play					( ref_sound* S, BOOL _loop, f32 delay	);
	void								i_start					( CSoundRender_Emitter* E	);
	void								i_stop					( CSoundRender_Emitter* E	);
	void								i_rewind				( CSoundRender_Emitter* E	);
	BOOL								i_allow_play			( CSoundRender_Emitter* E	);
    virtual BOOL						i_locked 				(){return bLocked;}

	virtual void						object_relcase			( CObject* obj );

	virtual f32						get_occlusion_to		( const fVector3& hear_pt, const fVector3& snd_pt, f32 dispersion=0.2f );
	f32								get_occlusion			(fVector3& P, f32 R, fVector3* occ );
	CSoundRender_Environment*			get_environment			( const fVector3& P );

	void								env_load				();
	void								env_unload				();
	void								env_apply				();
};
extern CSoundRender_Core* SoundRender;
#endif
