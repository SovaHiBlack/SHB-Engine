//	Module 		: ScriptAbstractAction.h
//	Description : Script abstract action

#pragma once

class CScriptAbstractAction
{
public:
	bool										m_bCompleted;

	inline			CScriptAbstractAction		( );
	virtual			~CScriptAbstractAction		( );
	virtual bool	completed					( );
};

#include "ScriptAbstractAction_inline.h"
