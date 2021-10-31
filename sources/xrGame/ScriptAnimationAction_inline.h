//	Module 		: ScriptAnimationAction_inline.h
//	Description : Script animation action class inline functions

#pragma once

inline CScriptAnimationAction::CScriptAnimationAction( )
{
	m_tMentalState = MonsterSpace::eMentalStateDanger;
	m_tGoalType = eGoalTypeMental;
	m_tAnimAction = MonsterSpace::eAA_NoAction;
	m_bCompleted = true;
	m_use_animation_movement_controller = false;
}

inline CScriptAnimationAction::CScriptAnimationAction(const char* caAnimationToPlay, bool use_animation_movement_controller)
{
	SetAnimation(caAnimationToPlay);
	m_use_animation_movement_controller = use_animation_movement_controller;
}

inline CScriptAnimationAction::CScriptAnimationAction(MonsterSpace::EMentalState tMentalState)
{
	SetMentalState(tMentalState);
}

// -------------------------------------------------------------------------------------------------
// Monster
// -------------------------------------------------------------------------------------------------
inline CScriptAnimationAction::CScriptAnimationAction(MonsterSpace::EScriptMonsterAnimAction tAnimAction, int index)
{
	m_tAnimAction = tAnimAction;
	m_bCompleted = false;
	anim_index = index;
	m_use_animation_movement_controller = false;
}

inline void CScriptAnimationAction::SetAnimation(const char* caAnimationToPlay)
{
	m_caAnimationToPlay = caAnimationToPlay;
	m_tMentalState = MonsterSpace::eMentalStateDanger;
	m_tGoalType = eGoalTypeAnimation;
	m_bCompleted = false;
	m_use_animation_movement_controller = false;
}

inline void CScriptAnimationAction::SetMentalState(MonsterSpace::EMentalState tMentalState)
{
	m_tMentalState = tMentalState;
	m_tGoalType = eGoalTypeMental;
	m_bCompleted = true;
	m_use_animation_movement_controller = false;
}

inline void CScriptAnimationAction::initialize( )
{ }
