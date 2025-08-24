#pragma once

#include "CameraEffects.h"

class CObjectAnimator;
class CEffectorController;
class CActor;

typedef fastdelegate::FastDelegate0<f32>		GET_KOEFF_FUNC;

void AddEffector(CActor* A, s32 type, const shared_str& sect_name);
void AddEffector(CActor* A, s32 type, const shared_str& sect_name, f32 factor);
void AddEffector(CActor* A, s32 type, const shared_str& sect_name, GET_KOEFF_FUNC);
void AddEffector(CActor* A, s32 type, const shared_str& sect_name, CEffectorController*);
void RemoveEffector(CActor* A, s32 type);

class CEffectorController
{
protected:
	CCameraEffector* m_ce;
	CPostProcessEffector* m_pe;

public:
	CEffectorController( ) : m_ce(NULL), m_pe(NULL)
	{ }
	virtual						~CEffectorController( );

	void			SetPP(CPostProcessEffector* p)
	{
		m_pe = p;
	}
	void			SetCam(CCameraEffector* p)
	{
		m_ce = p;
	}
	virtual BOOL			Valid( )
	{
		return (m_ce || m_pe);
	}
	virtual f32 xr_stdcall	GetFactor( ) = 0;
};

class CAnimatorCamEffector :public CCameraEffector
{
	typedef				CCameraEffector			inherited;
	bool				m_bCyclic;

protected:
	virtual bool		Cyclic( ) const
	{
		return m_bCyclic;
	}
	CObjectAnimator* m_objectAnimator;

public:
	bool				m_bAbsolutePositioning;

	CAnimatorCamEffector( );
	virtual				~CAnimatorCamEffector( );
	void		Start(pcstr fn);
	virtual BOOL		Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect);
	void		SetCyclic(bool b)
	{
		m_bCyclic = b;
	}
	virtual BOOL		Valid( );
	f32		GetAnimatorLength( )
	{
		return fLifeTime;
	}
};

class CAnimatorCamEffectorScriptCB :public CAnimatorCamEffector
{
	typedef	CAnimatorCamEffector			inherited;

	shared_str			cb_name;

public:
	CAnimatorCamEffectorScriptCB(pcstr _cb)
	{
		cb_name = _cb;
	}
	virtual BOOL		Valid( );
	virtual BOOL		AllowProcessingIfInvalid( )
	{
		return m_bAbsolutePositioning;
	}
	virtual void		ProcessIfInvalid(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect);
};

class CAnimatorCamLerpEffector :public CAnimatorCamEffector
{
protected:
	typedef				CAnimatorCamEffector		inherited;
	GET_KOEFF_FUNC									m_func;

public:
	void		SetFactorFunc(GET_KOEFF_FUNC f)
	{
		m_func = f;
	}
	virtual BOOL		Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect);
};

class CAnimatorCamLerpEffectorConst :public CAnimatorCamLerpEffector
{
protected:
	f32				m_factor;

public:
	CAnimatorCamLerpEffectorConst( );
	void				SetFactor(f32 v)
	{
		m_factor = v;
		clamp(m_factor, 0.0f, 1.0f);
	}
	f32 xr_stdcall	GetFactor( )
	{
		return m_factor;
	}
};

class CCameraEffectorControlled :public CAnimatorCamLerpEffector
{
	CEffectorController* m_controller;

public:
	CCameraEffectorControlled(CEffectorController* c);
	virtual				~CCameraEffectorControlled( );
	virtual BOOL		Valid( );
};

class CSndShockEffector :public CEffectorController
{
	typedef CEffectorController inherited;

public:
	f32						m_snd_length;	//ms
	f32						m_cur_length;	//ms
	f32						m_stored_volume;
	f32						m_end_time;
	f32						m_life_time;
	CActor* m_actor;

public:
	CSndShockEffector( );
	virtual						~CSndShockEffector( );
	void						Start(CActor* A, f32 snd_length, f32 power);
	void						Update( );

	virtual BOOL			Valid( );
	BOOL			InWork( );
	virtual f32 xr_stdcall	GetFactor( );
};

//////////////////////////////////////////////////////////////////////////
class CControllerPsyHitCamEffector :public CCameraEffector
{
	typedef CCameraEffector inherited;

	f32				m_time_total;
	f32				m_time_current;
	fVector3				m_dangle_target;
	fVector3				m_dangle_current;
	fVector3				m_position_source;
	fVector3				m_direction;
	f32				m_distance;

public:
	CControllerPsyHitCamEffector(ECamEffectorType type, const fVector3& src_pos, const fVector3& target_pos, f32 time);
	virtual BOOL		Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect);
};
//////////////////////////////////////////////////////////////////////////
