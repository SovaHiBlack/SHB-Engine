// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "xr_collide_defs.h"

//refs
class ENGINE_API IRender_DetailModel;
//
class ENGINE_API CEffect_Rain
{
private:
	struct	Item
	{
		Fvector			P;
		Fvector			Phit;
		Fvector			D;
		f32			fSpeed;
		u32				dwTime_Life;
		u32				dwTime_Hit;
		u32				uv_set;
		void			invalidate	()
		{
			dwTime_Life	= 0;
		}
	};
	struct	Particle
	{
		Particle		*next,*prev;
		Fmatrix			mXForm;
		Fsphere			bounds;
		f32			time;
	};
	enum	States
	{
		stIdle		= 0,
		stWorking
	};
private:
	// Visualization	(rain)
	ref_shader						SH_Rain;
	ref_geom						hGeom_Rain;

	// Visualization	(drops)
	IRender_DetailModel*			DM_Drop;
	ref_geom						hGeom_Drops;
	
	// Data and logic
	xr_vector<Item>					items;
	States							state;

	// Particles
	xr_vector<Particle>				particle_pool;
	Particle*						particle_active;
	Particle*						particle_idle;

	// Sounds
	ref_sound						snd_Ambient;

	// Utilities
	void							p_create		();
	void							p_destroy		();

	void							p_remove		(Particle* P, Particle* &LST);
	void							p_insert		(Particle* P, Particle* &LST);
	int								p_size			(Particle* LST);
	Particle*						p_allocate		();
	void							p_free			(Particle* P);

	// Some methods
	void							Born			(Item& dest, f32 radius);
	void							Hit				(Fvector& pos);
	BOOL							RayPick			(const Fvector& s, const Fvector& d, f32& range, collide::rq_target tgt);
	void							RenewItem		(Item& dest, f32 height, BOOL bHit);
public:
									CEffect_Rain	();
									~CEffect_Rain	();

	void							Render			();
	void							OnFrame			();
};
