////////////////////////////////////////////////////////////////////////////
//	Module 		: script_animation_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script animation action class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_animation_action.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptAnimationAction::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptAnimationAction>("anim")
			.enum_("type")
			[
				value("free", s32(MonsterSpace::eMentalStateFree)),
				value("danger", s32(MonsterSpace::eMentalStateDanger)),
				value("panic", s32(MonsterSpace::eMentalStatePanic))
			]
			.enum_("monster")
			[
				value("stand_idle", s32(MonsterSpace::eAA_StandIdle)),
				value("sit_idle", s32(MonsterSpace::eAA_SitIdle)),
				value("lie_idle", s32(MonsterSpace::eAA_LieIdle)),
				value("eat", s32(MonsterSpace::eAA_Eat)),
				value("sleep", s32(MonsterSpace::eAA_Sleep)),
				value("rest", s32(MonsterSpace::eAA_Rest)),
				value("attack", s32(MonsterSpace::eAA_Attack)),
				value("look_around", s32(MonsterSpace::eAA_LookAround)),
				value("turn", s32(MonsterSpace::eAA_Turn))
			]

			.def(								constructor<>())
			.def(								constructor<pcstr>())
			.def(								constructor<pcstr,bool>())
			.def(								constructor<MonsterSpace::EMentalState>())
			
			// Monster specific
			.def(								constructor<MonsterSpace::EScriptMonsterAnimAction, s32>())
			
			.def("anim",						&CScriptAnimationAction::SetAnimation)
			.def("type",						&CScriptAnimationAction::SetMentalState)
			.def("completed",					(bool (CScriptAnimationAction::*)())(&CScriptAnimationAction::completed))
	];
}