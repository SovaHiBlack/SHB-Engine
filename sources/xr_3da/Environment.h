#pragma once

// refs
class ENGINE_API	IRenderVisual;
class ENGINE_API	CIniFile;
class ENGINE_API 	CEnvironment;

// refs - effects
class ENGINE_API	CEnvironment;
class ENGINE_API	CLensFlareEffect;
class ENGINE_API	CRainEffect;
class ENGINE_API	CThunderboltEffect;

class ENGINE_API	CPerlinNoise1D;

#define DAY_LENGTH		86400.0f

#include "blenders\blender.h"
class CBlender_skybox : public IBlender
{
public:
	virtual pcstr		getComment( )
	{
		return "INTERNAL: combiner";
	}
	virtual BOOL		canBeDetailed( )
	{
		return FALSE;
	}
	virtual BOOL		canBeLMAPped( )
	{
		return FALSE;
	}

	virtual void		Compile(CBlender_Compile& C)
	{
		C.r_Pass("sky2", "sky2", FALSE, TRUE, FALSE);
		C.r_Sampler_clf("s_sky0", "$null");
		C.r_Sampler_clf("s_sky1", "$null");
		C.r_Sampler_rtf("s_tonemap", "$user$tonemap");	//. hack
		C.r_End( );
	}
};

// t-defs
class ENGINE_API	CEnvironmentModifier
{
public:
	fVector3			position;
	f32				radius;
	f32				power;

	f32				far_plane;
	fVector3			fog_color;
	f32				fog_density;
	fVector3			ambient;
	fVector3			sky_color;
	fVector3			hemi_color;

	void				load(IReader* fs);
	f32				sum(CEnvironmentModifier& _another, fVector3& view);
};

class ENGINE_API	CEnvironmentAmbient
{
public:
	struct SEffect
	{
		u32 			life_time;
		ref_sound		sound;
		shared_str		particles;
		fVector3			offset;
		f32			wind_gust_factor;
	};
	DEFINE_VECTOR(SEffect, EffectVec, EffectVecIt);

protected:
	shared_str			section;
	EffectVec			effects;
	xr_vector<ref_sound>sounds;
	fVector2			sound_dist;
	iVector2			sound_period;
	iVector2			effect_period;

public:
	void				load(const shared_str& section);
	IC SEffect* get_rnd_effect( )
	{
		return effects.empty( ) ? 0 : &effects[Random.randI(effects.size( ))];
	}
	IC ref_sound* get_rnd_sound( )
	{
		return sounds.empty( ) ? 0 : &sounds[Random.randI(sounds.size( ))];
	}
	IC const shared_str& name( )
	{
		return section;
	}
	IC u32				get_rnd_sound_time( )
	{
		return Random.randI(sound_period.x, sound_period.y);
	}
	IC f32			get_rnd_sound_dist( )
	{
		return Random.randF(sound_dist.x, sound_dist.y);
	}
	IC u32				get_rnd_effect_time( )
	{
		return Random.randI(effect_period.x, effect_period.y);
	}
};

class ENGINE_API	CEnvironmentDescriptor
{
public:
	f32				exec_time;
	f32				exec_time_loaded;

	shared_str			sky_texture_name;
	shared_str			sky_texture_env_name;
	shared_str			clouds_texture_name;

	ref_texture			sky_texture;
	ref_texture			sky_texture_env;
	ref_texture			clouds_texture;

	fVector4			clouds_color;
	fVector3			sky_color;
	f32				sky_rotation;

	f32				far_plane;

	fVector3			fog_color;
	f32				fog_density;
	f32				fog_distance;

	f32				rain_density;
	fVector3			rain_color;

	f32				bolt_period;
	f32				bolt_duration;

	f32				wind_velocity;
	f32				wind_direction;

	fVector3			ambient;
	fVector4			hemi_color;	// w = R2 correction
	fVector3			sun_color;
	fVector3			sun_dir;

	s32					lens_flare_id;
	s32					tb_id;

	CEnvironmentAmbient* env_ambient;

#ifdef DEBUG
	shared_str			sect_name;
#endif // DEBUG

	CEnvironmentDescriptor( );

	void				load(pcstr exec_tm, pcstr sect, CEnvironment* parent);
	void				copy(const CEnvironmentDescriptor& src)
	{
		f32 tm0 = exec_time;
		f32 tm1 = exec_time_loaded;
		*this = src;
		exec_time = tm0;
		exec_time_loaded = tm1;
	}

