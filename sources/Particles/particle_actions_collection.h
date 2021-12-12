#pragma once

#include "particle_actions.h"
#include "particle_core.h"

namespace PAPI
{
#define _METHODS	virtual void Load		(IReader& F);\
					virtual void Save		(IWriter& F);\
					virtual void Execute	(SParticleEffect* pe, float dt);\
					virtual void Transform	(const Fmatrix& m);

	struct PARTICLES_API PAAvoid : public SParticleAction
	{
		SParticleDomain		positionL;			// Avoid region (in local space)
		SParticleDomain		position;			// Avoid region
		float		look_ahead;			// how many time units ahead to look
		float		magnitude;			// what percent of the way to go each time
		float		epsilon;			// add to r^2 for softening

		_METHODS;
	};

	struct PARTICLES_API PABounce : public SParticleAction
	{
		SParticleDomain		positionL;			// Bounce region (in local space)
		SParticleDomain		position;			// Bounce region
		float		oneMinusFriction;	// Friction tangent to surface
		float		resilience;			// Resilence perpendicular to surface
		float		cutoffSqr;			// cutoff velocity; friction applies iff v > cutoff

		_METHODS;
	};

	struct PARTICLES_API PACopyVertexB : public SParticleAction
	{
		BOOL		copy_pos;			// True to copy pos to posB.

		_METHODS;
	};

	struct PARTICLES_API PADamping : public SParticleAction
	{
		CParticleVector		damping;			// Damping constant applied to velocity
		float		vlowSqr;			// Low and high cutoff velocities
		float		vhighSqr;

		_METHODS;
	};

	struct PARTICLES_API PAExplosion : public SParticleAction
	{
		CParticleVector		centerL;			// The center of the explosion (in local space)
		CParticleVector		center;				// The center of the explosion
		float		velocity;			// Of shock wave
		float		magnitude;			// At unit radius
		float		stdev;				// Sharpness or width of shock wave
		float		age;				// How long it's been going on
		float		epsilon;			// Softening parameter

		_METHODS;
	};

