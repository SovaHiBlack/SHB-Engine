////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_movement_action.h"
#include "script_game_object.h"
#include "detail_path_manager_space.h"
#include "patrol_path_manager_space.h"
#include "patrol_path_params.h"
#include "ai_monster_space.h"

CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams &tPatrolPathParams, f32 dist_to_end, MonsterSpace::EScriptMonsterSpeedParam speed_param)
{																																			
	m_tMoveAction			= tAct;
	SetPatrolPath			(tPatrolPathParams.m_path,tPatrolPathParams.m_path_name);															
	SetPatrolStart			(tPatrolPathParams.m_tPatrolPathStart);																				
	SetPatrolStop			(tPatrolPathParams.m_tPatrolPathStop);																				
	SetPatrolRandom			(tPatrolPathParams.m_bRandom);																						
	m_tSpeedParam			= speed_param;
	m_previous_patrol_point = tPatrolPathParams.m_previous_index;
	m_fDistToEnd			= dist_to_end;
}						

CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, const CPatrolPathParams &tPatrolPathParams, f32 fSpeed)
{
	SetBodyState		(tBodyState);
	SetMovementType		(tMovementType);
	SetPathType			(tPathType);
	SetPatrolPath		(tPatrolPathParams.m_path,tPatrolPathParams.m_path_name);
	SetPatrolStart		(tPatrolPathParams.m_tPatrolPathStart);
	SetPatrolStop		(tPatrolPathParams.m_tPatrolPathStop);
	SetPatrolRandom		(tPatrolPathParams.m_bRandom);
	SetSpeed			(fSpeed);
}

CScriptMovementAction::CScriptMovementAction		()
{
	SetInputKeys		(eInputKeyNone);
	SetBodyState		(MonsterSpace::eBodyStateStand);
	SetMovementType		(MonsterSpace::eMovementTypeStand);
	SetPathType			(DetailPathManager::eDetailPathTypeSmooth);
	SetPatrolPath		(0,"");
	SetPatrolStart		(PatrolPathManager::ePatrolStartTypeNearest);
	SetPatrolStop		(PatrolPathManager::ePatrolRouteTypeContinue);
	SetPatrolRandom		(true);
	SetSpeed			(0);
	SetObjectToGo		(0);
	SetPosition			(fVector3().set(0.0f,0.0f,0.0f));
	m_tGoalType			= eGoalTypeDummy;
	m_bCompleted		= true;
}

CScriptMovementAction::CScriptMovementAction		(const fVector3& tPosition, f32 fSpeed)
{
	SetBodyState		(MonsterSpace::eBodyStateStand);
	SetMovementType		(MonsterSpace::eMovementTypeStand);
	SetPathType			(DetailPathManager::eDetailPathTypeSmooth);
	SetPosition			(tPosition);
	SetSpeed			(fSpeed);
	m_tGoalType			= eGoalTypeNoPathPosition;
}

CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, fVector3& tPosition, f32	dist_to_end)
{																																			
	MonsterSpace::EScriptMonsterSpeedParam speed_param = MonsterSpace::eSP_Default;
	m_tMoveAction		= tAct;
	SetPosition			(tPosition);																										
	m_tSpeedParam		= speed_param;
	m_fDistToEnd		= dist_to_end;
}						

CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, u32 node_id, fVector3& tPosition, f32 dist_to_end)
{																																			
	m_tMoveAction			= tAct;
	m_tDestinationPosition	= tPosition;
	m_tGoalType				= eGoalTypePathNodePosition;
	m_tSpeedParam			= MonsterSpace::eSP_Default;
	m_fDistToEnd			= dist_to_end;
	m_tNodeID				= node_id;
	m_bCompleted			= false;
}						

CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams &tPatrolPathParams, f32 dist_to_end)
{																																			
	MonsterSpace::EScriptMonsterSpeedParam speed_param = MonsterSpace::eSP_Default;
	m_tMoveAction			= tAct;
	SetPatrolPath			(tPatrolPathParams.m_path,tPatrolPathParams.m_path_name);															
	SetPatrolStart			(tPatrolPathParams.m_tPatrolPathStart);																				
	SetPatrolStop			(tPatrolPathParams.m_tPatrolPathStop);																				
	SetPatrolRandom			(tPatrolPathParams.m_bRandom);																						
	m_tSpeedParam			= speed_param;
	m_previous_patrol_point = tPatrolPathParams.m_previous_index;
	m_fDistToEnd			= dist_to_end;
}						

CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, CScriptGameObject *tpObjectToGo, f32 dist_to_end)
{
	MonsterSpace::EScriptMonsterSpeedParam speed_param = MonsterSpace::eSP_Default;
	m_tMoveAction		= tAct;
	SetObjectToGo		(tpObjectToGo);
	m_tSpeedParam		= speed_param;
	m_fDistToEnd		= dist_to_end;
}

CScriptMovementAction::~CScriptMovementAction	()
{ }

void CScriptMovementAction::SetObjectToGo		(CScriptGameObject *tpObjectToGo)
{
	if (tpObjectToGo)
		m_tpObjectToGo	= tpObjectToGo->operator CObject*();
	else
		m_tpObjectToGo	= 0;
	m_tGoalType			= eGoalTypeObject;
	m_bCompleted		= false;
}
