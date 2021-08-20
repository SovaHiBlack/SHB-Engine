#pragma once

#include "xr_collide_defs.h"

class ENGINE_API IRenderDetailModel;

// Эффект Дождя
class ENGINE_API CEffectRain
{
private:
	struct Item
	{
		Fvector3						P;
		Fvector3						Phit;
		Fvector3						D;
		float							fSpeed;
		unsigned int					dwTime_Life;
		unsigned int					dwTime_Hit;
		unsigned int					uv_set;
		void		invalidate			( )
		{
			dwTime_Life					= 0;
		}
	};

	struct Particle
	{
		Particle*						next;
		Particle*						prev;
		Fmatrix							mXForm;
		Fsphere							bounds;
		float							time;
	};

	enum States
	{
		stIdle							= 0,
		stWorking
	};

	// Visualization	(rain)
	ref_shader							SH_Rain;
	ref_geom							hGeom_Rain;

	// Visualization	(drops)
	IRenderDetailModel*					DM_Drop;
	ref_geom							hGeom_Drops;

	// Data and logic
	xr_vector<Item>						items;
	States								state;

	// Particles
	xr_vector<Particle>					particle_pool;
	Particle*							particle_active;
	Particle*							particle_idle;

	// Sounds
	ref_sound							snd_Ambient;

	// Utilities
	void			p_create			( );
	void			p_destroy			( );

	void			p_remove			(Particle* P, Particle*& LST);
	void			p_insert			(Particle* P, Particle*& LST);
	int				p_size				(Particle* LST);
	Particle*		p_allocate			( );
	void			p_free				(Particle* P);

	// Some methods
	void			Born				(Item& dest, float radius);
	void			Hit					(Fvector3& pos);
	BOOL			RayPick				(const Fvector3& s, const Fvector3& d, float& range, collide::rq_target tgt);
	void			RenewItem			(Item& dest, float height, BOOL bHit);

public:
					CEffectRain			( );
					~CEffectRain		( );

	void			Render				( );
	void			OnFrame				( );
};
