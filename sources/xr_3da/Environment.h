#pragma once

// refs
class ENGINE_API	IRender_Visual;
class ENGINE_API	CInifile;
class ENGINE_API 	CEnvironment;

// refs - effects
class ENGINE_API	CEnvironment;
class ENGINE_API	CLensFlare;	
class ENGINE_API	CEffect_Rain;
class ENGINE_API	CEffect_Thunderbolt;

class ENGINE_API	CPerlinNoise1D;

#define DAY_LENGTH		86400.f

#include "blenders\blender.h"
class CBlender_skybox		: public IBlender  
{
public:
	virtual		pcstr		getComment()	{ return "INTERNAL: combiner";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C)
	{
		C.r_Pass			("sky2",		"sky2",			FALSE,	TRUE, FALSE);
		C.r_Sampler_clf		("s_sky0",		"$null"			);
		C.r_Sampler_clf		("s_sky1",		"$null"			);
		C.r_Sampler_rtf		("s_tonemap",	"$user$tonemap"	);	//. hack
		C.r_End				();
	}
};

// t-defs
class ENGINE_API	CEnvModifier
{
public:
	Fvector3			position;
	F32				radius;
	F32				power;

	F32				far_plane;
	Fvector3			fog_color;
	F32				fog_density;
	Fvector3			ambient;
	Fvector3			sky_color;		
	Fvector3			hemi_color;

	void				load		(IReader*		fs);
	F32				sum			(CEnvModifier&	_another, Fvector3& view);
};

class ENGINE_API	CEnvAmbient{
public:
	struct SEffect{
		u32 			life_time;
		ref_sound		sound;		
		shared_str		particles;
		Fvector			offset;
		F32			wind_gust_factor;
	};
	DEFINE_VECTOR(SEffect,EffectVec,EffectVecIt);
protected:
	shared_str			section;
	EffectVec			effects;
	xr_vector<ref_sound>sounds;
	Fvector2			sound_dist;
	Ivector2			sound_period;
	Ivector2			effect_period;
public:
	void				load				(const shared_str& section);
	IC SEffect*			get_rnd_effect		(){return effects.empty()?0:&effects[Random.randI(effects.size())];}
	IC ref_sound*		get_rnd_sound		(){return sounds.empty()?0:&sounds[Random.randI(sounds.size())];}
	IC const shared_str&name				(){return section;}
	IC u32				get_rnd_sound_time	(){return Random.randI(sound_period.x,sound_period.y);}
	IC F32			get_rnd_sound_dist	(){return Random.randF(sound_dist.x,sound_dist.y);}
	IC u32				get_rnd_effect_time (){return Random.randI(effect_period.x,effect_period.y);}
};

class ENGINE_API	CEnvDescriptor
{
public:
	F32				exec_time;
	F32				exec_time_loaded;
	
	shared_str			sky_texture_name	;
	shared_str			sky_texture_env_name;
	shared_str			clouds_texture_name	;

	ref_texture			sky_texture		;
	ref_texture			sky_texture_env	;
	ref_texture			clouds_texture	;

	Fvector4			clouds_color	;
	Fvector3			sky_color		;
	F32				sky_rotation	;

	F32				far_plane;

	Fvector3			fog_color;
	F32				fog_density;
	F32				fog_distance;

	F32				rain_density;
	Fvector3			rain_color;

	F32				bolt_period;
	F32				bolt_duration;

	F32				wind_velocity;
	F32				wind_direction;
    
	Fvector3			ambient		;
	Fvector4			hemi_color	;	// w = R2 correction
	Fvector3			sun_color	;
	Fvector3			sun_dir		;

    int					lens_flare_id;
	int					tb_id;
    
	CEnvAmbient*		env_ambient;

#ifdef DEBUG
	shared_str			sect_name;
#endif // DEBUG

						CEnvDescriptor	();

	void				load			(pcstr exec_tm, pcstr sect, CEnvironment* parent);
	void				copy			(const CEnvDescriptor& src)
	{
		F32 tm0		= exec_time;
		F32 tm1		= exec_time_loaded;
		*this			= src;
		exec_time		= tm0;
		exec_time_loaded= tm1;
	}

