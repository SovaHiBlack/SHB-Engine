////////////////////////////////////////////////////////////////////////////
//	Module 		: script_object_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object action class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_object_action.h"
#include "script_game_object.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptObjectAction::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptObjectAction>("object")
			.enum_("state")
			[
				value("idle", s32(MonsterSpace::eObjectActionIdle)),
				value("show", s32(MonsterSpace::eObjectActionShow)),
				value("hide", s32(MonsterSpace::eObjectActionHide)),
				value("take", s32(MonsterSpace::eObjectActionTake)),
				value("drop", s32(MonsterSpace::eObjectActionDrop)),
				value("strap", s32(MonsterSpace::eObjectActionStrapped)),
				value("aim1", s32(MonsterSpace::eObjectActionAim1)),
				value("aim2", s32(MonsterSpace::eObjectActionAim2)),
				value("reload", s32(MonsterSpace::eObjectActionReload1)),
				value("reload1", s32(MonsterSpace::eObjectActionReload1)),
				value("reload2", s32(MonsterSpace::eObjectActionReload2)),
				value("fire1", s32(MonsterSpace::eObjectActionFire1)),
				value("fire2", s32(MonsterSpace::eObjectActionFire2)),
				value("switch1", s32(MonsterSpace::eObjectActionSwitch1)),
				value("switch2", s32(MonsterSpace::eObjectActionSwitch2)),
				value("activate", s32(MonsterSpace::eObjectActionActivate)),
				value("deactivate", s32(MonsterSpace::eObjectActionDeactivate)),
				value("use", s32(MonsterSpace::eObjectActionUse)),
				value("turn_on", s32(MonsterSpace::eObjectActionTurnOn)),
				value("turn_off", s32(MonsterSpace::eObjectActionTurnOff)),
				value("dummy", s32(MonsterSpace::eObjectActionDummy))
			]
			.def(								constructor<>())
			.def(								constructor<CScriptGameObject*,MonsterSpace::EObjectAction>())
			.def(								constructor<CScriptGameObject*,MonsterSpace::EObjectAction,u32>())
			.def(								constructor<MonsterSpace::EObjectAction>())
			.def(								constructor<pcstr,MonsterSpace::EObjectAction>())
			.def("action",						&CScriptObjectAction::SetObjectAction)
			.def("object",						(void (CScriptObjectAction::*)(pcstr))(&CScriptObjectAction::SetObject))
			.def("object",						(void (CScriptObjectAction::*)(CScriptGameObject*))(&CScriptObjectAction::SetObject))
			.def("completed",					(bool (CScriptObjectAction::*)())(&CScriptObjectAction::completed))
	];
}
