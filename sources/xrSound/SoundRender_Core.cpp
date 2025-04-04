#include "stdafx.h"

#include "../xr_3da/xrLevel.h"
#include "soundrender_core.h"
#include "soundrender_source.h"
#include "soundrender_emitter.h"
#pragma warning(push)
#pragma warning(disable:4995)
#include <eax.h>
#pragma warning(pop)

int		psSoundTargets			= 16;
flags32	psSoundFlags			= {ss_Hardware | ss_EAX};
f32		psSoundOcclusionScale	= 0.5f;
f32		psSoundCull				= 0.01f;
f32		psSoundRolloff			= 0.75f;
u32		psSoundFreq				= sf_44K;
u32		psSoundModel			= 0;
f32		psSoundVEffects			= 1.0f;
f32		psSoundVFactor			= 1.0f;

f32		psSoundVMusic			= 0.7f;
int		psSoundCacheSizeMB		= 16;

CSoundRender_Core*				SoundRender = 0;
CSound_manager_interface*		Sound		= 0;

CSoundRender_Core::CSoundRender_Core	()
{
	bPresent					= FALSE;
    bEAX						= FALSE;
    bDeferredEAX				= FALSE;
	bUserEnvironment			= FALSE;
	geom_MODEL					= NULL;
	geom_ENV					= NULL;
	geom_SOM					= NULL;
	s_environment				= NULL;
	Handler						= NULL;
	s_targets_pu				= 0;
	s_emitters_u				= 0;
    e_current.set_identity		();
    e_target.set_identity		();
    bListenerMoved				= FALSE;
    bReady						= FALSE;
    bLocked						= FALSE;
	Timer_Value					= Timer.GetElapsed_ms();
	Timer_Delta					= 0;
	m_iPauseCounter				= 1;
}

CSoundRender_Core::~CSoundRender_Core()
{
	xr_delete					(geom_ENV);
	xr_delete					(geom_SOM);
}

void CSoundRender_Core::_initialize	(u64 window)
{
    Log							("* sound: EAX 2.0 extension:",bEAX?"present":"absent");
    Log							("* sound: EAX 2.0 deferred:",bDeferredEAX?"present":"absent");
	Timer.Start					( );

    // load environment
	env_load					();

	bPresent					= TRUE;

	// Cache
	cache_bytes_per_line		= (sdef_target_block/8)*wfm.nAvgBytesPerSec/1000;
    cache.initialize			(psSoundCacheSizeMB*1024,cache_bytes_per_line);

    bReady						= TRUE;
}

extern xr_vector<u8> g_target_temp_data;
void CSoundRender_Core::_clear	()
{
    bReady						= FALSE;
	cache.destroy				();
	env_unload					();

    // remove sources
	for (u32 sit=0; sit<s_sources.size(); sit++)
    	xr_delete				(s_sources[sit]);
    s_sources.clear				();
    
    // remove emmiters
	for (u32 eit=0; eit<s_emitters.size(); eit++)
    	xr_delete				(s_emitters[eit]);
    s_emitters.clear			();

	g_target_temp_data.clear	();
}

void CSoundRender_Core::stop_emitters()
{
	for (u32 eit=0; eit<s_emitters.size(); eit++)
		s_emitters[eit]->stop	(FALSE);
}

int CSoundRender_Core::pause_emitters(bool val)
{
	m_iPauseCounter				+= val?+1:-1;
	VERIFY						(m_iPauseCounter>=0);

	for (u32 it=0; it<s_emitters.size(); it++)
		((CSoundRender_Emitter*)s_emitters[it])->pause	(val,val?m_iPauseCounter:m_iPauseCounter+1);

	return m_iPauseCounter;
}

void CSoundRender_Core::env_load	()
{
	// Load environment
	string_path					fn;
	if (FS.exist(fn,"$game_data$",SNDENV_FILENAME))
	{
		s_environment				= xr_new<SoundEnvironment_LIB>();
		s_environment->Load			(fn);
	}

	// Load geometry

	// Assosiate geometry
}

void CSoundRender_Core::env_unload	()
{
	// Unload 
	if (s_environment)
		s_environment->Unload	();
	xr_delete					(s_environment);

	// Unload geometry
}

void CSoundRender_Core::_restart		()
{
	cache.destroy				();
	cache.initialize			(psSoundCacheSizeMB*1024,cache_bytes_per_line);
	env_apply					();
}

void CSoundRender_Core::set_handler(sound_event* E)
{
	Handler			= E;
}

void CSoundRender_Core::set_geometry_occ(CDB::MODEL* M)
{
	geom_MODEL		= M;
}

