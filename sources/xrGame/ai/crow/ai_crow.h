////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.h
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_CROW__
#define __XRAY_AI_CROW__

#include "../../entity_alive.h"
#include "../../..\XR_3DA\SkeletonAnimated.h"

class CMotionDef;
class CBlend;
class CKinematicsAnimated;

class CAI_Crow : public CEntity
{
	typedef	CEntity inherited;
	enum ECrowStates 	{
		eUndef		= -1,
		eDeathFall	= 0,
		eDeathDead,
		eFlyIdle,
		eFlyUp
	};

	// constants and types
	enum			{ MAX_ANIM_COUNT = 8	};
	enum			{ MAX_SND_COUNT = 8		};

	// animations
	struct SAnim
	{
		typedef			svector<MotionID,MAX_ANIM_COUNT> MotionSVec;
		MotionSVec		m_Animations;
		const MotionID	&GetRandom	(){return m_Animations[Random.randI(0,m_Animations.size())];}
		void			Load		(CKinematicsAnimated* visual, pcstr prefix);
	};

	struct SSound
	{
		typedef			svector<ref_sound,MAX_SND_COUNT>		SoundSVec;
		SoundSVec		m_Sounds;
		ref_sound&		GetRandom		()	{return m_Sounds[Random.randI(0,m_Sounds.size())];}
		void			Load			(pcstr prefix);
		void			SetPosition		(const Fvector& pos);
		void			Unload			();
	};
public:
	void				OnHitEndPlaying	(CBlend* B);
protected:
	
	struct SCrowAnimations
	{
		SAnim		m_idle;
		SAnim		m_fly;
		SAnim		m_death;
		SAnim		m_death_idle;
		SAnim		m_death_dead;
	};
	SCrowAnimations	m_Anims;
	struct SCrowSounds
	{
		SSound		m_idle;
	};
	SCrowSounds		m_Sounds;

	Fvector			vOldPosition;
	ECrowStates		st_current, st_target;
	// parameters block
	Fvector			vGoalDir;
	Fvector			vCurrentDir;
	Fvector			vHPB;
	f32			fDHeading;

	// constants
	f32			fGoalChangeDelta;
	f32			fSpeed;
	f32			fASpeed;
	f32			fMinHeight;
	Fvector			vVarGoal;
	f32			fIdleSoundDelta;

	// variables
	f32			fGoalChangeTime;
	f32			fIdleSoundTime;

	// 
	bool			bPlayDeathIdle;

	void			switch2_FlyUp				();
	void			switch2_FlyIdle				();
	void			switch2_DeathFall			();
	void			switch2_DeathDead			();

	void			state_DeathFall				();
	void			state_Flying				(f32 dt);

	void			CreateSkeleton				();
	
	void			UpdateWorkload				(f32 DT);

public:						
	u32				o_workload_frame	;
	u32				o_workload_rframe	;

public:
					CAI_Crow();
	virtual			~CAI_Crow();
	virtual void	Load						(pcstr section );
			void	init						();
	virtual BOOL	net_Spawn					( CSE_Abstract* DC );
	virtual void	net_Destroy					();
	virtual BOOL	renderable_ShadowGenerate	()			{ return FALSE;	}
	virtual BOOL	renderable_ShadowReceive	()			{ return FALSE;	}
	virtual void	renderable_Render			();
	virtual void	shedule_Update				(u32 DT);
	virtual void	UpdateCL					();

	virtual CEntity*cast_entity					()			{return this;}

	virtual void	net_Export					(NET_Packet& P);
	virtual void	net_Import					(NET_Packet& P);

	virtual void	g_fireParams				(const CHudItem* /**pHudItem/**/, Fvector& /**P/**/, Fvector& /**D/**/)	{};
	virtual void	g_WeaponBones				(int &/**L/**/, int &/**R1/**/, int &/**R2/**/)	{};

	virtual void	HitSignal					(f32	HitAmount,	Fvector& local_dir, CObject* who, s16 element);
	virtual void	HitImpulse					(f32	amount,		Fvector& vWorldDir, Fvector& vLocalDir);
	virtual	void	Hit							(SHit* pHDS);
	virtual void	Die							(CObject* who);
	virtual	f32	ffGetFov					() const {return 150.0f;	}
	virtual	f32	ffGetRange					() const {return 30.0f;	}

	virtual BOOL	IsVisibleForHUD	()			{ return FALSE;		}
	virtual bool	IsVisibleForZones()			{ return false;		}
	virtual BOOL	UsedAI_Locations()			;
	virtual void	create_physic_shell	()		;
};

#endif
