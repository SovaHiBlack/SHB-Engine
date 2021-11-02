//	Module 		: ScriptEntityAction.cpp
//	Description : Script entity action class

#include "stdafx.h"

#include "ScriptEntityAction.h"

CScriptEntityAction::~CScriptEntityAction( )
{ }

using namespace luabind;

#pragma optimize("s", on)
void CScriptEntityAction::script_register(lua_State* L)
{
	module(L)
	[
		class_<CScriptEntityAction>("entity_action")
		.def(constructor<>( ))
		.def(constructor<const CScriptEntityAction*>( ))
		.def("set_action", (void (CScriptEntityAction::*)(CScriptMovementAction&))	(&CScriptEntityAction::SetAction))
		.def("set_action", (void (CScriptEntityAction::*)(CScriptWatchAction&))	(&CScriptEntityAction::SetAction))
		.def("set_action", (void (CScriptEntityAction::*)(CScriptAnimationAction&))	(&CScriptEntityAction::SetAction))
		.def("set_action", (void (CScriptEntityAction::*)(CScriptSoundAction&))	(&CScriptEntityAction::SetAction))
		.def("set_action", (void (CScriptEntityAction::*)(CScriptParticleAction&))	(&CScriptEntityAction::SetAction))
		.def("set_action", (void (CScriptEntityAction::*)(CScriptObjectAction&))	(&CScriptEntityAction::SetAction))
		.def("set_action", (void (CScriptEntityAction::*)(CScriptActionCondition&))	(&CScriptEntityAction::SetAction))
		.def("set_action", (void (CScriptEntityAction::*)(CScriptMonsterAction&))	(&CScriptEntityAction::SetAction))
		.def("move", &CScriptEntityAction::CheckIfMovementCompleted)
		.def("look", &CScriptEntityAction::CheckIfWatchCompleted)
		.def("anim", &CScriptEntityAction::CheckIfAnimationCompleted)
		.def("sound", &CScriptEntityAction::CheckIfSoundCompleted)
		.def("particle", &CScriptEntityAction::CheckIfParticleCompleted)
		.def("object", &CScriptEntityAction::CheckIfObjectCompleted)
		.def("time", &CScriptEntityAction::CheckIfTimeOver)
		.def("all", (bool (CScriptEntityAction::*)())(&CScriptEntityAction::CheckIfActionCompleted))
		.def("completed", (bool (CScriptEntityAction::*)())(&CScriptEntityAction::CheckIfActionCompleted))
	];
}
