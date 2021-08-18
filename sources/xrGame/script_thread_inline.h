////////////////////////////////////////////////////////////////////////////
//	Module 		: script_thread_inline.h
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script thread class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	bool CScriptThread::active				() const
{
	return		(m_active);
}

inline	CSharedString CScriptThread::script_name	() const
{
	return		(m_script_name);
}

inline	int	 CScriptThread::thread_reference	() const
{
	return		(m_thread_reference);
}

inline	lua_State *CScriptThread::lua			() const
{
	return		(m_virtual_machine);
}
