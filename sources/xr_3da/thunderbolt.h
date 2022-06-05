// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

//refs
class ENGINE_API IRender_DetailModel;
class ENGINE_API CLAItem;

struct SThunderboltDesc
{
	// geom
	IRender_DetailModel*		l_model;
    // sound
    ref_sound					snd;
    // gradient
	struct SFlare
	{
		F32					fOpacity;
	    Fvector2				fRadius;
        shared_str				texture;
        shared_str				shader;
        ref_shader				hShader;
    	SFlare()				{ fOpacity = 0; fRadius.set(0.f,0.f);}
	};
    SFlare						m_GradientTop;
    SFlare						m_GradientCenter;
    shared_str					name;
	CLAItem*					color_anim;
public:
								SThunderboltDesc	(CInifile* pIni, pcstr sect);
							    ~SThunderboltDesc	();
};

struct SThunderboltCollection
{
	DEFINE_VECTOR(SThunderboltDesc*,DescVec,DescIt);
	DescVec			  			palette;
	shared_str					section;
public:
								SThunderboltCollection	(CInifile* pIni, pcstr sect);
								~SThunderboltCollection	();
	SThunderboltDesc*			GetRandomDesc			(){VERIFY(palette.size()>0); return palette[Random.randI(palette.size())];}
};

#define THUNDERBOLT_CACHE_SIZE	8
//
class ENGINE_API CEffect_Thunderbolt
{
protected:
	DEFINE_VECTOR(SThunderboltCollection*,CollectionVec,CollectionVecIt);
	CollectionVec				collection;
	SThunderboltDesc*			current;
private:
    Fmatrix				  		current_xform;
	Fvector3					current_direction;

	ref_geom			  		hGeom_model;
    // states
	enum EState
	{
        stIdle,
		stWorking
	};
	EState						state;

	ref_geom			  		hGeom_gradient;

    Fvector						lightning_center;
	F32						lightning_size;
	F32						lightning_phase;

	F32						life_time;
	F32						current_time;
	F32						next_lightning_time;
	BOOL						bEnabled;

    // params
    Fvector2					p_var_alt;
	F32						p_var_long;
	F32						p_min_dist;
	F32						p_tilt;
	F32						p_second_prop;
	F32						p_sky_color;
	F32						p_sun_color;
	F32						p_fog_color;
private:
	BOOL						RayPick				(const Fvector& s, const Fvector& d, F32& range);
    void						Bolt				(int id, F32 period, F32 life_time);
public:                     
								CEffect_Thunderbolt	(); 
								~CEffect_Thunderbolt();

	void						OnFrame				(int id, F32 period, F32 duration);
	void						Render				();

	int							AppendDef			(CInifile* pIni, pcstr sect);
};
