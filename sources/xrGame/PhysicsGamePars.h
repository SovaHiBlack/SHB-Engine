#pragma once

extern f32 object_damage_factor;
extern f32 collide_volume_max;
extern f32 collide_volume_min;

struct EffectPars
{
	const static f32 vel_cret_sound;
	const static f32 vel_cret_particles;
	const static f32 vel_cret_wallmark;
};

struct CharacterEffectPars
{
	const static f32 vel_cret_sound;
	const static f32 vel_cret_particles;
	const static f32 vel_cret_wallmark;
};

void	LoadPhysicsGameParams	();
