#pragma once

namespace PAPI
{
	// A effect of particles - Info and an array of Particles
	struct SParticleEffect
	{
		unsigned int			p_count;				// Number of particles currently existing
		unsigned int			max_particles;			// Max particles allowed in effect
		unsigned int			particles_allocated;	// Actual allocated size
		SParticle*				particles;				// Actually, num_particles in size
		OnBirthParticleCB		b_cb;
		OnDeadParticleCB		d_cb;
		void*					owner;
		unsigned int			param;

	public:
		SParticleEffect(int mp)
		{
			owner = nullptr;
			param = 0;
			b_cb = 0;
			d_cb = 0;
			p_count = 0;
			max_particles = mp;
			particles_allocated = max_particles;
			particles = xr_alloc<SParticle>(max_particles);
		}
		~SParticleEffect( )
		{
			xr_free(particles);
		}
		inline int Resize(unsigned int max_count)
		{
			// Reducing max
			if (particles_allocated >= max_count)
			{
				max_particles = max_count;

				// May have to kill particles
				if (p_count > max_particles)
				{
					p_count = max_particles;
				}

				return max_count;
			}

			// Allocate particles.
			SParticle* new_particles = xr_alloc<SParticle>(max_count);
			if (new_particles == nullptr)
			{
				// ERROR - Not enough memory. Just give all we've got
				max_particles = particles_allocated;
				return max_particles;
			}

			CopyMemory(new_particles, particles, p_count * sizeof(SParticle));
			xr_free(particles);
			particles = new_particles;

			max_particles = max_count;
			particles_allocated = max_count;
			return max_count;
		}
		inline void Remove(int i)
		{
			if (0 == p_count)
			{
				return;
			}

			SParticle& m = particles[i];
			if (d_cb)
			{
				d_cb(owner, param, m, i);
			}

			m = particles[--p_count]; // не менять правило удаления !!! (dependence ParticleGroup)
		}

		inline BOOL Add(const CParticleVector& pos,
			const CParticleVector& posB,
			const CParticleVector& size,
			const CParticleVector& rot,
			const CParticleVector& vel,
			unsigned int color,
			const float age = 0.0f,
			unsigned short frame = 0,
			unsigned short flags = 0)
		{
			if (p_count >= max_particles)
			{
				return FALSE;
			}
			else
			{
				SParticle& P = particles[p_count];
				P.pos = pos;
				P.posB = posB;
				P.size = size;
				P.rot = rot;
				P.vel = vel;
				P.color = color;
				P.age = age;
				P.frame = frame;
				P.flags.assign(flags);
				if (b_cb)
				{
					b_cb(owner, param, P, p_count);
				}

				p_count++;
				return TRUE;
			}
		}
	};
};
