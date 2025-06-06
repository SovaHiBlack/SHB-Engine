#pragma once

#include "..\XR_3DA\IParticleCustom.h"

namespace PS
{
	class CParticleEffect;

	class CPGDef
	{
	public:
		shared_str			m_Name;
		flags32				m_Flags;
		f32				m_fTimeLimit;
		struct SEffect{
			enum{
				flDefferedStop		= (1<<0),
				flOnPlayChild		= (1<<1),
				flEnabled			= (1<<2),
				flOnPlayChildRewind	= (1<<4),
				flOnBirthChild		= (1<<5),
				flOnDeadChild		= (1<<6),
			};
			flags32			m_Flags;
			shared_str		m_EffectName;  
			shared_str		m_OnPlayChildName;
			shared_str		m_OnBirthChildName;
			shared_str		m_OnDeadChildName;
			f32			m_Time0;
			f32			m_Time1;
							SEffect				(){m_Flags.zero();/*set(flEnabled)*/m_Time0=0;m_Time1=0;}
		};
		DEFINE_VECTOR(SEffect*,EffectVec,EffectIt);
		EffectVec			m_Effects;

	public:
							CPGDef		  	();
							~CPGDef		  	();
		void				SetName		  	(pcstr name);

		void 				Save		  	(IWriter& F);
		BOOL 				Load		 	(IReader& F);
	};
	DEFINE_VECTOR(CPGDef*,PGDVec,PGDIt);

	class CParticleGroup: public IParticleCustom
	{
		const CPGDef*		m_Def;
		f32				m_CurrentTime;
		fVector3				m_InitialPosition;

	public:
		DEFINE_VECTOR(IRenderVisual*,VisualVec,VisualVecIt);
		struct SItem		{
			IRenderVisual*	_effect;
			VisualVec		_children_related;
			VisualVec		_children_free;
		public:
			void			Set				(IRenderVisual* e);
			void			Clear			();

			IC u32			GetVisuals		(xr_vector<IRenderVisual*>& visuals)
			{
				visuals.reserve				(_children_related.size()+_children_free.size()+1);
				if (_effect)				visuals.push_back(_effect);
				visuals.insert				(visuals.end(),_children_related.begin(),_children_related.end());
				visuals.insert				(visuals.end(),_children_free.begin(),_children_free.end());
				return visuals.size();
			}
			
			void			OnDeviceCreate	();
			void			OnDeviceDestroy	();

			void			StartRelatedChild	(CParticleEffect* emitter, pcstr eff_name, PAPI::Particle& m);
			void			StopRelatedChild	(u32 idx);
			void			StartFreeChild		(CParticleEffect* emitter, pcstr eff_name, PAPI::Particle& m);

			void 			UpdateParent	(const fMatrix4x4& m, const fVector3& velocity, BOOL bXFORM);
			void			OnFrame			(u32 u_dt, const CPGDef::SEffect& def, fBox3& box, bool& bPlaying);

			u32				ParticlesCount	();
			BOOL			IsPlaying		();
			void			Play			();
			void			Stop			(BOOL def_stop);
		};
		DEFINE_VECTOR(SItem,SItemVec,SItemVecIt)
		SItemVec			items;
	public:
		enum{
			flRT_Playing		= (1<<0),
			flRT_DefferedStop	= (1<<1),
		};
		flags8				m_RT_Flags;

	public:
		CParticleGroup	();
		virtual				~CParticleGroup	();
		virtual void	 	OnFrame			(u32 dt);

		virtual void		Copy			(IRenderVisual* pFrom) {FATAL("Can't duplicate particle system - NOT IMPLEMENTED");}

		virtual void 		OnDeviceCreate	();
		virtual void 		OnDeviceDestroy	();

		virtual void		UpdateParent	(const fMatrix4x4& m, const fVector3& velocity, BOOL bXFORM);

		BOOL				Compile			(CPGDef* def);

		const CPGDef*		GetDefinition	(){return m_Def;}

		virtual void		Play			();
		virtual void		Stop			(BOOL bDefferedStop=TRUE);
		virtual BOOL		IsPlaying		(){return m_RT_Flags.is(flRT_Playing);}

		virtual f32		GetTimeLimit	(){VERIFY(m_Def); return m_Def->m_fTimeLimit;}

		virtual const shared_str	Name		(){VERIFY(m_Def); return m_Def->m_Name;}

		virtual u32 		ParticlesCount	();
	};

}
#define PGD_VERSION				0x0003
//----------------------------------------------------
#define PGD_CHUNK_VERSION		0x0001
#define PGD_CHUNK_NAME			0x0002
#define PGD_CHUNK_FLAGS			0x0003
#define PGD_CHUNK_EFFECTS		0x0004 // obsolette
#define PGD_CHUNK_TIME_LIMIT	0x0005
#define PGD_CHUNK_OWNER			0x0006
#define PGD_CHUNK_EFFECTS2		0x0007
//---------------------------------------------------------------------------
