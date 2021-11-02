//	Module 		: ScriptMonsterAction.h
//	Description : Script monster action class

#pragma once

#include "ScriptAbstractAction.h"
#include "script_export_space.h"
#include "ai_monster_space.h"

class CScriptGameObject;

class CScriptMonsterAction : public CScriptAbstractAction
{
public:
	MonsterSpace::EScriptMonsterGlobalAction	m_tAction;
	CObject* m_tObject;

	inline				CScriptMonsterAction( );
	inline				CScriptMonsterAction(MonsterSpace::EScriptMonsterGlobalAction action);
	inline				CScriptMonsterAction(MonsterSpace::EScriptMonsterGlobalAction action, CScriptGameObject* tObj);
	virtual			~CScriptMonsterAction( );
	void	SetObject(CScriptGameObject* tObj);

	static void script_register(lua_State* L);
};

add_to_type_list(CScriptMonsterAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptMonsterAction)

#include "ScriptMonsterAction_inline.h"
