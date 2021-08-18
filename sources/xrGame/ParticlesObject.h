#pragma once

#include "..\ENGINE\PS_instance.h"

extern const Fvector3 zero_vel;

class CParticlesObject		:	public CPS_Instance
{
	typedef CPS_Instance	inherited;

	u32					dwLastTime;
	void				Init				(const char* p_name, IRender_Sector* S, BOOL bAutoRemove);
	void				UpdateSpatial		();

protected:
	bool				m_bLooped;			//флаг, что система зациклена
	bool				m_bStopping;		//вызвана функция Stop()

protected:
	u32					mt_dt;

protected:
	virtual				~CParticlesObject	();

public:
						CParticlesObject	(const char* p_name, BOOL bAutoRemove, bool destroy_on_game_load);

	virtual bool		shedule_Needed		()	{return true;};
	virtual float		shedule_Scale		()	;
	virtual void		shedule_Update		(u32 dt);
	virtual void		renderable_Render	();
	void				PerformAllTheWork	(u32 dt);
	void	__stdcall	PerformAllTheWork_mt();

	Fvector3&			Position			();
	void				SetXFORM			(const Fmatrix& m);
	inline	Fmatrix&		XFORM				()	{return renderable.xform;}
	void				UpdateParent		(const Fmatrix& m, const Fvector3& vel);

	void				play_at_pos			(const Fvector3& pos, BOOL xform=FALSE);
	virtual void		Play				();
	void				Stop				(BOOL bDefferedStop=TRUE);
	virtual BOOL		Locked				()				{ return mt_dt; }
	
	bool				IsLooped			() {return m_bLooped;}
	bool				IsAutoRemove		();
	bool				IsPlaying			();
	void				SetAutoRemove		(bool auto_remove);

	const CSharedString			Name		();

public:
	static CParticlesObject*	Create		(const char* p_name, BOOL bAutoRemove=TRUE, bool remove_on_game_load = true)
	{
		return xr_new<CParticlesObject>(p_name, bAutoRemove, remove_on_game_load);
	}
	static void					Destroy		(CParticlesObject*& p)
	{
		if (p){ 
			p->PSI_destroy		();
			p					= 0;
		}
	}
};
