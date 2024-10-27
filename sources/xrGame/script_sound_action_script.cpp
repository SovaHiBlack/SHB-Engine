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
				value("idle",					int(MonsterSound::eMonsterSoundIdle)),
				value("eat",					int(MonsterSound::eMonsterSoundEat)),
				value("attack",					int(MonsterSound::eMonsterSoundAggressive)),
				value("attack_hit",				int(MonsterSound::eMonsterSoundAttackHit)),
				value("take_damage",			int(MonsterSound::eMonsterSoundTakeDamage)),
				value("die",					int(MonsterSound::eMonsterSoundDie)),
				value("threaten",				int(MonsterSound::eMonsterSoundThreaten)),
				value("steal",					int(MonsterSound::eMonsterSoundSteal)),
				value("panic",					int(MonsterSound::eMonsterSoundPanic))
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
			.def(								constructor<MonsterSound::EType,int>())
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
