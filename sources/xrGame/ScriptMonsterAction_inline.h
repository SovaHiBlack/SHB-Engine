//	Module 		: ScriptMonsterAction_inline.h
//	Description : Script monster action class inline functions

#pragma once

inline CScriptMonsterAction::CScriptMonsterAction( )
{
	m_tAction = MonsterSpace::eGA_None;
	m_bCompleted = true;
}

inline CScriptMonsterAction::CScriptMonsterAction(MonsterSpace::EScriptMonsterGlobalAction action)
{
	m_tAction = action;
	m_bCompleted = false;
}

inline CScriptMonsterAction::CScriptMonsterAction(MonsterSpace::EScriptMonsterGlobalAction action, CScriptGameObject* tObj)
{
	m_tAction = action;
	m_bCompleted = false;
	SetObject(tObj);
}
