#ifndef PSystemH
#define PSystemH
#pragma once

#ifdef XR_PARTICLES_EXPORTS
	#define PARTICLES_API __declspec(dllexport)
#else
	#define PARTICLES_API __declspec(dllimport)
	#pragma comment(lib,"xrParticles.lib")
#endif

// Actually this must be < sqrt(MAXFLOAT) since we store this value squared.
#define P_MAXFLOAT	1.0e16f

#ifdef MAXINT
	#define P_MAXINT	MAXINT
#else
	#define P_MAXINT	0x7fffffff
#endif

#define drand48()		::Random.randF()
//#define drand48() (((f32) rand())/((f32) RAND_MAX))

namespace PAPI{
	class pVector	: public fVector3
	{
	public:
		IC 			pVector		(f32 ax, f32 ay, f32 az)	{ set(ax,ay,az);								}
		IC 			pVector		()								{}
		IC f32 	length		() const						{	return _sqrt(x*x+y*y+z*z);					}
		IC f32 	length2		() const						{	return (x*x+y*y+z*z);						}
		IC f32 	operator*	(const pVector &a) const		{	return x*a.x + y*a.y + z*a.z;				}
		IC pVector 	operator*	(const f32 s) const			{	return pVector(x*s, y*s, z*s);				}
		IC pVector 	operator/	(const f32 s) const			{
			f32 invs = 1.0f / s;	return pVector(x*invs, y*invs, z*invs);	}
		IC pVector 	operator+	(const pVector& a) const		{	return pVector(x+a.x, y+a.y, z+a.z);		}
		IC pVector 	operator-	(const pVector& a) const		{	return pVector(x-a.x, y-a.y, z-a.z);		}
		IC pVector 	operator-	()								{	x = -x;	y = -y;	z = -z;	return *this;		}
		IC pVector& operator+=	(const pVector& a)				{	x += a.x;y += a.y;z += a.z;	return *this;	}
		IC pVector& operator-=	(const pVector& a)		 		{	x -= a.x;y -= a.y;z -= a.z;	return *this;	}
		IC pVector& operator*=	(const f32 a)					{	x *= a;	y *= a;	z *= a;	return *this;		}
		IC pVector& operator/=	(const f32 a)					{	f32 b = 1.0f / a;	x *= b;	y *= b;	z *= b;	return *this;		}
		IC pVector& operator=	(const pVector& a)				{	x = a.x;y = a.y;z = a.z;return *this;		}
		IC pVector 	operator^	(const pVector& b) const		{	return pVector(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);		}
	};
	// A single particle
	struct Particle
	{
		enum{
			ANIMATE_CCW	= (1<<0),
		};
		pVector		pos;	// 12
		pVector		posB;   // 12
		pVector		vel;    // 12  	
		pVector		size;   // 12
		pVector		rot;	// 12   60
		u32			color;	// 4
		f32			age;	// 4
		u16			frame;	// 2
		flags16		flags;	// 2
	};                  	// 		72

	typedef void (* OnBirthParticleCB)	(pvoid owner, u32 param, PAPI::Particle& P, u32 idx);
	typedef void (* OnDeadParticleCB)	(pvoid owner, u32 param, PAPI::Particle& P, u32 idx);
	//////////////////////////////////////////////////////////////////////
	// Type codes for domains
	enum PDomainEnum
	{
		PDPoint 	= 0,	// Single point
		PDLine 		= 1,	// Line segment
		PDTriangle 	= 2,	// Triangle
		PDPlane 	= 3,	// Arbitrarily-oriented plane
		PDBox 		= 4,	// Axis-aligned box
		PDSphere 	= 5,	// Sphere
		PDCylinder 	= 6,	// Cylinder
		PDCone 		= 7,	// Cone
		PDBlob 		= 8,	// Gaussian blob
		PDDisc 		= 9,	// Arbitrarily-oriented disc
		PDRectangle = 10,	// Rhombus-shaped planar region
		domain_enum_force_dword = u32(-1)
	};
	//////////////////////////////////////////////////////////////////////
	// Type codes for all actions
	enum PActionEnum
	{
		PAAvoidID,			// Avoid entering the domain of space.
		PABounceID,			// Bounce particles off a domain of space.
		PACallActionListID_obsolette,	// 
		PACopyVertexBID,	// Set the secondary position from current position.
		PADampingID,		// Dampen particle velocities.
		PAExplosionID,		// An Explosion.
		PAFollowID,			// Accelerate toward the previous particle in the effect.
		PAGravitateID,		// Accelerate each particle toward each other particle.
		PAGravityID,		// Acceleration in the given direction.
		PAJetID,			// 
		PAKillOldID,		// 
		PAMatchVelocityID,	// 
		PAMoveID,			// 
		PAOrbitLineID,		// 
		PAOrbitPointID,		// 
		PARandomAccelID,	// 
		PARandomDisplaceID,	// 
		PARandomVelocityID,	// 
		PARestoreID,		// 
		PASinkID,			// 
		PASinkVelocityID,	// 
		PASourceID,			// 
		PASpeedLimitID,		// 
		PATargetColorID,	// 
		PATargetSizeID,		// 
		PATargetRotateID,	// 
		PATargetRotateDID,	// 
		PATargetVelocityID,	// 
		PATargetVelocityDID,// 
		PAVortexID,			// 
		PATurbulenceID,     //
		PAScatterID, 	    //
		action_enum_force_dword = u32(-1)
	};
	struct ParticleAction;

	class IParticleManager
	{
	public:
									IParticleManager	(){}
		virtual						~IParticleManager	(){}

		// create&destroy
		virtual s32					CreateEffect		(u32 max_particles)=0;
		virtual void				DestroyEffect		(s32 effect_id)=0;
		virtual s32					CreateActionList	()=0;
		virtual void				DestroyActionList	(s32 alist_id)=0;

		// control
		virtual void				PlayEffect			(s32 effect_id, s32 alist_id)=0;
		virtual void				StopEffect			(s32 effect_id, s32 alist_id, BOOL deffered=TRUE)=0;

		// update&render
		virtual void				Update				(s32 effect_id, s32 alist_id, f32 dt)=0;
		virtual void				Render				(s32 effect_id)=0;
		virtual void				Transform			(s32 alist_id, const fMatrix4x4& m, const fVector3& velocity)=0;

		// effect
		virtual void				RemoveParticle		(s32 effect_id, u32 p_id)=0;
		virtual void				SetMaxParticles		(s32 effect_id, u32 max_particles)=0;
		virtual void				SetCallback			(s32 effect_id, OnBirthParticleCB b, OnDeadParticleCB d, pvoid owner, u32 param)=0;
		virtual void				GetParticles		(s32 effect_id, Particle*& particles, u32& cnt)=0;
		virtual u32					GetParticlesCount	(s32 effect_id)=0;
		
		// action
		virtual ParticleAction*		CreateAction		(PActionEnum type)=0;
		virtual u32					LoadActions			(s32 alist_id, IReader& R)=0;
		virtual void				SaveActions			(s32 alist_id, IWriter& W)=0;
	};

	PARTICLES_API IParticleManager* ParticleManager		();
};
#endif //PSystemH
