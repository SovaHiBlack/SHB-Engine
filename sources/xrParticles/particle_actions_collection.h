//---------------------------------------------------------------------------
#ifndef particle_actions_collectionH
#define particle_actions_collectionH

#include "particle_actions.h"
#include "particle_core.h"
namespace PAPI{
#define _METHODS	virtual void 	Load		(IReader& F);\
					virtual void 	Save		(IWriter& F);\
					virtual void 	Execute		(ParticleEffect *pe, f32 dt);\
					virtual void 	Transform	(const fMatrix4x4& m);

	struct PARTICLES_API PAAvoid : public ParticleAction
	{
		pDomain positionL;	// Avoid region (in local space)
		pDomain position;	// Avoid region
		f32 look_ahead;	// how many time units ahead to look
		f32 magnitude;	// what percent of the way to go each time
		f32 epsilon;		// add to r^2 for softening

		_METHODS;
	};

	struct PARTICLES_API PABounce : public ParticleAction
	{
		pDomain positionL;	// Bounce region (in local space)
		pDomain position;	// Bounce region
		f32 oneMinusFriction;	// Friction tangent to surface
		f32 resilience;	// Resilence perpendicular to surface
		f32 cutoffSqr;	// cutoff velocity; friction applies iff v > cutoff

		_METHODS;
	};

	struct PARTICLES_API PACopyVertexB : public ParticleAction
	{
		BOOL copy_pos;		// True to copy pos to posB.

		_METHODS;
	};

	struct PARTICLES_API PADamping : public ParticleAction
	{
		pVector damping;	// Damping constant applied to velocity
		f32 vlowSqr;		// Low and high cutoff velocities
		f32 vhighSqr;

		_METHODS;
	};

	struct PARTICLES_API PAExplosion : public ParticleAction
	{
		pVector centerL;	// The center of the explosion (in local space)
		pVector center;		// The center of the explosion
		f32 velocity;		// Of shock wave
		f32 magnitude;	// At unit radius
		f32 stdev;		// Sharpness or width of shock wave
		f32 age;			// How long it's been going on
		f32 epsilon;		// Softening parameter

		_METHODS;
	};

