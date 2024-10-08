#pragma once

#include "script_lua_helper.h"
#include "script_debugger_threads.h"
#include "script_CallStack.h"
#include "script_debugger_messages.h"
//#include "script_debugger_utils.h"

class CMailSlotMsg;
struct lua_State;

#define DMOD_NONE					0
#define DMOD_STEP_INTO				1
#define DMOD_STEP_OVER				2
#define DMOD_STEP_OUT				3
#define DMOD_RUN_TO_CURSOR			4
//#define DMOD_SHOW_STACK_LEVEL		5

#define DMOD_BREAK					10
#define DMOD_STOP					11

struct SBreakPoint{
	shared_str	fileName;
	s32			nLine;
	SBreakPoint()	{nLine=0;};
	SBreakPoint(const SBreakPoint& other)
	{
		operator = (other);
	};
	SBreakPoint& operator = (const SBreakPoint& other){
		fileName=	other.fileName;
		nLine	=	other.nLine;
		return *this;
	}
};

class CScriptDebugger
{
public:
	void			Connect				(pcstr mslot_name);
	void			Eval				(pcstr strCode, pstr res, int res_sz);
	void			AddLocalVariable	(const Variable& var);
	void			ClearLocalVariables	();
	void			AddGlobalVariable	(pcstr name, pcstr type, pcstr value);
	void			ClearGlobalVariables();
	void			StackLevelChanged	();
	void			initiateDebugBreak	();
	void			DebugBreak			(pcstr szFile, int nLine);
	void			ErrorBreak			(pcstr szFile = 0, int nLine = 0);
	void			LineHook			(pcstr szFile, int nLine);
	void			FunctionHook		(pcstr szFile, int nLine, BOOL bCall);
	void			Write				(pcstr szMsg);

	int				PrepareLua			(lua_State* );
	void			UnPrepareLua		(lua_State* l, int idx);
	BOOL			PrepareLuaBind		();

					CScriptDebugger		();
	virtual			~CScriptDebugger	();

//	void			Go					();
//	void			StepInto			();
//	void			StepOver			();
//	void			StepOut				();
//	void			RunToCursor			();

	void			ClearThreads		();
	void			AddThread			(SScriptThread&);

	void			ClearStackTrace		();
	void			AddStackTrace		(pcstr strDesc, pcstr strFile, int nLine);
	int				GetStackTraceLevel	();
	
	BOOL			Active				();
//	static CScriptDebugger* GetDebugger	() { return m_pDebugger; };
	LRESULT			_SendMessage(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	void			DrawVariableInfo	(pstr varName);
	void			DrawCurrentState	();
	void			DrawThreadInfo		(int nThreadID);
	void			GetBreakPointsFromIde();
	void			FillBreakPointsIn	(CMailSlotMsg* msg);
	bool			HasBreakPoint		(pcstr fileName, s32 lineNum);
	void			CheckNewMessages	();
	LRESULT			DebugMessage		(UINT nMsg, WPARAM wParam, LPARAM lParam);
	void			WaitForReply		(bool bWaitForModalResult);
	bool			TranslateIdeMessage (CMailSlotMsg*);
	void			SendMessageToIde	(CMailSlotMsg&);


	CDbgScriptThreads					*m_threads;
	CDbgLuaHelper						*m_lua;
	CScriptCallStack					*m_callStack;
//	static CScriptDebugger*				m_pDebugger;
	int									m_nMode;
	int									m_nLevel;  //for step into/over/out
	string_path							m_strPathName;	//for run_to_line_number
	int									m_nLine;		//for run_to_line_number

	HANDLE								m_mailSlot;
	BOOL								m_bIdePresent;

	xr_vector<SBreakPoint>				m_breakPoints;
	string_path							m_curr_connected_mslot;
};
