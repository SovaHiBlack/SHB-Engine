////////////////////////////////////////////////////////////////////////////
//	Module 		: script_object_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_abstract_action.h"
#include "script_export_space.h"
#include "ai_monster_space.h"

class CScriptGameObject;

class CScriptObjectAction : public CScriptAbstractAction {
public:
	CObject								*m_tpObject;
	MonsterSpace::EObjectAction			m_tGoalType;
	u32									m_dwQueueSize;
	CSharedString							m_caBoneName;

	inline				CScriptObjectAction	();
	inline				CScriptObjectAction	(CScriptGameObject *tpLuaGameObject, MonsterSpace::EObjectAction tObjectActionType, u32 dwQueueSize = u32(-1));
	inline				CScriptObjectAction	(const char* caBoneName, MonsterSpace::EObjectAction tObjectActionType);
	inline				CScriptObjectAction	(MonsterSpace::EObjectAction tObjectActionType);
	virtual			~CScriptObjectAction();
			void	SetObject			(CScriptGameObject *tpLuaGameObject);
	inline		void	SetObject			(const char* caBoneName);
	inline		void	SetObjectAction		(MonsterSpace::EObjectAction tObjectActionType);
	inline		void	SetQueueSize		(u32 dwQueueSize);
	inline		void	initialize			();

	static void script_register(lua_State*);
};

add_to_type_list(CScriptObjectAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptObjectAction)

#include "script_object_action_inline.h"