#pragma once

#include "../../entity.h"
#include "../../..\xrCore\FastDelegate.h"
#include "../../..\XR_3DA\SkeletonAnimated.h"

class CParticlesObject;

class CPhantom : public CEntity {
	
	typedef	CEntity inherited;
private:
	enum EState{
		stInvalid		= -2,
		stIdle			= -1,
		stBirth			= 0,
		stFly			= 1,
		stContact		= 2,
		stShoot			= 3,
		stCount
	};
	EState				m_CurState;
	EState				m_TgtState;

	void				SwitchToState_internal		(EState new_state);
	void				SwitchToState				(EState new_state){m_TgtState=new_state;}
	void __stdcall		OnIdleState					();
	void __stdcall		OnFlyState					();
	void __stdcall		OnDeadState					();

	void				UpdateFlyMedia				();

	fastdelegate::FastDelegate0<>					UpdateEvent;
private:
	struct SStateData{
		shared_str		particles;
		ref_sound		sound;
		MotionID		motion;
	};
	SStateData			m_state_data[stCount];
private:
	CParticlesObject*	m_fly_particles;
	static void			animation_end_callback	(CBlend* B);
private:
	CObject*			m_enemy;

	F32				fSpeed;
	F32				fASpeed;
	Fvector2			vHP;

	F32				fContactHit;

	Fmatrix				XFORM_center				();

	CParticlesObject*	PlayParticles				(const shared_str& name, BOOL bAutoRemove, const Fmatrix& xform);
//	void				PlayMotion					(MotionID);

	void				UpdatePosition				(const Fvector& tgt_pos);

	void				PsyHit						(const CObject *object, F32 value);
public:
						CPhantom					();
	virtual				~CPhantom					();
	
	virtual void		Load						(pcstr section );
	virtual BOOL		net_Spawn					( CSE_Abstract* DC );
	virtual void		net_Destroy					();
	
	virtual void		net_Export					(NET_Packet& P);
	virtual void		net_Import					(NET_Packet& P);
	virtual void		save						(NET_Packet &output_packet);
	virtual void		load						(IReader &input_packet);

	virtual void		shedule_Update				(u32 DT); 
	virtual void		UpdateCL					();

	virtual void		HitSignal					(F32	HitAmount,	Fvector& local_dir, CObject* who, s16 element){}
	virtual void		HitImpulse					(F32	amount,		Fvector& vWorldDir, Fvector& vLocalDir){}
	virtual	void		Hit							(SHit* pHDS);

	virtual BOOL		IsVisibleForHUD				() {return false;}
	virtual bool		IsVisibleForZones			() {return false;}

	virtual BOOL		UsedAI_Locations			() {return false;}

	virtual CEntity*	cast_entity					() {return this;}
};