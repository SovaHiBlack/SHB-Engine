//	Module 		: ScriptEntity_inline.h
//	Description : Script entity class inline functions

#pragma once

inline	CGameObject& CScriptEntity::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}
