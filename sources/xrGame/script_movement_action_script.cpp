////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_movement_action.h"
#include "script_game_object.h"
#include "patrol_path_manager_space.h"
#include "detail_path_manager_space.h"
#include "ai_monster_space.h"
#include "patrol_path_params.h"
#include "patrol_path.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptMovementAction::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptMovementAction>("move")
			.enum_("body")
			[
				value("crouch", s32(MonsterSpace::eBodyStateCrouch)),
				value("standing", s32(MonsterSpace::eBodyStateStand))
			]
			.enum_("move")
			[
				value("walk", s32(MonsterSpace::eMovementTypeWalk)),
				value("run", s32(MonsterSpace::eMovementTypeRun)),
				value("stand", s32(MonsterSpace::eMovementTypeStand))
			]
			.enum_("path")
			[
				value("line", s32(DetailPathManager::eDetailPathTypeSmooth)),
				value("dodge", s32(DetailPathManager::eDetailPathTypeSmoothDodge)),
				value("criteria", s32(DetailPathManager::eDetailPathTypeSmoothCriteria)),
				value("curve", s32(DetailPathManager::eDetailPathTypeSmooth)),
				value("curve_criteria", s32(DetailPathManager::eDetailPathTypeSmoothCriteria))
			]
			.enum_("input")
			[
				value("none", s32(CScriptMovementAction::eInputKeyNone)),
				value("fwd", s32(CScriptMovementAction::eInputKeyForward)),
				value("back", s32(CScriptMovementAction::eInputKeyBack)),
				value("left", s32(CScriptMovementAction::eInputKeyLeft)),
				value("right", s32(CScriptMovementAction::eInputKeyRight)),
				value("up", s32(CScriptMovementAction::eInputKeyShiftUp)),
				value("down", s32(CScriptMovementAction::eInputKeyShiftDown)),
				value("handbrake", s32(CScriptMovementAction::eInputKeyBreaks)),
				value("on", s32(CScriptMovementAction::eInputKeyEngineOn)),
				value("off", s32(CScriptMovementAction::eInputKeyEngineOff))
			]
			.enum_("monster")
			[

				value("walk_fwd", s32(MonsterSpace::eMA_WalkFwd)),
				value("walk_bkwd", s32(MonsterSpace::eMA_WalkBkwd)),
				value("run_fwd", s32(MonsterSpace::eMA_Run)),
				value("drag", s32(MonsterSpace::eMA_Drag)),
				value("jump", s32(MonsterSpace::eMA_Jump)),
				value("steal", s32(MonsterSpace::eMA_Steal))
			]
			.enum_("monster_speed_param")
			[
				value("default", s32(MonsterSpace::eSP_Default)),
				value("force", s32(MonsterSpace::eSP_ForceSpeed))
			]

			.def(								constructor<>())
			.def(								constructor<const CScriptMovementAction::EInputKeys>())
			.def(								constructor<const CScriptMovementAction::EInputKeys, f32>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,DetailPathManager::EDetailPathType,CScriptGameObject*>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,DetailPathManager::EDetailPathType,CScriptGameObject*, f32>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,DetailPathManager::EDetailPathType,const CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,DetailPathManager::EDetailPathType,const CPatrolPathParams &, f32>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,DetailPathManager::EDetailPathType,const fVector3&>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,DetailPathManager::EDetailPathType,const fVector3&, f32>())
			.def(								constructor<const fVector3&, f32>())
			
			// Monsters 
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction, fVector3&>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CScriptGameObject*>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction, fVector3&, f32>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,u32, fVector3&>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,u32, fVector3&, f32>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CPatrolPathParams &, f32>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CScriptGameObject*, f32>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction, fVector3&, f32,	MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CPatrolPathParams &, f32,	MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CScriptGameObject*, f32,	MonsterSpace::EScriptMonsterSpeedParam>())

			.def("body",						&CScriptMovementAction::SetBodyState)
			.def("move",						&CScriptMovementAction::SetMovementType)
			.def("path",						&CScriptMovementAction::SetPathType)
			.def("object",						&CScriptMovementAction::SetObjectToGo)
			.def("patrol",						&CScriptMovementAction::SetPatrolPath)
			.def("position",					&CScriptMovementAction::SetPosition)
			.def("input",						&CScriptMovementAction::SetInputKeys)
			.def("completed",					(bool (CScriptMovementAction::*)())(&CScriptMovementAction::completed))
	];
}