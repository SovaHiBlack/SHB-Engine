////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound action class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_sound_action.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptSoundAction::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptSoundAction>("sound")
			.enum_("type")
			[
				value("idle", s32(MonsterSound::eMonsterSoundIdle)),
				value("eat", s32(MonsterSound::eMonsterSoundEat)),
				value("attack", s32(MonsterSound::eMonsterSoundAggressive)),
				value("attack_hit", s32(MonsterSound::eMonsterSoundAttackHit)),
				value("take_damage", s32(MonsterSound::eMonsterSoundTakeDamage)),
				value("die", s32(MonsterSound::eMonsterSoundDie)),
				value("threaten", s32(MonsterSound::eMonsterSoundThreaten)),
				value("steal", s32(MonsterSound::eMonsterSoundSteal)),
				value("panic", s32(MonsterSound::eMonsterSoundPanic))
			]

			.def(								constructor<>())
			.def(								constructor<pcstr, pcstr>())
			.def(								constructor<pcstr, pcstr,const fVector3&>())
			.def(								constructor<pcstr, pcstr,const fVector3&,const fVector3&>())
			.def(								constructor<pcstr, pcstr,const fVector3&,const fVector3&,bool>())
			.def(								constructor<pcstr,const fVector3&>())
			.def(								constructor<pcstr,const fVector3&,const fVector3&>())
			.def(								constructor<pcstr,const fVector3&,const fVector3&,bool>())
			.def(								constructor<CScriptSound&, pcstr,const fVector3&>())
			.def(								constructor<CScriptSound&, pcstr,const fVector3&,const fVector3&>())
			.def(								constructor<CScriptSound&, pcstr,const fVector3&,const fVector3&,bool>())
			.def(								constructor<CScriptSound&,const fVector3&>())
			.def(								constructor<CScriptSound&,const fVector3&,const fVector3&>())
			.def(								constructor<CScriptSound&,const fVector3&,const fVector3&,bool>())
			// monster specific
			.def(								constructor<MonsterSound::EType>())
			.def(								constructor<MonsterSound::EType, s32>())
			// trader specific
			.def(								constructor<pcstr, pcstr,MonsterSpace::EMonsterHeadAnimType>())

			.def("set_sound",					(void (CScriptSoundAction::*)(pcstr))(&CScriptSoundAction::SetSound))
			.def("set_sound",					(void (CScriptSoundAction::*)(const CScriptSound &))(&CScriptSoundAction::SetSound))
			.def("set_sound_type",				&CScriptSoundAction::SetSoundType)
			.def("set_bone",					&CScriptSoundAction::SetBone)
			.def("set_position",				&CScriptSoundAction::SetPosition)
			.def("set_angles",					&CScriptSoundAction::SetAngles)
			.def("completed",					(bool (CScriptSoundAction::*)())(&CScriptSoundAction::completed))
	];
}
