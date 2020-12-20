#include "stdafx.h"

#include "PhysicsGamePars.h"

float object_damage_factor								= 1.0f;		//times increace damage from object collision
float collide_volume_max								= 200.0f;	//max collide sound level
float collide_volume_min								= 0.0f;		//min collide sound level

const float EffectPars::vel_cret_sound					= 10.0f;
const float EffectPars::vel_cret_particles				= 15.0f;
const float EffectPars::vel_cret_wallmark				= 30.0f;

const float CharacterEffectPars::vel_cret_sound			= 20.0f;
const float CharacterEffectPars::vel_cret_particles		= 60.0f;
const float CharacterEffectPars::vel_cret_wallmark		= 100.0f;

void LoadPhysicsGameParams( )
{
	collide_volume_min		= pSettings->r_float("sound", "snd_collide_min_volume");
	collide_volume_max		= pSettings->r_float("sound", "snd_collide_max_volume");
	object_damage_factor	= pSettings->r_float("physics", "object_damage_factor");
	object_damage_factor	*= object_damage_factor;
}
