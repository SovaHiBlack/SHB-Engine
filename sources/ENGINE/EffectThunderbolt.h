#pragma once

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
		float					fOpacity;
		Fvector2				fRadius;
		shared_str				texture;
		shared_str				shader;
		ref_shader				hShader;
		SFlare( )
		{
			fOpacity = 0;
			fRadius.set(0.0f, 0.0f);
		}
	};
	SFlare						m_GradientTop;
	SFlare						m_GradientCenter;
	shared_str					name;
	CLAItem*					color_anim;

public:
	SThunderboltDesc(CIniFile* pIni, const char* sect);
	~SThunderboltDesc( );
};

struct SThunderboltCollection
{
	using ThunderboltDescVec		= xr_vector<SThunderboltDesc*>;
	using ThunderboltDescVec_it		= ThunderboltDescVec::iterator;
	ThunderboltDescVec				palette;
	shared_str						section;

public:
	SThunderboltCollection(CIniFile* pIni, const char* sect);
	~SThunderboltCollection( );
	SThunderboltDesc* GetRandomDesc( )
	{
		VERIFY(palette.size( ) > 0);
		return palette[Random.randI(palette.size( ))];
	}
};

// Эффект Молнии
class ENGINE_API CEffectThunderbolt
{
protected:
	using ThunderboltCollectionVec		= xr_vector<SThunderboltCollection*>;
	using ThunderboltCollectionVec_it	= ThunderboltCollectionVec::iterator;
	ThunderboltCollectionVec			collection;
	SThunderboltDesc*					current;

private:
	Fmatrix						current_xform;
	Fvector3					current_direction;

	ref_geom					hGeom_model;

	// states
	enum EState
	{
		stIdle,
		stWorking
	};
	EState						state;

	ref_geom					hGeom_gradient;

	Fvector3					lightning_center;
	float						lightning_size;
	float						lightning_phase;

	float						life_time;
	float						current_time;
	float						next_lightning_time;
	BOOL						bEnabled;

	// params
	Fvector2					p_var_alt;
	float						p_var_long;
	float						p_min_dist;
	float						p_tilt;
	float						p_second_prop;
	float						p_sky_color;
	float						p_sun_color;
	float						p_fog_color;

	BOOL						RayPick(const Fvector3& s, const Fvector3& d, float& range);
	void						Bolt(int id, float period, float life_time);

public:
	CEffectThunderbolt( );
	~CEffectThunderbolt( );

	void						OnFrame(int id, float period, float duration);
	void						Render( );

	int							AppendDef(CIniFile* pIni, const char* sect);
};
