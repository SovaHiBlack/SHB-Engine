#pragma once

#include "..\XR_3DA\Envelope.h"
#include "..\XR_3DA\PostProcessEffector.h"
#include "..\XR_3DA\CameraManager.h"

class CEffectorController;

#define POSTPROCESS_PARAMS_COUNT	10
#define POSTPROCESS_FILE_VERSION	0x0001

#define POSTPROCESS_FILE_EXTENSION	".ppe"

typedef enum _pp_params
{
	pp_unknown = -1,
	pp_base_color = 0,
	pp_add_color = 1,
	pp_gray_color = 2,
	pp_gray_value = 3,
	pp_blur = 4,
	pp_dual_h = 5,
	pp_dual_v = 6,
	pp_noise_i = 7,
	pp_noise_g = 8,
	pp_noise_f = 9,
	pp_force_dword = 0x7fffffff
} pp_params;

class CPostProcessParam
{
public:
	virtual void	update(f32 dt) = 0;
	virtual void	load(IReader& pReader) = 0;
	virtual void	save(IWriter& pWriter) = 0;
	virtual f32		get_length( ) = 0;
	virtual size_t	get_keys_count( ) = 0;
};

class CPostProcessValue : public CPostProcessParam
{
protected:
	CEnvelope m_Value;
	f32* m_pfParam;

public:
	CPostProcessValue(f32* pfparam)
	{
		m_pfParam = pfparam;
	}
	virtual void	update(f32 dt)
	{
		*m_pfParam = m_Value.Evaluate(dt);
	}
	virtual void	load(IReader& pReader);
	virtual void	save(IWriter& pWriter);
	virtual f32		get_length( )
	{
		f32 mn;
		f32 mx;
		return m_Value.GetLength(&mn, &mx);
	}
	virtual size_t  get_keys_count( )
	{
		return m_Value.keys.size( );
	}
};

class CPostProcessColor : public CPostProcessParam
{
protected:
	f32 m_fBase;
	CEnvelope m_Red;
	CEnvelope m_Green;
	CEnvelope m_Blue;
	SPPInfo::SColor* m_pColor;

public:
	CPostProcessColor(SPPInfo::SColor* pcolor)
	{
		m_pColor = pcolor;
	}
	virtual void	update(f32 dt)
	{
		m_pColor->r = m_Red.Evaluate(dt);
		m_pColor->g = m_Green.Evaluate(dt);
		m_pColor->b = m_Blue.Evaluate(dt);
	}
	virtual void	load(IReader& pReader);
	virtual void	save(IWriter& pWriter);
	virtual f32		get_length( )
	{
		f32 mn;
		f32 mx;
		f32 r = m_Red.GetLength(&mn, &mx);
		f32 g = m_Green.GetLength(&mn, &mx);
		f32 b = m_Blue.GetLength(&mn, &mx);
		mn = (r > g ? r : g);
		return (mn > b ? mn : b);
	}
	virtual size_t	get_keys_count( )
	{
		return m_Red.keys.size( );
	}
};

class CPostprocessAnimator :public CPostProcessEffector
{
protected:
	CPostProcessParam* m_Params[POSTPROCESS_PARAMS_COUNT];
	shared_str										m_Name;
	SPPInfo											m_EffectorParams;
	f32											m_factor;
	f32											m_dest_factor;
	bool											m_bStop;
	f32											m_factor_speed;
	bool											m_bCyclic;
	f32											m_start_time;
	f32											f_length;

	void		Update(f32 tm);

public:
	CPostprocessAnimator(s32 id, bool cyclic);
	CPostprocessAnimator( );
	virtual		~CPostprocessAnimator( );
	void		Clear( );
	void		Load(pcstr name);
	IC pcstr	Name( )
	{
		return *m_Name;
	}
	virtual void	Stop(f32 speed);
	void			SetDesiredFactor(f32 f, f32 sp);
	void			SetCurrentFactor(f32 f);
	void			SetCyclic(bool b)
	{
		m_bCyclic = b;
	}
	f32				GetLength( );
	virtual BOOL		Valid( );
	virtual BOOL		Process(SPPInfo& PPInfo);

	void			Create( );
};

class CPostprocessAnimatorLerp :public CPostprocessAnimator
{
protected:
	fastdelegate::FastDelegate0<f32>	m_get_factor_func;

public:
	void			SetFactorFunc(fastdelegate::FastDelegate0<f32> f)
	{
		m_get_factor_func = f;
	}
	virtual BOOL		Process(SPPInfo& PPInfo);
};

class CPostprocessAnimatorLerpConst :public CPostprocessAnimator
{
protected:
	f32		m_power;

public:
	CPostprocessAnimatorLerpConst( )
	{
		m_power = 1.0f;
	}
	void		SetPower(f32 val)
	{
		m_power = val;
	}
	virtual BOOL		Process(SPPInfo& PPInfo);
};

class CPostprocessAnimatorControlled :public CPostprocessAnimatorLerp
{
	CEffectorController* m_controller;

public:
	virtual				~CPostprocessAnimatorControlled( );
	CPostprocessAnimatorControlled(CEffectorController* c);
	virtual BOOL		Valid( );
};