	struct PARTICLES_API PAFollow : public ParticleAction
	{
		f32 magnitude;	// The grav of each particle
		f32 epsilon;		// Softening parameter
		f32 max_radius;	// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAGravitate : public ParticleAction
	{
		f32 magnitude;	// The grav of each particle
		f32 epsilon;		// Softening parameter
		f32 max_radius;	// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAGravity : public ParticleAction
	{
		pVector directionL;	// Amount to increment velocity (in local space)
		pVector direction;	// Amount to increment velocity

		_METHODS;
	};

	struct PARTICLES_API PAJet : public ParticleAction
	{
		pVector	centerL;	// Center of the fan (in local space)
		pDomain accL;		// Acceleration vector domain  (in local space)
		pVector	center;		// Center of the fan
		pDomain acc;		// Acceleration vector domain
		f32 magnitude;	// Scales acceleration
		f32 epsilon;		// Softening parameter
		f32 max_radius;	// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAKillOld : public ParticleAction
	{
		f32 age_limit;		// Exact age at which to kill particles.
		BOOL kill_less_than;	// True to kill particles less than limit.

		_METHODS;
	};

	struct PARTICLES_API PAMatchVelocity : public ParticleAction
	{
		f32 magnitude;	// The grav of each particle
		f32 epsilon;		// Softening parameter
		f32 max_radius;	// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAMove : public ParticleAction
	{
		_METHODS;
	};

	struct PARTICLES_API PAOrbitLine : public ParticleAction
	{
		pVector pL, axisL;	// Endpoints of line to which particles are attracted (in local space)
		pVector p, axis;	// Endpoints of line to which particles are attracted
		f32 magnitude;	// Scales acceleration
		f32 epsilon;		// Softening parameter
		f32 max_radius;	// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAOrbitPoint : public ParticleAction
	{
		pVector centerL;	// Point to which particles are attracted (in local space)
		pVector center;		// Point to which particles are attracted
		f32 magnitude;	// Scales acceleration
		f32 epsilon;		// Softening parameter
		f32 max_radius;	// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PARandomAccel : public ParticleAction
	{
		pDomain gen_accL;	// The domain of random accelerations.(in local space)
		pDomain gen_acc;	// The domain of random accelerations.

		_METHODS;
	};

	struct PARTICLES_API PARandomDisplace : public ParticleAction
	{
		pDomain gen_dispL;	// The domain of random displacements.(in local space)
		pDomain gen_disp;	// The domain of random displacements.

		_METHODS;
	};

	struct PARTICLES_API PARandomVelocity : public ParticleAction
	{
		pDomain gen_velL;	// The domain of random velocities.(in local space)
		pDomain gen_vel;	// The domain of random velocities.

		_METHODS;
	};

	struct PARTICLES_API PARestore : public ParticleAction
	{
		f32 time_left;	// Time remaining until they should be in position.

		_METHODS;
	};

	struct PARTICLES_API PAScatter : public ParticleAction
	{
		pVector	centerL;	// Center of the fan (in local space)
		pVector	center;		// Center of the fan
		f32 magnitude;	// Scales acceleration
		f32 epsilon;		// Softening parameter
		f32 max_radius;	// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PASink : public ParticleAction
	{
		BOOL kill_inside;	// True to dispose of particles *inside* domain
		pDomain positionL;	// Disposal region (in local space)
		pDomain position;	// Disposal region

		_METHODS;
	};

	struct PARTICLES_API PASinkVelocity : public ParticleAction
	{
		BOOL kill_inside;	// True to dispose of particles with vel *inside* domain
		pDomain velocityL;	// Disposal region (in local space)
		pDomain velocity;	// Disposal region

		_METHODS;
	};

	struct PARTICLES_API PASpeedLimit : public ParticleAction
	{
		f32 min_speed;		// Clamp speed to this minimum.
		f32 max_speed;		// Clamp speed to this maximum.

		_METHODS;
	};

	struct PARTICLES_API PASource : public ParticleAction
	{
		enum{
			flSingleSize		= (1ul<<29ul),// True to get positionB from position.
			flSilent			= (1ul<<30ul),
			flVertexB_tracks	= (1ul<<31ul),// True to get positionB from position.
			fl_FORCEDWORD		= u32(-1)
		};
		pDomain positionL;	// Choose a position in this domain. (local_space)
		pDomain velocityL;	// Choose a velocity in this domain. (local_space)
		pDomain position;	// Choose a position in this domain.
		pDomain velocity;	// Choose a velocity in this domain.
		pDomain rot;		// Choose a rotation in this domain.
		pDomain size;		// Choose a size in this domain.
		pDomain color;		// Choose a color in this domain.
		f32 alpha;		// Alpha of all generated particles
		f32 particle_rate;// Particles to generate per unit time
		f32 age;			// Initial age of the particles
		f32 age_sigma;	// St. dev. of initial age of the particles
		pVector parent_vel;	
		f32 parent_motion;

		_METHODS;
	};

	struct PARTICLES_API PATargetColor : public ParticleAction
	{
		pVector color;		// Color to shift towards
		f32 alpha;		// Alpha value to shift towards
		f32 scale;		// Amount to shift by (1 == all the way)

		_METHODS;
	};

	struct PARTICLES_API PATargetSize : public ParticleAction
	{
		pVector size;		// Size to shift towards
		pVector scale;		// Amount to shift by per frame (1 == all the way)

		_METHODS;
	};

	struct PARTICLES_API PATargetRotate : public ParticleAction
	{
		pVector rot;		// Rotation to shift towards
		f32 scale;		// Amount to shift by per frame (1 == all the way)

		_METHODS;
	};

	struct PARTICLES_API PATargetVelocity : public ParticleAction
	{
		pVector velocityL;	// Velocity to shift towards (in local space)
		pVector velocity;	// Velocity to shift towards
		f32 scale;		// Amount to shift by (1 == all the way)

		_METHODS;
	};

	struct PARTICLES_API PAVortex : public ParticleAction
	{
		pVector centerL;	// Center of vortex (in local space)
		pVector axisL;		// Axis around which vortex is applied (in local space)
		pVector center;		// Center of vortex
		pVector axis;		// Axis around which vortex is applied
		f32 magnitude;	// Scale for rotation around axis
		f32 epsilon;		// Softening parameter
		f32 max_radius;	// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PATurbulence : public ParticleAction
	{
		f32 frequency;	// Frequency
		int	octaves;		// Octaves
		f32 magnitude;	// Scale for rotation around axis
		f32 epsilon;		// Softening parameter
		pVector offset;		// Offset
		f32 age;

		_METHODS;
	};
};

//---------------------------------------------------------------------------
#endif
