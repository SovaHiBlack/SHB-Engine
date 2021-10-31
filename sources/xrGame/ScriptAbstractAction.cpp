//	Module 		: ScriptAbstractAction.cpp
//	Description : Script abstract action

#include "stdafx.h"

#include "ScriptAbstractAction.h"

CScriptAbstractAction::~CScriptAbstractAction( )
{ }

bool CScriptAbstractAction::completed( )
{
	return m_bCompleted;
}
