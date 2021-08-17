#pragma once

#include "CameraEffector.h"

class CObjectAnimator;
class CEffectorController;
class CActor;

using GET_KOEFF_FUNC											= fastdelegate::FastDelegate0<float>;

void AddEffector(CActor* A, int type, const CSharedString& sect_name);
void AddEffector(CActor* A, int type, const CSharedString& sect_name, float factor);
void AddEffector(CActor* A, int type, const CSharedString& sect_name, GET_KOEFF_FUNC);
void AddEffector(CActor* A, int type, const CSharedString& sect_name, CEffectorController*);
void RemoveEffector(CActor* A, int type);

class CEffectorController
{
protected:
	CEffectorCam*												m_ce;
	CEffectorPP*												m_pe;

public:
								CEffectorController				( ) : m_ce(NULL), m_pe(NULL)
	{ }
	virtual						~CEffectorController			( );

	void						SetPP							(CEffectorPP* p)
	{
		m_pe = p;
	}
	void						SetCam							(CEffectorCam* p)
	{
		m_ce = p;
	}
	virtual BOOL				Valid							( )
	{
		return m_ce || m_pe;
	}
	virtual float xr_stdcall	GetFactor						( ) = 0;
};

class CAnimatorCamEffector : public CEffectorCam
{
	using inherited												= CEffectorCam;
	bool														m_bCyclic;

protected:
	virtual bool				Cyclic							( ) const
	{
		return m_bCyclic;
	}
	CObjectAnimator*											m_objectAnimator;

public:
	bool														m_bAbsolutePositioning;

								CAnimatorCamEffector			( );
	virtual						~CAnimatorCamEffector			( );
	void						Start							(const char* fn);
	virtual BOOL				Process							(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect);
	void						SetCyclic						(bool b)
	{
		m_bCyclic = b;
	}
	virtual BOOL				Valid							( );
	float						GetAnimatorLength				( )
	{
		return fLifeTime;
	}
};

class CAnimatorCamEffectorScriptCB : public CAnimatorCamEffector
{
	using inherited												= CAnimatorCamEffector;
	CSharedString													cb_name;

public:
								CAnimatorCamEffectorScriptCB	(const char* _cb)
	{
		cb_name = _cb;
	}
	virtual BOOL				Valid							( );
	virtual BOOL				AllowProcessingIfInvalid		( )
	{
		return m_bAbsolutePositioning;
	}
	virtual void				ProcessIfInvalid				(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect);
};

class CAnimatorCamLerpEffector : public CAnimatorCamEffector
{
protected:
	using inherited												= CAnimatorCamEffector;
	GET_KOEFF_FUNC												m_func;

public:
	void						SetFactorFunc					(GET_KOEFF_FUNC f)
	{
		m_func = f;
	}
	virtual BOOL				Process							(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect);
};

class CAnimatorCamLerpEffectorConst : public CAnimatorCamLerpEffector
{
protected:
	float														m_factor;

public:
								CAnimatorCamLerpEffectorConst	( );
	void						SetFactor						(float v)
	{
		m_factor = v;
		clamp(m_factor, 0.0f, 1.0f);
	}
	float xr_stdcall			GetFactor						( )
	{
		return m_factor;
	}
};

class CCameraEffectorControlled : public CAnimatorCamLerpEffector
{
	CEffectorController*										m_controller;

public:
								CCameraEffectorControlled		(CEffectorController* c);
	virtual						~CCameraEffectorControlled		( );
	virtual BOOL				Valid							( );
};

class SndShockEffector : public CEffectorController
{
	using inherited												= CEffectorController;

public:
	float														m_snd_length;	//ms
	float														m_cur_length;	//ms
	float														m_stored_volume;
	float														m_end_time;
	float														m_life_time;
	CActor*														m_actor;

								SndShockEffector				( );
	virtual						~SndShockEffector				( );
	void						Start							(CActor* A, float snd_length, float power);
	void						Update							( );

	virtual BOOL				Valid							( );
	BOOL						InWork							( );
	virtual float xr_stdcall	GetFactor						( );
};

class CControllerPsyHitCamEffector : public CEffectorCam
{
	using inherited												= CEffectorCam;

	float														m_time_total;
	float														m_time_current;
	Fvector3													m_dangle_target;
	Fvector3													m_dangle_current;
	Fvector3													m_position_source;
	Fvector3													m_direction;
	float														m_distance;

public:
								CControllerPsyHitCamEffector	(ECameraEffectorType type, const Fvector3& src_pos, const Fvector3& target_pos, float time);
	virtual BOOL				Process							(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect);
};