	void				on_device_create( );
	void				on_device_destroy( );
};

class ENGINE_API	CEnvironmentDescriptorMixer : public CEnvironmentDescriptor
{
public:
	STextureList		sky_r_textures;
	STextureList		sky_r_textures_env;
	STextureList		clouds_r_textures;
	f32				weight;

	f32				fog_near;
	f32				fog_far;

public:
	void				lerp(CEnvironment* parent, CEnvironmentDescriptor& A, CEnvironmentDescriptor& B, f32 f, CEnvironmentModifier& M, f32 m_power);
	void				clear( );
	void				destroy( );
};

class ENGINE_API CEnvironment
{
	struct str_pred : public std::binary_function<shared_str, shared_str, bool>
	{
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{
			return xr_strcmp(x, y) < 0;
		}
	};

public:
	DEFINE_VECTOR(CEnvironmentAmbient*, EnvAmbVec, EnvAmbVecIt);
	DEFINE_VECTOR(CEnvironmentDescriptor*, EnvVec, EnvIt);
	DEFINE_MAP_PRED(shared_str, EnvVec, EnvsMap, EnvsMapIt, str_pred);

private:
	// clouds
	FvectorVec				CloudsVerts;
	U16Vec					CloudsIndices;

private:
	f32					NormalizeTime(f32 tm);
	f32					TimeDiff(f32 prev, f32 cur);
	f32					TimeWeight(f32 val, f32 min_t, f32 max_t);
	void					SelectEnvs(EnvVec* envs, CEnvironmentDescriptor*& e0, CEnvironmentDescriptor*& e1, f32 tm);
	void					SelectEnv(EnvVec* envs, CEnvironmentDescriptor*& e, f32 tm);
	void					StopWFX( );

public:
	static bool sort_env_pred(const CEnvironmentDescriptor* x, const CEnvironmentDescriptor* y)
	{
		return x->exec_time < y->exec_time;
	}
	static bool sort_env_etl_pred(const CEnvironmentDescriptor* x, const CEnvironmentDescriptor* y)
	{
		return x->exec_time_loaded < y->exec_time_loaded;
	}

protected:
	CBlender_skybox			m_b_skybox;
	CPerlinNoise1D* PerlinNoise1D;

	f32					fGameTime;

public:
	f32					wind_strength_factor;
	f32					wind_gust_factor;
	// Environments
	CEnvironmentDescriptorMixer		CurrentEnv;
	CEnvironmentDescriptor* Current[2];

	bool					bWFX;
	f32					wfx_time;
	CEnvironmentDescriptor* WFX_end_desc[2];

	EnvVec* CurrentWeather;
	shared_str				CurrentWeatherName;
	shared_str				CurrentCycleName;

	EnvsMap					WeatherCycles;
	EnvsMap					WeatherFXs;
	xr_vector<CEnvironmentModifier>	Modifiers;
	EnvAmbVec				Ambients;

	ref_shader				sh_2sky;
	ref_geom				sh_2geom;

	ref_shader				clouds_sh;
	ref_geom				clouds_geom;

	CRainEffect* eff_Rain;
	CLensFlareEffect* eff_LensFlare;
	CThunderboltEffect* eff_Thunderbolt;

	f32					fTimeFactor;
	ref_texture				tonemap;
	ref_texture				tsky0, tsky1;

	void					SelectEnvs(f32 gt);

	CEnvironmentAmbient* AppendEnvAmb(const shared_str& sect);

	void					Invalidate( );

public:
	CEnvironment( );
	~CEnvironment( );

	void					load( );
	void					unload( );

	void					mods_load( );
	void					mods_unload( );

	void					OnFrame( );

	void					RenderSky( );
	void					RenderClouds( );
	void					RenderFlares( );
	void					RenderLast( );

	bool					SetWeatherFX(shared_str name);
	bool					IsWFXPlaying( )
	{
		return bWFX;
	}
	void					SetWeather(shared_str name, bool forced = false);
	shared_str				GetWeather( )
	{
		return CurrentWeatherName;
	}
	void					SetGameTime(f32 game_time, f32 time_factor);

	void					OnDeviceCreate( );
	void					OnDeviceDestroy( );
};

ENGINE_API extern flags32	psEnvFlags;
ENGINE_API extern f32		psVisDistance;
