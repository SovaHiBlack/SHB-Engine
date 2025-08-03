#pragma once

struct lua_State;
struct Proto;
struct lua_Debug;
class CScriptFile;
class CScriptDebugger;

class CDbgLuaHelper  
{
public:
	void		RestoreGlobals				();
	void		CoverGlobals				();
	void		Describe					(pstr szRet, s32 nIndex, s32 szRet_size);
	bool		Eval						(pcstr szCode, pstr szRet, s32 szret_size);
	bool		GetCalltip					(pcstr szWord, pstr szCalltip, s32 sz_calltip);
	void		DrawGlobalVariables			();
	void		DrawLocalVariables			();
	pcstr		GetSource					();

	CDbgLuaHelper							(CScriptDebugger* d);
	virtual ~CDbgLuaHelper					();

	// debugger functions
	s32			PrepareLua					(lua_State* );
	void		UnPrepareLua				(lua_State* , s32);
	void		PrepareLuaBind				();


	void		DrawStackTrace				();
	static s32  OutputTop					(lua_State* );

	static void hookLua						(lua_State *, lua_Debug *);
	static s32  hookLuaBind					(lua_State *);

	static s32	errormessageLua				(lua_State* );
	static void errormessageLuaBind			(lua_State* );
	static void line_hook					(lua_State *, lua_Debug *);
	static void func_hook					(lua_State *, lua_Debug *);
	static void set_lua						(lua_State *);
	void		DrawVariable				(lua_State * l, pcstr name, bool bOpenTable);
	void		DrawTable					(lua_State *l, pcstr name, bool bRecursive=true);
	void		DrawVariableInfo			(pstr);
	CScriptDebugger*	debugger			(){return m_debugger;}

protected:
	CScriptDebugger*						m_debugger;
	static		CDbgLuaHelper*				m_pThis;
	static lua_State*						L;
	lua_Debug*								m_pAr;
};
