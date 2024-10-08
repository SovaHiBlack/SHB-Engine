#pragma once

#include "CameraEffector.h"

class CObjectAnimator;
class CEffectorController;
class CActor;

typedef fastdelegate::FastDelegate0<F32>		GET_KOEFF_FUNC;

void AddEffector		(CActor* A, int type, const shared_str& sect_name);
void AddEffector		(CActor* A, int type, const shared_str& sect_name, F32 factor);
void AddEffector		(CActor* A, int type, const shared_str& sect_name, GET_KOEFF_FUNC);
void AddEffector		(CActor* A, int type, const shared_str& sect_name, CEffectorController*);
void RemoveEffector		(CActor* A, int type);

class CEffectorController
{
protected:
	CEffectorCam*				m_ce;
	CEffectorPP*				m_pe;
public:
								CEffectorController():m_ce(NULL),m_pe(NULL)	{}
	virtual						~CEffectorController();

				void			SetPP		(CEffectorPP* p)				{m_pe=p;}
				void			SetCam		(CEffectorCam* p)				{m_ce=p;}
	virtual		BOOL			Valid		()								{return m_ce||m_pe;};
	virtual	F32 xr_stdcall	GetFactor	()								=0;
};

class CAnimatorCamEffector :public CEffectorCam
{
	typedef				CEffectorCam			inherited;
	bool				m_bCyclic;
protected:
	virtual bool		Cyclic					() const		{return m_bCyclic;}
	CObjectAnimator*							m_objectAnimator;
public:
	bool				m_bAbsolutePositioning;

						CAnimatorCamEffector	();
	virtual				~CAnimatorCamEffector	();
			void		Start					(pcstr fn);
	virtual	BOOL		Process					(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect);
			void		SetCyclic				(bool b)				{m_bCyclic=b;}
	virtual	BOOL		Valid					();
	F32		GetAnimatorLength		()						{return fLifeTime;};
};

class CAnimatorCamEffectorScriptCB :public CAnimatorCamEffector 
{
	typedef 	CAnimatorCamEffector			inherited;

	shared_str			cb_name;
public:
	CAnimatorCamEffectorScriptCB	(pcstr _cb){cb_name =_cb;};
	virtual	BOOL		Valid					();
	virtual BOOL		AllowProcessingIfInvalid()	{return m_bAbsolutePositioning;}
	virtual	void		ProcessIfInvalid		(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect);
};

class CAnimatorCamLerpEffector :public CAnimatorCamEffector
{
protected:
	typedef				CAnimatorCamEffector		inherited;
	GET_KOEFF_FUNC									m_func;
public:
			void		SetFactorFunc				(GET_KOEFF_FUNC f)	{m_func=f;}
	virtual	BOOL		Process						(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect);
};

class CAnimatorCamLerpEffectorConst :public CAnimatorCamLerpEffector
{
protected:
	F32				m_factor;
public:
						CAnimatorCamLerpEffectorConst	();
	void				SetFactor						(F32 v)		{m_factor=v; clamp(m_factor,0.0f,1.0f);}
	F32	xr_stdcall	GetFactor						()				{return m_factor;}
};

class CCameraEffectorControlled :public CAnimatorCamLerpEffector
{
	CEffectorController*		m_controller;
public:
						CCameraEffectorControlled		(CEffectorController* c);
	virtual				~CCameraEffectorControlled		();
	virtual BOOL		Valid							();
};

class SndShockEffector:public CEffectorController
{
	typedef CEffectorController inherited;
public:
	F32						m_snd_length;	//ms
	F32						m_cur_length;	//ms
	F32						m_stored_volume;
	F32						m_end_time;
	F32						m_life_time;
	CActor*						m_actor;
public:
								SndShockEffector	();
	virtual						~SndShockEffector	();
	void						Start				(CActor* A, F32 snd_length, F32 power);
	void						Update				();

	virtual		BOOL			Valid				();
				BOOL			InWork				();
	virtual	F32 xr_stdcall	GetFactor			();
};


//////////////////////////////////////////////////////////////////////////
class CControllerPsyHitCamEffector :public CEffectorCam {
	typedef CEffectorCam inherited;
	
	F32				m_time_total;
	F32				m_time_current;
	Fvector				m_dangle_target;
	Fvector				m_dangle_current;
	Fvector				m_position_source;
	Fvector				m_direction;
	F32				m_distance;

public:
						CControllerPsyHitCamEffector	(ECamEffectorType type, const Fvector &src_pos, const Fvector &target_pos, F32 time);
	virtual	BOOL		Process							(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect);
};
//////////////////////////////////////////////////////////////////////////