	struct PARTICLES_API PAFollow : public SParticleAction
	{
		float		magnitude;			// The grav of each particle
		float		epsilon;			// Softening parameter
		float		max_radius;			// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAGravitate : public SParticleAction
	{
		float		magnitude;			// The grav of each particle
		float		epsilon;			// Softening parameter
		float		max_radius;			// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAGravity : public SParticleAction
	{
		CParticleVector		directionL;			// Amount to increment velocity (in local space)
		CParticleVector		direction;			// Amount to increment velocity

		_METHODS;
	};

	struct PARTICLES_API PAJet : public SParticleAction
	{
		CParticleVector		centerL;			// Center of the fan (in local space)
		SParticleDomain		accL;				// Acceleration vector domain  (in local space)
		CParticleVector		center;				// Center of the fan
		SParticleDomain		acc;				// Acceleration vector domain
		float		magnitude;			// Scales acceleration
		float		epsilon;			// Softening parameter
		float		max_radius;			// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAKillOld : public SParticleAction
	{
		float		age_limit;			// Exact age at which to kill particles.
		BOOL		kill_less_than;		// True to kill particles less than limit.

		_METHODS;
	};

	struct PARTICLES_API PAMatchVelocity : public SParticleAction
	{
		float		magnitude;			// The grav of each particle
		float		epsilon;			// Softening parameter
		float		max_radius;			// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAMove : public SParticleAction
	{
		_METHODS;
	};

	struct PARTICLES_API PAOrbitLine : public SParticleAction
	{
		CParticleVector		pL, axisL;			// Endpoints of line to which particles are attracted (in local space)
		CParticleVector		p, axis;			// Endpoints of line to which particles are attracted
		float		magnitude;			// Scales acceleration
		float		epsilon;			// Softening parameter
		float		max_radius;			// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PAOrbitPoint : public SParticleAction
	{
		CParticleVector		centerL;			// Point to which particles are attracted (in local space)
		CParticleVector		center;				// Point to which particles are attracted
		float		magnitude;			// Scales acceleration
		float		epsilon;			// Softening parameter
		float		max_radius;			// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PARandomAccel : public SParticleAction
	{
		SParticleDomain		gen_accL;			// The domain of random accelerations.(in local space)
		SParticleDomain		gen_acc;			// The domain of random accelerations.

		_METHODS;
	};

	struct PARTICLES_API PARandomDisplace : public SParticleAction
	{
		SParticleDomain		gen_dispL;			// The domain of random displacements.(in local space)
		SParticleDomain		gen_disp;			// The domain of random displacements.

		_METHODS;
	};

	struct PARTICLES_API PARandomVelocity : public SParticleAction
	{
		SParticleDomain		gen_velL;			// The domain of random velocities.(in local space)
		SParticleDomain		gen_vel;			// The domain of random velocities.

		_METHODS;
	};

	struct PARTICLES_API PARestore : public SParticleAction
	{
		float		time_left;			// Time remaining until they should be in position.

		_METHODS;
	};

	struct PARTICLES_API PAScatter : public SParticleAction
	{
		CParticleVector		centerL;			// Center of the fan (in local space)
		CParticleVector		center;				// Center of the fan
		float		magnitude;			// Scales acceleration
		float		epsilon;			// Softening parameter
		float		max_radius;			// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PASink : public SParticleAction
	{
		BOOL		kill_inside;		// True to dispose of particles *inside* domain
		SParticleDomain		positionL;			// Disposal region (in local space)
		SParticleDomain		position;			// Disposal region

		_METHODS;
	};

	struct PARTICLES_API PASinkVelocity : public SParticleAction
	{
		BOOL		kill_inside;		// True to dispose of particles with vel *inside* domain
		SParticleDomain		velocityL;			// Disposal region (in local space)
		SParticleDomain		velocity;			// Disposal region

		_METHODS;
	};

	struct PARTICLES_API PASpeedLimit : public SParticleAction
	{
		float		min_speed;			// Clamp speed to this minimum.
		float		max_speed;			// Clamp speed to this maximum.

		_METHODS;
	};

	struct PARTICLES_API PASource : public SParticleAction
	{
		enum
		{
			flSingleSize		= (1ul << 29ul),	// True to get positionB from position.
			flSilent			= (1ul << 30ul),
			flVertexB_tracks	= (1ul << 31ul),	// True to get positionB from position.
			fl_FORCEDWORD		= unsigned int(-1)
		};
		SParticleDomain		positionL;			// Choose a position in this domain. (local_space)
		SParticleDomain		velocityL;			// Choose a velocity in this domain. (local_space)
		SParticleDomain		position;			// Choose a position in this domain.
		SParticleDomain		velocity;			// Choose a velocity in this domain.
		SParticleDomain		rot;				// Choose a rotation in this domain.
		SParticleDomain		size;				// Choose a size in this domain.
		SParticleDomain		color;				// Choose a color in this domain.
		float		alpha;				// Alpha of all generated particles
		float		particle_rate;		// Particles to generate per unit time
		float		age;				// Initial age of the particles
		float		age_sigma;			// St. dev. of initial age of the particles
		CParticleVector		parent_vel;
		float		parent_motion;

		_METHODS;
	};

	struct PARTICLES_API PATargetColor : public SParticleAction
	{
		CParticleVector		color;				// Color to shift towards
		float		alpha;				// Alpha value to shift towards
		float		scale;				// Amount to shift by (1 == all the way)

		_METHODS;
	};

	struct PARTICLES_API PATargetSize : public SParticleAction
	{
		CParticleVector		size;				// Size to shift towards
		CParticleVector		scale;				// Amount to shift by per frame (1 == all the way)

		_METHODS;
	};

	struct PARTICLES_API PATargetRotate : public SParticleAction
	{
		CParticleVector		rot;				// Rotation to shift towards
		float		scale;				// Amount to shift by per frame (1 == all the way)

		_METHODS;
	};

	struct PARTICLES_API PATargetVelocity : public SParticleAction
	{
		CParticleVector		velocityL;			// Velocity to shift towards (in local space)
		CParticleVector		velocity;			// Velocity to shift towards
		float		scale;				// Amount to shift by (1 == all the way)

		_METHODS;
	};

	struct PARTICLES_API PAVortex : public SParticleAction
	{
		CParticleVector		centerL;			// Center of vortex (in local space)
		CParticleVector		axisL;				// Axis around which vortex is applied (in local space)
		CParticleVector		center;				// Center of vortex
		CParticleVector		axis;				// Axis around which vortex is applied
		float		magnitude;			// Scale for rotation around axis
		float		epsilon;			// Softening parameter
		float		max_radius;			// Only influence particles within max_radius

		_METHODS;
	};

	struct PARTICLES_API PATurbulence : public SParticleAction
	{
		float		frequency;			// Frequency
		int			octaves;			// Octaves
		float		magnitude;			// Scale for rotation around axis
		float		epsilon;			// Softening parameter
		CParticleVector		offset;				// Offset
		float		age;

		_METHODS;
	};
};
