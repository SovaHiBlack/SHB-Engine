////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_type_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound type script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_sound_type.h"
#include "ai_sounds.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptSoundType::script_register(lua_State *L)
{
	module(L)
	[
		class_<enum_exporter<ESoundTypes> >("snd_type")
			.enum_("sound_types")
			[
				value("no_sound",				s32(eST_NO_SOUND)),
				value("weapon",					s32(eST_WEAPON)),
				value("item",					s32(eST_ITEM)),
				value("monster",				s32(eST_MONSTER)),
				value("anomaly",				s32(eST_ANOMALY)),
				value("world",					s32(eST_WORLD)),
				value("pick_up",				s32(eST_PICKING_UP)),
				value("drop",					s32(eST_DROPPING)),
				value("hide",					s32(eST_HIDING)),
				value("take",					s32(eST_TAKING)),
				value("use",					s32(eST_USING)),
				value("shoot",					s32(eST_SHOOTING)),
				value("empty",					s32(eST_EMPTY_CLICKING)),
				value("bullet_hit",				s32(eST_BULLET_HIT)),
				value("reload",					s32(eST_RECHARGING)),
				value("die",					s32(eST_DYING)),
				value("injure",					s32(eST_INJURING)),
				value("step",					s32(eST_STEP)),
				value("talk",					s32(eST_TALKING)),
				value("attack",					s32(eST_ATTACKING)),
				value("eat",					s32(eST_EATING)),
				value("idle",					s32(eST_IDLE)),
				value("object_break",			s32(eST_OBJECT_BREAKING)),
				value("object_collide",			s32(eST_OBJECT_COLLIDING)),
				value("object_explode",			s32(eST_OBJECT_EXPLODING)),
				value("ambient",				s32(eST_AMBIENT)),
				value("item_pick_up",			s32(eST_ITEM_PICKING_UP)),
				value("item_drop",				s32(eST_ITEM_DROPPING)),
				value("item_hide",				s32(eST_ITEM_HIDING)),
				value("item_take",				s32(eST_ITEM_TAKING)),
				value("item_use",				s32(eST_ITEM_USING)),
				value("weapon_shoot",			int(SOUND_TYPE_WEAPON_SHOOTING			)),
				value("weapon_empty",			int(SOUND_TYPE_WEAPON_EMPTY_CLICKING	)),
				value("weapon_bullet_hit",		int(SOUND_TYPE_WEAPON_BULLET_HIT		)),
				value("weapon_reload",			int(SOUND_TYPE_WEAPON_RECHARGING		)),
				value("monster_die",			int(SOUND_TYPE_MONSTER_DYING			)),
				value("monster_injure",			int(SOUND_TYPE_MONSTER_INJURING			)),
				value("monster_step",			int(SOUND_TYPE_MONSTER_STEP				)),
				value("monster_talk",			int(SOUND_TYPE_MONSTER_TALKING			)),
				value("monster_attack",			int(SOUND_TYPE_MONSTER_ATTACKING		)),
				value("monster_eat",			int(SOUND_TYPE_MONSTER_EATING			)),
				value("anomaly_idle",			int(SOUND_TYPE_ANOMALY_IDLE				)),
				value("world_object_break",		int(SOUND_TYPE_WORLD_OBJECT_BREAKING	)),
				value("world_object_collide",	int(SOUND_TYPE_WORLD_OBJECT_COLLIDING	)),
				value("world_object_explode",	int(SOUND_TYPE_WORLD_OBJECT_EXPLODING	)),
				value("world_ambient",			int(SOUND_TYPE_WORLD_AMBIENT			))
			]
	];
}