void CSoundRender_Core::set_geometry_som(IReader* I)
{
	xr_delete				(geom_SOM);

	if (0==I)		return;

	// check version
	R_ASSERT		(I->find_chunk(0));
	u32 version		= I->r_u32(); 
	VERIFY2			(version==0,"Invalid SOM version");
	// load geometry	
	IReader* geom	= I->open_chunk(1); 
	VERIFY2			(geom,"Corrupted SOM file");
	// Load tris and merge them
	struct SOM_poly{
		fVector3	v1;
		fVector3	v2;
		fVector3	v3;
		u32			b2sided;
		f32		occ;
	};
	// Create AABB-tree
	CDB::Collector				CL;			
	while (!geom->eof()){
		SOM_poly				P;
		geom->r					(&P,sizeof(P));
		CL.add_face_packed_D	(P.v1,P.v2,P.v3,*(u32*)&P.occ,0.01f);
		if (P.b2sided)
			CL.add_face_packed_D(P.v3,P.v2,P.v1,*(u32*)&P.occ,0.01f);
	}
	geom_SOM			= xr_new<CDB::MODEL> ();
	geom_SOM->build		(CL.getV(),int(CL.getVS()),CL.getT(),int(CL.getTS()));
}

void CSoundRender_Core::set_geometry_env(IReader* I)
{
	xr_delete				(geom_ENV);

	if (0==I)				return;
	if (0==s_environment)	return;

	// Assosiate names
	xr_vector<u16>			ids;
	IReader*				names	= I->open_chunk(0);
	while (!names->eof())
	{
		string256			n;
		names->r_stringZ	(n,sizeof(n));
		int id				= s_environment->GetID(n);
		R_ASSERT			(id>=0);
		ids.push_back		(u16(id));
	}
	names->close		();

	// Load geometry
	IReader*			geom_ch	= I->open_chunk(1);
	
	u8*	_data			= (u8*)xr_malloc(geom_ch->length());
	
	Memory.mem_copy		(_data, geom_ch->pointer(), geom_ch->length() );

	IReader* geom		= xr_new<IReader>(_data, geom_ch->length(), 0);
	
	hdrCFORM			H;
	geom->r				(&H,sizeof(hdrCFORM));
	fVector3*	verts	= (fVector3*)geom->pointer();
	CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
	for (u32 it=0; it<H.facecount; it++)
	{
		CDB::TRI*	T	= tris+it;
		u16		id_front= (u16)((T->dummy&0x0000ffff)>>0);		//	front face
		u16		id_back	= (u16)((T->dummy&0xffff0000)>>16);		//	back face
		R_ASSERT		(id_front<(u16)ids.size());
		R_ASSERT		(id_back<(u16)ids.size());
		T->dummy		= u32(ids[id_back]<<16) | u32(ids[id_front]);
	}

	geom_ENV			= xr_new<CDB::MODEL> ();
	geom_ENV->build		(verts, H.vertcount, tris, H.facecount);

	geom_ch->close			();
	geom->close				();
	xr_free					(_data);
}

void	CSoundRender_Core::verify_refsound		( ref_sound& S)
{
/*
#ifdef	DEBUG
	int			local_value		= 0;
	void*		ptr_refsound	= &S;
	void*		ptr_local		= &local_value;
	ptrdiff_t	difference		= (ptrdiff_t)_abs(s64(ptrdiff_t(ptr_local) - ptrdiff_t(ptr_refsound)));
	string256	err_str;
	if(difference < (4*1024))
	{
		sprintf		(err_str,"diff=[%d] local/stack-based ref_sound passed. memory corruption will accur.",difference);
		VERIFY2		(0, err_str);
	}
#endif
*/
}

void	CSoundRender_Core::create				( ref_sound& S, pcstr fName, esound_type sound_type, int game_type )
{
	if (!bPresent)		return;
	verify_refsound		(S);
    S._p				= xr_new<ref_sound_data>(fName,sound_type,game_type);
}

void	CSoundRender_Core::clone				( ref_sound& S, const ref_sound& from, esound_type sound_type, int	game_type )
{
	if (!bPresent)		return;
	S._p				= xr_new<ref_sound_data>();
	S._p->handle		= from._p->handle;
	S._p->g_type		= (game_type==sg_SourceType)?S._p->handle->game_type():game_type;
	S._p->s_type		= sound_type;
}


