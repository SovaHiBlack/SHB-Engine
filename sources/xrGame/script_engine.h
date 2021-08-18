////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ScriptStorage.h"
#include "script_export_space.h"
//#include "script_space_forward.h"
#include "associative_vector.h"

extern "C" {
	#include "..\Lua\src\lua.h"
	#include "..\Lua\src\luajit.h"
	#include "..\Lua\src\lcoco.h"
};

#include "script_engine_space.h"

class CScriptProcess;
class CScriptThread;
struct lua_State;
struct lua_Debug;

#ifdef USE_DEBUGGER
	class CScriptDebugger;
#endif

class CScriptEngine : public CScriptStorage {
public:
	typedef CScriptStorage											inherited;
	typedef ScriptEngine::EScriptProcessors							EScriptProcessors;
	typedef associative_vector<EScriptProcessors,CScriptProcess*>	CScriptProcessStorage;

private:
	bool						m_reload_modules;

protected:
	CScriptProcessStorage		m_script_processes;
	int							m_stack_level;
	CSharedString					m_class_registrators;

#ifdef USE_DEBUGGER
	CScriptDebugger				*m_scriptDebugger;
#endif

private:
	string128					m_last_no_file;
	u32							m_last_no_file_length;

			bool				no_file_exists				(const char* file_name, u32 string_length);
			void				add_no_file					(const char* file_name, u32 string_length);

public:
								CScriptEngine				();
	virtual						~CScriptEngine				();
			void				init						();
	virtual	void				unload						();
	static	int					lua_panic					(lua_State *L);
	static	void				lua_error					(lua_State *L);
	static	int					lua_pcall_failed			(lua_State *L);
	static	void				lua_hook_call				(lua_State *L, lua_Debug *dbg);
			void				setup_callbacks				();
			void				load_common_scripts			();
//			bool				load_file					(const char*	caScriptName, const char* namespace_name);
	inline		CScriptProcess		*script_process				(const EScriptProcessors &process_id) const;
	inline		void				add_script_process			(const EScriptProcessors &process_id, CScriptProcess *script_process);
			void				remove_script_process		(const EScriptProcessors &process_id);
			void				setup_auto_load				();
			void				process_file_if_exists		(const char* file_name, bool warn_if_not_exist);
			void				process_file				(const char* file_name);
			void				process_file				(const char* file_name, bool reload_modules);
			bool				function_object				(const char* function_to_call, luabind::object &object, int type = LUA_TFUNCTION);
			void				register_script_classes		();
	inline		void				parse_script_namespace		(const char* function_to_call, char* name_space, char* functor);

	template <typename _result_type>
	inline		bool				functor						(const char* function_to_call, luabind::functor<_result_type> &lua_function);

#ifdef USE_DEBUGGER
			void				stopDebugger				();
			void				restartDebugger				();
			CScriptDebugger		*debugger					();
#endif
			void				collect_all_garbage			();

public:
	static void script_register(lua_State*);
};

add_to_type_list(CScriptEngine)
#undef script_type_list
#define script_type_list save_type_list(CScriptEngine)

#include "script_engine_inline.h"
