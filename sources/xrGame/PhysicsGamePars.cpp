#include "stdafx.h"

#include "PhysicsGamePars.h"

f32 object_damage_factor							= 1.0f;		// times increace damage from object collision
f32 collide_volume_max								= 200.0f;	// max collide sound level
f32 collide_volume_min								= 0.0f;		// min collide sound level

const f32 EffectPars::vel_cret_sound				= 10.0f;	// min vel_cret for collide sound
const f32 EffectPars::vel_cret_particles			= 15.0f;	// min vel_cret for particles
const f32 EffectPars::vel_cret_wallmark				= 30.0f;	// min vel_cret for wallmark

const f32 CharacterEffectPars::vel_cret_sound		= 20.0f;	// min vel_cret for collide sound
const f32 CharacterEffectPars::vel_cret_particles	= 60.0f;	// min vel_cret for particles
const f32 CharacterEffectPars::vel_cret_wallmark	= 100.0f;	// min vel_cret for wallmark

void LoadPhysicsGameParams( )
{
	collide_volume_min		= pSettings->r_float("sound", "snd_collide_min_volume");
	collide_volume_max		= pSettings->r_float("sound", "snd_collide_max_volume");
	object_damage_factor	= pSettings->r_float("physics", "object_damage_factor");
	object_damage_factor	*= object_damage_factor;
}