	void				on_device_create	();
	void				on_device_destroy	();
};

class ENGINE_API		CEnvDescriptorMixer: public CEnvDescriptor{
public:
	STextureList		sky_r_textures;		
	STextureList		sky_r_textures_env;	
	STextureList		clouds_r_textures;	
	F32				weight;

	F32				fog_near;
	F32				fog_far;
public:
	void				lerp			(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, F32 f, CEnvModifier& M, F32 m_power);
	void				clear			();
	void				destroy			();
};

class ENGINE_API	CEnvironment
{
	struct str_pred : public std::binary_function<shared_str, shared_str, bool>	{	
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{	return xr_strcmp(x,y)<0;	}
	};
public:
	DEFINE_VECTOR			(CEnvAmbient*,EnvAmbVec,EnvAmbVecIt);
	DEFINE_VECTOR			(CEnvDescriptor*,EnvVec,EnvIt);
	DEFINE_MAP_PRED			(shared_str,EnvVec,EnvsMap,EnvsMapIt,str_pred);
private:
	// clouds
	FvectorVec				CloudsVerts;
	U16Vec					CloudsIndices;
private:
	F32					NormalizeTime	(F32 tm);
	F32					TimeDiff		(F32 prev, F32 cur);
	F32					TimeWeight		(F32 val, F32 min_t, F32 max_t);
	void					SelectEnvs		(EnvVec* envs, CEnvDescriptor*& e0, CEnvDescriptor*& e1, F32 tm);
	void					SelectEnv		(EnvVec* envs, CEnvDescriptor*& e, F32 tm);
	void					StopWFX			();
public:
	static bool sort_env_pred	(const CEnvDescriptor* x, const CEnvDescriptor* y)
	{	return x->exec_time < y->exec_time;	}
	static bool sort_env_etl_pred	(const CEnvDescriptor* x, const CEnvDescriptor* y)
	{	return x->exec_time_loaded < y->exec_time_loaded;	}
protected:
	CBlender_skybox			m_b_skybox;
	CPerlinNoise1D*			PerlinNoise1D;

	F32					fGameTime;
public:
	F32					wind_strength_factor;
	F32					wind_gust_factor;
	// Environments
	CEnvDescriptorMixer		CurrentEnv;
	CEnvDescriptor*			Current[2];

	bool					bWFX;
	F32					wfx_time;
	CEnvDescriptor*			WFX_end_desc[2];
    
    EnvVec*					CurrentWeather;
    shared_str				CurrentWeatherName;
	shared_str				CurrentCycleName;

	EnvsMap					WeatherCycles;
	EnvsMap					WeatherFXs;
	xr_vector<CEnvModifier>	Modifiers;
	EnvAmbVec				Ambients;

	ref_shader				sh_2sky;
	ref_geom				sh_2geom;

	ref_shader				clouds_sh;
	ref_geom				clouds_geom;

	CEffect_Rain*			eff_Rain;
	CLensFlare*				eff_LensFlare;
	CEffect_Thunderbolt*	eff_Thunderbolt;

	F32					fTimeFactor;
	ref_texture				tonemap;
	ref_texture				tsky0,tsky1;

	void					SelectEnvs			(F32 gt);

	CEnvAmbient*			AppendEnvAmb		(const shared_str& sect);

	void					Invalidate			();
public:
							CEnvironment		();
							~CEnvironment		();

	void					load				();
    void					unload				();

	void					mods_load			();
	void					mods_unload			();

	void					OnFrame				();

	void					RenderSky			();
	void					RenderClouds		();
	void					RenderFlares		();
	void					RenderLast			();

	bool					SetWeatherFX		(shared_str name);
	bool					IsWFXPlaying		(){return bWFX;}
    void					SetWeather			(shared_str name, bool forced=false);
    shared_str				GetWeather			()					{ return CurrentWeatherName;}
	void					SetGameTime			(F32 game_time, F32 time_factor);

	void					OnDeviceCreate		();
	void					OnDeviceDestroy		();
};

ENGINE_API extern Flags32	psEnvFlags;
ENGINE_API extern F32		psVisDistance;