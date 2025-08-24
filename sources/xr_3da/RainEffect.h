// RainEffect.h: interface for the CRainEffect class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "xr_collide_defs.h"

//refs
class ENGINE_API IRender_DetailModel;
//
class ENGINE_API CRainEffect
{
private:
	struct	Item
	{
		fVector3			P;
		fVector3			Phit;
		fVector3			D;
		f32			fSpeed;
		u32				dwTime_Life;
		u32				dwTime_Hit;
		u32				uv_set;
		void			invalidate( )
		{
			dwTime_Life = 0;
		}
	};
	struct	Particle
	{
		Particle* next;
		Particle* prev;
		fMatrix4x4			mXForm;
		fSphere			bounds;
		f32			time;
	};
	enum	States
	{
		stIdle = 0,
		stWorking
	};

private:
	// Visualization	(rain)
	ref_shader						SH_Rain;
	ref_geom						hGeom_Rain;

	// Visualization	(drops)
	IRender_DetailModel* DM_Drop;
	ref_geom						hGeom_Drops;

	// Data and logic
	xr_vector<Item>					items;
	States							state;

	// Particles
	xr_vector<Particle>				particle_pool;
	Particle* particle_active;
	Particle* particle_idle;

	// Sounds
	ref_sound						snd_Ambient;

	// Utilities
	void							p_create( );
	void							p_destroy( );

	void							p_remove(Particle* P, Particle*& LST);
	void							p_insert(Particle* P, Particle*& LST);
	s32								p_size(Particle* LST);
	Particle* p_allocate( );
	void							p_free(Particle* P);

	// Some methods
	void							Born(Item& dest, f32 radius);
	void							Hit(fVector3& pos);
	BOOL							RayPick(const fVector3& s, const fVector3& d, f32& range, collide::rq_target tgt);
	void							RenewItem(Item& dest, f32 height, BOOL bHit);

public:
	CRainEffect( );
	~CRainEffect( );

	void							Render( );
	void							OnFrame( );
};
