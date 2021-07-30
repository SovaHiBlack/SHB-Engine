////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.h
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../EntityAlive.h"
#include "../../..\ENGINE\SkeletonAnimated.h"

class CMotionDef;
class CBlend;
class CKinematicsAnimated;

class CCrow : public CEntity
{
	typedef	CEntity inherited;
	enum ECrowStates
	{
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
		void			Load		(CKinematicsAnimated* visual, const char* prefix);
	};

	struct SSound
	{
		typedef			svector<ref_sound,MAX_SND_COUNT>		SoundSVec;
		SoundSVec		m_Sounds;
		ref_sound&		GetRandom		()	{return m_Sounds[Random.randI(0,m_Sounds.size())];}
		void			Load			(const char* prefix);
		void			SetPosition		(const Fvector3& pos);
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

	Fvector3			vOldPosition;
	ECrowStates		st_current, st_target;
	// parameters block
	Fvector3			vGoalDir;
	Fvector3			vCurrentDir;
	Fvector3			vHPB;
	float			fDHeading;

	// constants
	float			fGoalChangeDelta;
	float			fSpeed;
	float			fASpeed;
	float			fMinHeight;
	Fvector3			vVarGoal;
	float			fIdleSoundDelta;

	// variables
	float			fGoalChangeTime;
	float			fIdleSoundTime;

	// 
	bool			bPlayDeathIdle;

	void			switch2_FlyUp				();
	void			switch2_FlyIdle				();
	void			switch2_DeathFall			();
	void			switch2_DeathDead			();

	void			state_DeathFall				();
	void			state_Flying				(float dt);

	void			CreateSkeleton				();
	
	void			UpdateWorkload				(float DT);

public:						
	u32				o_workload_frame	;
	u32				o_workload_rframe	;

public:
	CCrow();
	virtual			~CCrow();
	virtual void	Load						(const char* section );
			void	init						();
	virtual BOOL	net_Spawn					( CSE_Abstract* DC );
	virtual void	net_Destroy					();
	virtual bool	renderable_ShadowGenerate	()			{ return false;	}
	virtual bool	renderable_ShadowReceive	()			{ return false;	}
	virtual void	renderable_Render			();
	virtual void	shedule_Update				(u32 DT);
	virtual void	UpdateCL					();

	virtual CEntity*cast_entity					()			{return this;}

	virtual void	net_Export					(CNetPacket& P);
	virtual void	net_Import					(CNetPacket& P);

	virtual void	g_fireParams				(const CHudItem* /**pHudItem/**/, Fvector3& /**P/**/, Fvector3& /**D/**/)	{};
	virtual void	g_WeaponBones				(int &/**L/**/, int &/**R1/**/, int &/**R2/**/)	{};

	virtual void	HitSignal					(float	HitAmount, Fvector3& local_dir, CObject* who, S16 element);
	virtual void	HitImpulse					(float	amount, Fvector3& vWorldDir, Fvector3& vLocalDir);
	virtual	void	Hit							(SHit* pHDS);
	virtual void	Die							(CObject* who);
	virtual	float	ffGetFov					() const {return 150.0f;	}
	virtual	float	ffGetRange					() const {return 30.0f;	}

	virtual BOOL	IsVisibleForHUD	()			{ return FALSE;		}
	virtual bool	IsVisibleForZones()			{ return false;		}
	virtual BOOL	UsedAI_Locations()			;
	virtual void	create_physic_shell	()		;
};
