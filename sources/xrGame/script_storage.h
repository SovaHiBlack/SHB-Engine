////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_storage_space.h"
#include "script_space_forward.h"

struct lua_State;
class CScriptThread;

#ifdef DEBUG
#	ifndef ENGINE_BUILD
#		define USE_DEBUGGER
#	endif
#endif // def DEBUG

using namespace ScriptStorage;

class CScriptStorage {
private:
	lua_State					*m_virtual_machine	;
	CScriptThread				*m_current_thread	;
	BOOL						m_jit				;

#ifdef DEBUG
public:
	bool						m_stack_is_ready	;
#endif

#ifdef DEBUG
protected:
	CMemoryWriter				m_output;
#endif // DEBUG

protected:
	static	s32					vscript_log					(ScriptStorage::ELuaMessageType tLuaMessageType, pcstr caFormat, va_list marker);
			bool				parse_namespace				(pcstr caNamespaceName, pstr b, pstr c);
			bool				do_file						(pcstr	caScriptName, pcstr caNameSpaceName);
			void				reinit						();

public:
#ifdef DEBUG
			void				print_stack					();
#endif

public:
								CScriptStorage				();
	virtual						~CScriptStorage				();
	IC		lua_State			*lua						();
	IC		void				current_thread				(CScriptThread *thread);
	IC		CScriptThread		*current_thread				() const;
			bool				load_buffer					(lua_State *L, pcstr caBuffer, size_t tSize, pcstr caScriptName, pcstr caNameSpaceName = 0);
			bool				load_file_into_namespace	(pcstr	caScriptName, pcstr caNamespaceName);
			bool				namespace_loaded			(pcstr	caName, bool remove_from_stack = true);
			bool				object						(pcstr	caIdentifier, s32 type);
			bool				object						(pcstr	caNamespaceName, pcstr	caIdentifier, s32 type);
			luabind::object		name_space					(pcstr	namespace_name);
	static	s32		__cdecl		script_log					(ELuaMessageType message, pcstr	caFormat, ...);
	static	bool				print_output				(lua_State *L, pcstr	caScriptName, s32		iErorCode = 0);
	static	void				print_error					(lua_State *L, s32		iErrorCode);

#ifdef DEBUG
public:
			void				flush_log					();
#endif // DEBUG
};

#include "script_storage_inline.h"