void	CSoundRender_Core::play					( ref_sound& S, CObject* O, u32 flags, f32 delay)
{
	if (!bPresent || 0==S._handle())return;
	verify_refsound		(S);
	S._p->g_object		= O;
	if (S._feedback())	((CSoundRender_Emitter*)S._feedback())->rewind ();
	else				i_play					(&S,flags&sm_Looped,delay);
	if (flags&sm_2D)	S._feedback()->switch_to_2D();
}
void	CSoundRender_Core::play_no_feedback		( ref_sound& S, CObject* O, u32 flags, f32 delay, fVector3* pos, f32* vol, f32* freq, fVector2* range)
{
	if (!bPresent || 0==S._handle())return;
	verify_refsound		(S);
	ref_sound_data_ptr	orig = S._p;
	S._p				= xr_new<ref_sound_data>();
	S._p->handle		= orig->handle;
	S._p->g_type		= orig->g_type;
	S._p->g_object		= O;
	i_play				(&S,flags&sm_Looped,delay);
	if (flags&sm_2D)	S._feedback()->switch_to_2D();
	if (pos)			S._feedback()->set_position	(*pos);
	if (freq)			S._feedback()->set_frequency(*freq);
	if (range)			S._feedback()->set_range   	((*range)[0],(*range)[1]);
	if (vol)			S._feedback()->set_volume   (*vol);
	S._p				= orig;
}
void	CSoundRender_Core::play_at_pos			( ref_sound& S, CObject* O, const fVector3& pos, u32 flags, f32 delay)
{
	if (!bPresent || 0==S._handle())return;
	verify_refsound		(S);
	S._p->g_object		= O;
	if (S._feedback())	((CSoundRender_Emitter*)S._feedback())->rewind ();
	else				i_play					(&S,flags&sm_Looped,delay);
	S._feedback()->set_position					(pos);
	if (flags&sm_2D)	S._feedback()->switch_to_2D();
}
void	CSoundRender_Core::destroy	(ref_sound& S )
{
	if (S._feedback()){                   
		CSoundRender_Emitter* E		= (CSoundRender_Emitter*)S._feedback();
		E->stop						(FALSE);
	}
	S._p				= 0;
}                                                    

void CSoundRender_Core::_create_data( ref_sound_data& S, pcstr fName, esound_type sound_type, int game_type)
{
	string_path			fn;
	strcpy				(fn,fName);
    if (strext(fn))		*strext(fn)	= 0;
	S.handle			= (CSound_source*)SoundRender->i_create_source(fn);
	S.g_type			= (game_type==sg_SourceType)?S.handle->game_type():game_type;
	S.s_type			= sound_type;
	S.feedback			= 0; 
    S.g_object			= 0; 
    S.g_userdata		= 0;
}
void CSoundRender_Core::_destroy_data( ref_sound_data& S)
{
	if (S.feedback){                   
		CSoundRender_Emitter* E		= (CSoundRender_Emitter*)S.feedback;
		E->stop						(FALSE);
	}
	R_ASSERT						(0==S.feedback);
	SoundRender->i_destroy_source	((CSoundRender_Source*)S.handle);
	S.handle						= NULL;
}

CSoundRender_Environment*	CSoundRender_Core::get_environment			( const fVector3& P )
{
	static CSoundRender_Environment	identity;

	if (bUserEnvironment){
		return &s_user_environment;
	}else{
		if (geom_ENV){
			fVector3	dir				= {0.0f,-1.0f,0.0f};

			geom_DB.ray_options		(CDB::OPT_ONLYNEAREST);
			geom_DB.ray_query		(geom_ENV,P,dir,1000.f);
			if (geom_DB.r_count()){
				CDB::RESULT*		r	= geom_DB.r_begin();
           
				CDB::TRI*			T	= geom_ENV->get_tris()+r->id;
				fVector3*			V	= geom_ENV->get_verts();
				fVector3 tri_norm;
				tri_norm.mknormal		(V[T->verts[0]],V[T->verts[1]],V[T->verts[2]]);
				f32	dot				= dir.dotproduct(tri_norm);
				if (dot<0){
					u16		id_front	= (u16)((T->dummy&0x0000ffff)>>0);		//	front face
					return	s_environment->Get(id_front);
				}else{
					u16		id_back		= (u16)((T->dummy&0xffff0000)>>16);	//	back face
					return	s_environment->Get(id_back);
				}
			}else{
				identity.set_identity	();
				return &identity;
			}
		}else{
			identity.set_identity	();
			return &identity;
		}
	}
}

void						CSoundRender_Core::env_apply		()
{
/*
	// Force all sounds to change their environment
	// (set their positions to signal changes in environment)
	for (u32 it=0; it<s_emitters.size(); it++)
	{
		CSoundRender_Emitter*	pEmitter	= s_emitters[it];
		const CSound_params*	pParams		= pEmitter->get_params	();
		pEmitter->set_position	(pParams->position);
	}
*/
    bListenerMoved			= TRUE;
}

