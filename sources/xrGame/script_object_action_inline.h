////////////////////////////////////////////////////////////////////////////
//	Module 		: script_object_action_inline.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object action class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CScriptObjectAction::CScriptObjectAction	()
{
	m_tpObject			= 0;
	m_tGoalType			= MonsterSpace::eObjectActionIdle;
	m_bCompleted		= true;
}

inline	CScriptObjectAction::CScriptObjectAction	(CScriptGameObject *tpLuaGameObject, MonsterSpace::EObjectAction tObjectActionType, u32 dwQueueSize)
{
	SetObject			(tpLuaGameObject);
	SetObjectAction		(tObjectActionType);
	SetQueueSize		(dwQueueSize);
}

inline	CScriptObjectAction::CScriptObjectAction	(const char* caBoneName, MonsterSpace::EObjectAction tObjectActionType)
{
	SetObject			(caBoneName);
	SetObjectAction		(tObjectActionType);
}

inline	CScriptObjectAction::CScriptObjectAction	(MonsterSpace::EObjectAction tObjectActionType)
{
	SetObjectAction		(tObjectActionType);
}

inline	void CScriptObjectAction::SetObject			(const char* caBoneName)
{
	m_caBoneName		= caBoneName;
	m_bCompleted		= false;
}

inline	void CScriptObjectAction::SetObjectAction	(MonsterSpace::EObjectAction tObjectActionType)
{
	m_tGoalType			= tObjectActionType;
	m_bCompleted		= false;
}

inline	void CScriptObjectAction::SetQueueSize		(u32 dwQueueSize)
{
	m_dwQueueSize		= dwQueueSize;
	m_bCompleted		= false;
}

inline	void CScriptObjectAction::initialize		()
{ }
