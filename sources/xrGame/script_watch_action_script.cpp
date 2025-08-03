////////////////////////////////////////////////////////////////////////////
//	Module 		: script_watch_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script watch action class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_watch_action.h"
#include "script_game_object.h"
#include "sight_manager_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptWatchAction::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptWatchAction>("look")
			.enum_("look")
			[
				value("path_dir", s32(SightManager::eSightTypePathDirection)),
				value("search", s32(SightManager::eSightTypeSearch)),
				value("danger", s32(SightManager::eSightTypeCover)),
				value("point", s32(SightManager::eSightTypePosition)),
				value("fire_point", s32(SightManager::eSightTypeFirePosition)),
				value("cur_dir", s32(SightManager::eSightTypeCurrentDirection)),
				value("direction", s32(SightManager::eSightTypeDirection))
			]
			.def(								constructor<>())
			.def(								constructor<SightManager::ESightType>())
			.def(								constructor<SightManager::ESightType, fVector3&>())
			.def(								constructor<SightManager::ESightType, CScriptGameObject*>())
			.def(								constructor<SightManager::ESightType, CScriptGameObject*, pcstr>())

			// searchlight 
			.def(								constructor<const fVector3&, f32, f32>())
			.def(								constructor<CScriptGameObject*, f32, f32>())

			.def("object",						&CScriptWatchAction::SetWatchObject)		// time
			.def("direct",						&CScriptWatchAction::SetWatchDirection)		// time
			.def("type",						&CScriptWatchAction::SetWatchType)
			.def("bone",						&CScriptWatchAction::SetWatchBone)
			.def("completed",					(bool (CScriptWatchAction::*)())(&CScriptWatchAction::completed))
	];
}