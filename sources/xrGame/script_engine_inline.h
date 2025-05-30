////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_inline.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CScriptEngine::add_script_process		(const EScriptProcessors &process_id, CScriptProcess *script_process)
{
	CScriptProcessStorage::const_iterator	I = m_script_processes.find(process_id);
	VERIFY									(I == m_script_processes.end());
	m_script_processes.insert				(std::make_pair(process_id,script_process));
}

CScriptProcess *CScriptEngine::script_process	(const EScriptProcessors &process_id) const
{
	CScriptProcessStorage::const_iterator	I = m_script_processes.find(process_id);
	if ((I != m_script_processes.end()))
		return								((*I).second);
	return									(0);
}

IC	void CScriptEngine::parse_script_namespace(pcstr function_to_call, pstr name_space, pstr function)
{
	pcstr					I = function_to_call, J = 0;
	for ( ; ; J=I,++I) {
		I					= strchr(I,'.');
		if (!I)
			break;
	}
	strcpy					(name_space,"_G");
	if (!J)
		strcpy				(function,function_to_call);
	else {
		CopyMemory		(name_space,function_to_call, u32(J - function_to_call)*sizeof(char));
		name_space[u32(J - function_to_call)] = 0;
		strcpy				(function,J + 1);
	}
}

template <typename _result_type>
IC	bool CScriptEngine::functor(pcstr function_to_call, luabind::functor<_result_type> &lua_function)
{
	luabind::object			object;
	if (!function_object(function_to_call,object))
		return				(false);

	try {
		lua_function		= luabind::object_cast<luabind::functor<_result_type> >(object);
	}
	catch(...) {
		return				(false);
	}

	return					(true);
}

#ifdef USE_DEBUGGER
IC CScriptDebugger *CScriptEngine::debugger			()
{
	return m_scriptDebugger;
}
#endif
