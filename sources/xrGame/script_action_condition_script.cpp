////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_condition_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action condition class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_action_condition.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptActionCondition::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptActionCondition>("cond")
			.enum_("cond")
			[
				value("move_end", s32(CScriptActionCondition::MOVEMENT_FLAG)),
				value("look_end", s32(CScriptActionCondition::WATCH_FLAG)),
				value("anim_end", s32(CScriptActionCondition::ANIMATION_FLAG)),
				value("sound_end", s32(CScriptActionCondition::SOUND_FLAG)),
				value("object_end", s32(CScriptActionCondition::OBJECT_FLAG)),
				value("time_end", s32(CScriptActionCondition::TIME_FLAG)),
				value("act_end", s32(CScriptActionCondition::ACT_FLAG))
			]
			.def(					constructor<>())
			.def(					constructor<u32>())
			.def(					constructor<u32,double>())
	];
}
