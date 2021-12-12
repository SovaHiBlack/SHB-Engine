#pragma once

#ifdef PARTICLES_EXPORTS
#	define PARTICLES_API __declspec(dllexport)
#else
#	define PARTICLES_API __declspec(dllimport)
#	pragma comment(lib, "Particles.lib")
#endif

// Actually this must be < sqrt(MAXFLOAT) since we store this value squared.
#define P_MAXFLOAT		1.0e16f

#ifdef MAXINT
#	define P_MAXINT		MAXINT
#else
#	define P_MAXINT		0x7fffffff
#endif

#define drand48()		::Random.randF()

namespace PAPI
{
	class CParticleVector : public Fvector3
	{
	public:
		inline						CParticleVector		(float ax, float ay, float az)
		{
			set(ax, ay, az);
		}
		inline						CParticleVector		( )
		{ }
		inline float				length				( ) const
		{
			return _sqrt(x * x + y * y + z * z);
		}
		inline float				length2				( ) const
		{
			return (x * x + y * y + z * z);
		}
		inline float				operator*			(const CParticleVector& a) const
		{
			return x * a.x + y * a.y + z * a.z;
		}
		inline CParticleVector		operator*			(const float s) const
		{
			return CParticleVector(x * s, y * s, z * s);
		}
		inline CParticleVector		operator/			(const float s) const
		{
			float invs = 1.0f / s;
			return CParticleVector(x * invs, y * invs, z * invs);
		}
		inline CParticleVector		operator+			(const CParticleVector& a) const
		{
			return CParticleVector(x + a.x, y + a.y, z + a.z);
		}
		inline CParticleVector		operator-			(const CParticleVector& a) const
		{
			return CParticleVector(x - a.x, y - a.y, z - a.z);
		}
		inline CParticleVector		operator-			( )
		{
			x = -x;
			y = -y;
			z = -z;
			return *this;
		}
		inline CParticleVector&		operator+=			(const CParticleVector& a)
		{
			x += a.x;
			y += a.y;
			z += a.z;
			return *this;
		}
		inline CParticleVector&		operator-=			(const CParticleVector& a)
		{
			x -= a.x;
			y -= a.y;
			z -= a.z;
			return *this;
		}
		inline CParticleVector&		operator*=			(const float a)
		{
			x *= a;
			y *= a;
			z *= a;
			return *this;
		}
		inline CParticleVector&		operator/=			(const float a)
		{
			float b = 1.0f / a;
			x *= b;
			y *= b;
			z *= b;
			return *this;
		}
		inline CParticleVector&		operator=			(const CParticleVector& a)
		{
			x = a.x;
			y = a.y;
			z = a.z;
			return *this;
		}
		inline CParticleVector		operator^			(const CParticleVector& b) const
		{
			return CParticleVector(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
		}
	};

	// A single particle
	struct SParticle
	{
		enum
		{
			ANIMATE_CCW = (1 << 0)
		};
		CParticleVector		pos;	// 12
		CParticleVector		posB;	// 12
		CParticleVector		vel;	// 12
		CParticleVector		size;	// 12
		CParticleVector		rot;	// 12	60
		unsigned int		color;	// 4
		float				age;	// 4
		unsigned short		frame;	// 2
		Flags16				flags;	// 2
	};						//		72

	typedef void (*OnBirthParticleCB)	(void* owner, unsigned int param, PAPI::SParticle& P, unsigned int idx);
	typedef void (*OnDeadParticleCB)	(void* owner, unsigned int param, PAPI::SParticle& P, unsigned int idx);
	//////////////////////////////////////////////////////////////////////
	// Type codes for domains
	enum EParticleDomain
	{
		PDPoint						= 0,	// Single point
		PDLine						= 1,	// Line segment
		PDTriangle					= 2,	// Triangle
		PDPlane						= 3,	// Arbitrarily-oriented plane
		PDBox						= 4,	// Axis-aligned box
		PDSphere					= 5,	// Sphere
		PDCylinder					= 6,	// Cylinder
		PDCone						= 7,	// Cone
		PDBlob						= 8,	// Gaussian blob
		PDDisc						= 9,	// Arbitrarily-oriented disc
		PDRectangle					= 10,	// Rhombus-shaped planar region
		domain_enum_force_dword		= unsigned int(-1)
	};
	//////////////////////////////////////////////////////////////////////
	// Type codes for all actions
	enum EParticleAction
	{
		PAAvoidID,							// Avoid entering the domain of space.
		PABounceID,							// Bounce particles off a domain of space.
		PACallActionListID_obsolette,		// 
		PACopyVertexBID,					// Set the secondary position from current position.
		PADampingID,						// Dampen particle velocities.
		PAExplosionID,						// An Explosion.
		PAFollowID,							// Accelerate toward the previous particle in the effect.
		PAGravitateID,						// Accelerate each particle toward each other particle.
		PAGravityID,						// Acceleration in the given direction.
		PAJetID,							// 
		PAKillOldID,						// 
		PAMatchVelocityID,					// 
		PAMoveID,							// 
		PAOrbitLineID,						// 
		PAOrbitPointID,						// 
		PARandomAccelID,					// 
		PARandomDisplaceID,					// 
		PARandomVelocityID,					// 
		PARestoreID,						// 
		PASinkID,							// 
		PASinkVelocityID,					// 
		PASourceID,							// 
		PASpeedLimitID,						// 
		PATargetColorID,					// 
		PATargetSizeID,						// 
		PATargetRotateID,					// 
		PATargetRotateDID,					// 
		PATargetVelocityID,					// 
		PATargetVelocityDID,				// 
		PAVortexID,							// 
		PATurbulenceID,						//
		PAScatterID,						//
		action_enum_force_dword = unsigned int(-1)
	};

	struct SParticleAction;

	class IParticleManager
	{
	public:
									IParticleManager		( )
		{ }
		virtual						~IParticleManager		( )
		{ }

		// Create&Destroy
		virtual int					CreateEffect			(unsigned int max_particles) = 0;
		virtual void				DestroyEffect			(int effect_id) = 0;
		virtual int					CreateActionList		( ) = 0;
		virtual void				DestroyActionList		(int alist_id) = 0;

		// Control
		virtual void				PlayEffect				(int effect_id, int alist_id) = 0;
		virtual void				StopEffect				(int effect_id, int alist_id, BOOL deffered = TRUE) = 0;

		// Update&Render
		virtual void				Update					(int effect_id, int alist_id, float dt) = 0;
		virtual void				Render					(int effect_id) = 0;
		virtual void				Transform				(int alist_id, const Fmatrix& m, const Fvector3& velocity) = 0;

		// Effect
		virtual void				RemoveParticle			(int effect_id, unsigned int p_id) = 0;
		virtual void				SetMaxParticles			(int effect_id, unsigned int max_particles) = 0;
		virtual void				SetCallback				(int effect_id, OnBirthParticleCB b, OnDeadParticleCB d, void* owner, u32 param) = 0;
		virtual void				GetParticles			(int effect_id, SParticle*& particles, unsigned int& cnt) = 0;
		virtual unsigned int		GetParticlesCount		(int effect_id) = 0;

		// Action
		virtual SParticleAction*	CreateAction			(EParticleAction type) = 0;
		virtual unsigned int		LoadActions				(int alist_id, IReader& R) = 0;
		virtual void				SaveActions				(int alist_id, IWriter& W) = 0;
	};

	PARTICLES_API IParticleManager*	ParticleManager			( );
};
