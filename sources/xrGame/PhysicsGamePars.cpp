#include "stdafx.h"
#include "PhysicsGamePars.h"
f32 object_damage_factor		=		1.f		; //times increace damage from object collision
f32 collide_volume_max		=		200.f	; //max collide sound level
f32 collide_volume_min		=		0.f		; //min collide sound level

//f32 vel_cret_sound			=		10.f	; //min vel_cret for collide sound
//f32 vel_cret_wallmark			=		30.f	; //min vel_cret for wallmark
//f32 vel_cret_particles		=		15.f	; //...................		

const f32 EffectPars::vel_cret_sound=10.f;
const f32 EffectPars::vel_cret_particles=15.f;
const f32 EffectPars::vel_cret_wallmark=30.f;



const f32 CharacterEffectPars::vel_cret_sound=20.f;
const f32 CharacterEffectPars::vel_cret_particles=60.f;
const f32 CharacterEffectPars::vel_cret_wallmark=100.f;

void LoadPhysicsGameParams()
{
	collide_volume_min=pSettings->r_float("sound","snd_collide_min_volume");
	collide_volume_max=pSettings->r_float("sound","snd_collide_max_volume");
	object_damage_factor=pSettings->r_float("physics","object_damage_factor");
	object_damage_factor*=object_damage_factor;
}