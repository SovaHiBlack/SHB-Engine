//---------------------------------------------------------------------------
#pragma once

#include "ParticleEffectDef.h"

namespace PS
{
	class CParticleEffect: public IParticleCustom
	{
		friend class CPEDef;
	protected:
		CPEDef*				m_Def;

		f32				m_fElapsedLimit;

		int					m_HandleEffect;
		int					m_HandleActionList;

		s32					m_MemDT;

		Fvector				m_InitialPosition;
	public:
		fMatrix4x4				m_XFORM;
    protected:
    	DestroyCallback		m_DestroyCallback;
        CollisionCallback	m_CollisionCallback;
	public:
		enum{
			flRT_Playing		= (1<<0),
			flRT_DefferedStop	= (1<<1),
			flRT_XFORM			= (1<<2),
		};
		Flags8				m_RT_Flags;
	protected:
		void				RefreshShader		();

	public:
							CParticleEffect		();
		virtual 			~CParticleEffect	();

		void	 			OnFrame				(u32 dt);

		virtual void		Render				(f32 LOD);
		virtual void		Copy				(IRender_Visual* pFrom);

		virtual void 		OnDeviceCreate		();
		virtual void 		OnDeviceDestroy		();

		virtual void		UpdateParent		(const fMatrix4x4& m, const Fvector& velocity, BOOL bXFORM);

		BOOL				Compile				(CPEDef* def);

		IC CPEDef*			GetDefinition		(){return m_Def;}
		IC int				GetHandleEffect		(){return m_HandleEffect;}
		IC int				GetHandleActionList	(){return m_HandleActionList;}

		virtual void		Play				();
		virtual void		Stop				(BOOL bDefferedStop=TRUE);
		virtual BOOL		IsPlaying			(){return m_RT_Flags.is(flRT_Playing);}

		virtual f32			GetTimeLimit		(){VERIFY(m_Def); return m_Def->m_Flags.is(CPEDef::dfTimeLimit)?m_Def->m_fTimeLimit:-1.f;}

		virtual const shared_str	Name			(){VERIFY(m_Def); return m_Def->m_Name;}

        void				SetDestroyCB		(DestroyCallback 	destroy_cb)		{m_DestroyCallback 	= destroy_cb;}
        void				SetCollisionCB		(CollisionCallback	collision_cb)	{m_CollisionCallback= collision_cb;}
        void				SetBirthDeadCB		(PAPI::OnBirthParticleCB bc, PAPI::OnDeadParticleCB dc, void* owner, u32 p);		

	    virtual u32			ParticlesCount		();
	};
	DEFINE_VECTOR				(PS::CPEDef*,PEDVec,PEDIt);
    void OnEffectParticleBirth	(void* owner, u32 param, PAPI::Particle& m, u32 idx);
    void OnEffectParticleDead	(void* owner, u32 param, PAPI::Particle& m, u32 idx);

    extern const u32		uDT_STEP;
	extern const f32		fDT_STEP;
}
//---------------------------------------------------------------------------