void CSoundRender_Core::update_listener( const fVector3& P, const fVector3& D, const fVector3& N, f32 dt )
{
}

void	CSoundRender_Core::i_eax_listener_set	(CSound_environment* _E)
{
	VERIFY(bEAX);
    CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
    EAXLISTENERPROPERTIES 		ep;
    ep.lRoom					= iFloor(E->Room)				;	// room effect level at low frequencies
    ep.lRoomHF					= iFloor(E->RoomHF)				;   // room effect high-frequency level re. low frequency level
    ep.flRoomRolloffFactor		= E->RoomRolloffFactor			;   // like DS3D flRolloffFactor but for room effect
    ep.flDecayTime				= E->DecayTime					;   // reverberation decay time at low frequencies
    ep.flDecayHFRatio			= E->DecayHFRatio				;   // high-frequency to low-frequency decay time ratio
    ep.lReflections				= iFloor(E->Reflections)		;   // early reflections level relative to room effect
    ep.flReflectionsDelay		= E->ReflectionsDelay			;   // initial reflection delay time
    ep.lReverb					= iFloor(E->Reverb)	 			;   // late reverberation level relative to room effect
    ep.flReverbDelay			= E->ReverbDelay				;   // late reverberation delay time relative to initial reflection
    ep.dwEnvironment			= EAXLISTENER_DEFAULTENVIRONMENT;  	// sets all listener properties
    ep.flEnvironmentSize		= E->EnvironmentSize			;  	// environment size in meters
    ep.flEnvironmentDiffusion	= E->EnvironmentDiffusion		; 	// environment diffusion
    ep.flAirAbsorptionHF		= E->AirAbsorptionHF			;	// change in level per meter at 5 kHz
    ep.dwFlags					= EAXLISTENER_DEFAULTFLAGS		;	// modifies the behavior of properties

    u32 deferred				= bDeferredEAX?DSPROPERTY_EAXLISTENER_DEFERRED:0;
    
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOM, 					&ep.lRoom,					sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMHF, 				&ep.lRoomHF,				sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, 	&ep.flRoomRolloffFactor,	sizeof(f32));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYTIME, 		  	&ep.flDecayTime,			sizeof(f32));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYHFRATIO,			&ep.flDecayHFRatio,			sizeof(f32));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONS, 			&ep.lReflections,			sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY,    	&ep.flReflectionsDelay,		sizeof(f32));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERB, 				&ep.lReverb,				sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERBDELAY, 			&ep.flReverbDelay,			sizeof(f32));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION,	&ep.flEnvironmentDiffusion,	sizeof(f32));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, 		&ep.flAirAbsorptionHF,		sizeof(f32));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_FLAGS, 				&ep.dwFlags,				sizeof(DWORD));
}

void	CSoundRender_Core::i_eax_listener_get	(CSound_environment* _E)
{
	VERIFY(bEAX);
    CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
    EAXLISTENERPROPERTIES 		ep;
	i_eax_get					(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &ep, sizeof(EAXLISTENERPROPERTIES));
    E->Room						= (f32)ep.lRoom					;
    E->RoomHF					= (f32)ep.lRoomHF					;
    E->RoomRolloffFactor		= (f32)ep.flRoomRolloffFactor		;
    E->DecayTime			   	= (f32)ep.flDecayTime				;
    E->DecayHFRatio				= (f32)ep.flDecayHFRatio			;
    E->Reflections				= (f32)ep.lReflections			;
    E->ReflectionsDelay			= (f32)ep.flReflectionsDelay		;
    E->Reverb					= (f32)ep.lReverb					;
    E->ReverbDelay				= (f32)ep.flReverbDelay			;
    E->EnvironmentSize			= (f32)ep.flEnvironmentSize		;
    E->EnvironmentDiffusion		= (f32)ep.flEnvironmentDiffusion	;
    E->AirAbsorptionHF			= (f32)ep.flAirAbsorptionHF		;
}

void CSoundRender_Core::i_eax_commit_setting()
{
	// commit eax 
    if (bDeferredEAX)
    	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_COMMITDEFERREDSETTINGS,NULL,0);
}

void CSoundRender_Core::object_relcase( CObject* obj )
{
	if (obj){
		for (u32 eit=0; eit<s_emitters.size(); eit++){
        	if (s_emitters[eit])
                if (s_emitters[eit]->owner_data)
                 	if (obj==s_emitters[eit]->owner_data->g_object) 
	                    s_emitters[eit]->owner_data->g_object	= 0;     
        }
    }
}
