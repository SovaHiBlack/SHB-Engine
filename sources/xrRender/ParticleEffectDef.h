//---------------------------------------------------------------------------
#pragma once

#include "..\XR_3DA\BasicVisual.h"
#include "..\XR_3DA\IParticleCustom.h"

namespace PAPI
{
	struct Particle;
	struct ParticleEffect;
	struct PAHeader;
	struct ParticleAction;
	DEFINE_VECTOR(ParticleAction*,PAVec,PAVecIt);
}
struct EParticleAction;        

namespace PS
{
	class CParticleEffect;

	typedef BOOL ( * CollisionCallback)(CParticleEffect* E, PAPI::Particle& P, const fVector3& pt, const fVector3& norm); // TRUE-continue collision exec
	typedef void ( * DestroyCallback)	(CParticleEffect* E, PAPI::Particle& P);

	class PFunction;
	struct SFrame
	{
		fVector2			m_fTexSize;
		fVector2			reserved;
		int     			m_iFrameDimX;
		int 				m_iFrameCount;
		f32				m_fSpeed;

		void 				InitDefault()
		{
			m_fTexSize.set	(32.f/256.f,64.f/128.f);
			m_iFrameDimX 	= 8;
			m_iFrameCount 	= 16;
			m_fSpeed		= 24.f;
		}
		IC void       		CalculateTC(int frame, fVector2& lt, fVector2& rb)
		{
			lt.x       	 	= (frame%m_iFrameDimX)*m_fTexSize.x;
			lt.y        	= (frame/m_iFrameDimX)*m_fTexSize.y;
			rb.x        	= lt.x+m_fTexSize.x;
			rb.y        	= lt.y+m_fTexSize.y;
		}
	};

	class CPEDef
	{
	public:
		enum{
			dfSprite		= (1<<0),
//			dfObject		= (1<<1),

			dfFramed		= (1<<10),
			dfAnimated		= (1<<11),
			dfRandomFrame   = (1<<12),
			dfRandomPlayback= (1<<13),
			
			dfTimeLimit		= (1<<14),

			dfAlignToPath	= (1<<15),
			dfCollision		= (1<<16),
			dfCollisionDel	= (1<<17),
			dfVelocityScale	= (1<<18),
			dfCollisionDyn	= (1<<19),
			dfWorldAlign	= (1<<20),
			dfFaceAlign		= (1<<21),
			dfCulling		= (1<<22),
			dfCullCCW		= (1<<23),
		};
		shared_str		  	m_Name;
		flags32				m_Flags;
	// texture
		shared_str		  	m_ShaderName;
		shared_str		  	m_TextureName;
		ref_shader			m_CachedShader;
		SFrame				m_Frame;
	// compiled actions
		CMemoryWriter		m_Actions;
	// def        
		f32				m_fTimeLimit;			// time limit
		int					m_MaxParticles;			// max particle count
		fVector3				m_VelocityScale;		// velocity scale
		fVector3				m_APDefaultRotation;	// align to path
	// collision
		f32 				m_fCollideOneMinusFriction;
		f32 				m_fCollideResilience;
		f32 				m_fCollideSqrCutoff;
	// execute
		void				ExecuteAnimate		(PAPI::Particle *particles, u32 p_cnt, f32 dt);
		void				ExecuteCollision	(PAPI::Particle *particles, u32 p_cnt, f32 dt, CParticleEffect* owner, CollisionCallback cb);

	public:
							CPEDef				();
							~CPEDef				();
		
		void				SetName				(pcstr name);
		IC pcstr			Name				()const{return *m_Name;}
		void				CreateShader		();
		void				DestroyShader		();

		void 				Save				(IWriter& F);
		BOOL 				Load				(IReader& F);
	};
};
//----------------------------------------------------
#define PED_VERSION				0x0001
//----------------------------------------------------
#define PED_CHUNK_VERSION		0x0001
#define PED_CHUNK_NAME			0x0002
#define PED_CHUNK_EFFECTDATA	0x0003
#define PED_CHUNK_ACTIONLIST   	0x0004
#define PED_CHUNK_FLAGS			0x0005
#define PED_CHUNK_FRAME			0x0006
#define PED_CHUNK_SPRITE	   	0x0007
#define PED_CHUNK_TIMELIMIT		0x0008
#define PED_CHUNK_TIMELIMIT2	0x0009
#define PED_CHUNK_SOURCETEXT_  	0x0020 // obsolette
#define PED_CHUNK_COLLISION	   	0x0021
#define PED_CHUNK_VEL_SCALE		0x0022
#define PED_CHUNK_OWNER			0x0023
#define PED_CHUNK_EDATA			0x0024
#define PED_CHUNK_ALIGN_TO_PATH	0x0025
//---------------------------------------------------------------------------
