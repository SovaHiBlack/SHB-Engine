#include "stdafx.h"

#include <dxerr.h>

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#include <direct.h>
#pragma warning(pop)

extern bool shared_str_initialized;

#define DEBUG_INVOKE	__asm int 3

#pragma comment(lib, "dxerr.lib")

#include <dbghelp.h>						// MiniDump flags
#include <new.h>							// for _set_new_mode
#include <signal.h>							// for signals

CORE_API CDebug		Debug;

static bool error_after_dialog = false;
extern void copy_to_clipboard(const char* string);

void copy_to_clipboard(const char* string)
{
	if (IsDebuggerPresent( ))
	{
		return;
	}

	if (!OpenClipboard(0))
	{
		return;
	}

	HGLOBAL handle = GlobalAlloc(GHND | GMEM_DDESHARE, (strlen(string) + 1) * sizeof(char));
	if (!handle)
	{
		CloseClipboard( );
		return;
	}

	char* memory = (char*) GlobalLock(handle);
	strcpy(memory, string);
	GlobalUnlock(handle);
	EmptyClipboard( );
	SetClipboardData(CF_TEXT, handle);
	CloseClipboard( );
}

void update_clipboard(const char* string)
{

#ifdef DEBUG
	if (IsDebuggerPresent( ))
	{
		return;
	}

	if (!OpenClipboard(0))
	{
		return;
	}

	HGLOBAL handle = GetClipboardData(CF_TEXT);
	if (!handle)
	{
		CloseClipboard( );
		copy_to_clipboard(string);
		return;
	}

	char* memory = (char*) GlobalLock(handle);
	U32 memory_length = xr_strlen(memory);
	U32 string_length = xr_strlen(string);
	char* buffer = (char*) _alloca((memory_length + string_length + 1) * sizeof(char));
	strcpy(buffer, memory);
	GlobalUnlock(handle);

	strcat(buffer, string);
	CloseClipboard( );
	copy_to_clipboard(buffer);
#endif // def DEBUG

}

extern void BuildStackTrace( );
extern char g_stackTrace[100][4096];
extern int	g_stackTraceCount;

void LogStackTrace(const char* header)
{
	if (!shared_str_initialized)
	{
		return;
	}

	BuildStackTrace( );

	Msg("%s", header);

	for (int i = 1; i < g_stackTraceCount; ++i)
	{
		Msg("%s", g_stackTrace[i]);
	}
}

void gather_info(const char* expression, const char* description, const char* argument0, const char* argument1, const char* file, int line, const char* function, char* assertion_info)
{
	char* buffer = assertion_info;
	const char* endline = "\n";
	const char* prefix = "[error]";
	bool extended_description = (description && !argument0 && strchr(description, '\n'));
	for (int i = 0; i < 2; ++i)
	{
		if (!i)
		{
			buffer += sprintf(buffer, "%sFATAL ERROR%s%s", endline, endline, endline);
		}

		buffer += sprintf(buffer, "%sExpression    : %s%s", prefix, expression, endline);
		buffer += sprintf(buffer, "%sFunction      : %s%s", prefix, function, endline);
		buffer += sprintf(buffer, "%sFile          : %s%s", prefix, file, endline);
		buffer += sprintf(buffer, "%sLine          : %d%s", prefix, line, endline);

		if (extended_description)
		{
			buffer += sprintf(buffer, "%s%s%s", endline, description, endline);
		}
		else
		{
			buffer += sprintf(buffer, "%sDescription   : %s%s", prefix, description, endline);
			if (argument0)
			{
				if (argument1)
				{
					buffer += sprintf(buffer, "%sArgument 0    : %s%s", prefix, argument0, endline);
					buffer += sprintf(buffer, "%sArgument 1    : %s%s", prefix, argument1, endline);
				}
				else
				{
					buffer += sprintf(buffer, "%sArguments     : %s%s", prefix, argument0, endline);
				}
			}
		}

		buffer += sprintf(buffer, "%s", endline);
		if (!i)
		{
			if (shared_str_initialized)
			{
				Msg("%s", assertion_info);
				FlushLog( );
			}

			buffer = assertion_info;
			endline = "\r\n";
			prefix = "";
		}
	}

	if (!IsDebuggerPresent( ) && !strstr(GetCommandLine( ), "-no_call_stack_assert"))
	{
		if (shared_str_initialized)
		{
			Msg("stack trace:\n");
		}

		BuildStackTrace( );

		for (int i = 2; i < g_stackTraceCount; ++i)
		{
			if (shared_str_initialized)
			{
				Msg("%s", g_stackTrace[i]);
			}
		}

		if (shared_str_initialized)
		{
			FlushLog( );
		}

		copy_to_clipboard(assertion_info);
	}
}

void CDebug::do_exit(const std::string& message)
{
	FlushLog( );
	MessageBox(NULL, message.c_str( ), "Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
	TerminateProcess(GetCurrentProcess( ), 1);
}

void CDebug::backend(const char* expression, const char* description, const char* argument0, const char* argument1, const char* file, int line, const char* function, bool& ignore_always)
{
	static xrCriticalSection CS;
	CS.Enter( );

	error_after_dialog = true;

	string4096 assertion_info;
	gather_info(expression, description, argument0, argument1, file, line, function, assertion_info);

	if (handler)
	{
		handler( );
	}

	if (get_on_dialog( ))
	{
		get_on_dialog( )	(true);
	}

	DEBUG_INVOKE;

	if (get_on_dialog( ))
	{
		get_on_dialog( )	(false);
	}

	CS.Leave( );
}

const char* CDebug::error2string(long code)
{
	const char* result = 0;
	static string1024 desc_storage;

	result = DXGetErrorDescription(code);

	if (0 == result)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, code, 0, desc_storage, sizeof(desc_storage) - 1, 0);
		result = desc_storage;
	}

	return result;
}

void CDebug::error(long hr, const char* expr, const char* file, int line, const char* function, bool& ignore_always)
{
	backend(error2string(hr), expr, 0, 0, file, line, function, ignore_always);
}

void CDebug::error(long hr, const char* expr, const char* e2, const char* file, int line, const char* function, bool& ignore_always)
{
	backend(error2string(hr), expr, e2, 0, file, line, function, ignore_always);
}

void CDebug::fail(const char* e1, const char* file, int line, const char* function, bool& ignore_always)
{
	backend("assertion failed", e1, 0, 0, file, line, function, ignore_always);
}

void CDebug::fail(const char* e1, const std::string& e2, const char* file, int line, const char* function, bool& ignore_always)
{
	backend(e1, e2.c_str( ), 0, 0, file, line, function, ignore_always);
}

void CDebug::fail(const char* e1, const char* e2, const char* file, int line, const char* function, bool& ignore_always)
{
	backend(e1, e2, 0, 0, file, line, function, ignore_always);
}

void CDebug::fail(const char* e1, const char* e2, const char* e3, const char* file, int line, const char* function, bool& ignore_always)
{
	backend(e1, e2, e3, 0, file, line, function, ignore_always);
}

void CDebug::fail(const char* e1, const char* e2, const char* e3, const char* e4, const char* file, int line, const char* function, bool& ignore_always)
{
	backend(e1, e2, e3, e4, file, line, function, ignore_always);
}

void __cdecl CDebug::fatal(const char* file, int line, const char* function, const char* F, ...)
{
	string1024 buffer;

	va_list p;
	va_start(p, F);
	vsprintf(buffer, F, p);
	va_end(p);

	bool ignore_always = true;

	backend("fatal error", "<no expression>", buffer, 0, file, line, function, ignore_always);
}

int out_of_memory_handler(size_t size)
{
	Memory.mem_compact( );
	U32 process_heap = mem_usage_impl(nullptr, nullptr);
	U32 eco_strings = g_pStringContainer->stat_economy( );
	U32 eco_smem = g_pSharedMemoryContainer->stat_economy( );
	Msg("* [x-ray]: process heap[%d K]", process_heap / 1024);
	Msg("* [x-ray]: economy: strings[%d K], smem[%d K]", eco_strings / 1024, eco_smem);
	Debug.fatal(DEBUG_INFO, "Out of memory. Memory request: %d K", size / 1024);
	return 1;
}

extern const char* log_name( );

#if 1
extern void BuildStackTrace(struct _EXCEPTION_POINTERS* pExceptionInfo);
typedef LONG WINAPI UnhandledExceptionFilterType(struct _EXCEPTION_POINTERS* pExceptionInfo);
typedef LONG(__stdcall* PFNCHFILTFN) (EXCEPTION_POINTERS* pExPtrs);
extern "C" BOOL __stdcall SetCrashHandlerFilter(PFNCHFILTFN pFn);

static UnhandledExceptionFilterType* previous_filter = 0;

void format_message(char* buffer, const U32& buffer_size)
{
	void* message;
	DWORD error_code = GetLastError( );

	if (!error_code)
	{
		*buffer = 0;
		return;
	}

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*) &message, 0, NULL);

	sprintf(buffer, "[error][%8d]    : %s", error_code, message);
	LocalFree(message);
}

LONG WINAPI UnhandledFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
	string256 error_message;
	format_message(error_message, sizeof(error_message));

	if (!error_after_dialog && !strstr(GetCommandLine( ), "-no_call_stack_assert"))
	{
		CONTEXT save = *pExceptionInfo->ContextRecord;
		BuildStackTrace(pExceptionInfo);
		*pExceptionInfo->ContextRecord = save;

		if (shared_str_initialized)
		{
			Msg("stack trace:\n");
		}

		copy_to_clipboard("stack trace:\r\n\r\n");

		string4096 buffer;
		for (int i = 0; i < g_stackTraceCount; ++i)
		{
			if (shared_str_initialized)
			{
				Msg("%s", g_stackTrace[i]);
			}

			sprintf(buffer, "%s\r\n", g_stackTrace[i]);
			update_clipboard(buffer);
		}

		if (*error_message)
		{
			if (shared_str_initialized)
			{
				Msg("\n%s", error_message);
			}

			strcat(error_message, "\r\n");
			update_clipboard(error_message);
		}
	}

	if (shared_str_initialized)
	{
		FlushLog( );
	}

	if (!previous_filter)
	{
		return				(EXCEPTION_CONTINUE_SEARCH);
	}

	previous_filter(pExceptionInfo);

	return					(EXCEPTION_CONTINUE_SEARCH);
}
#endif

//////////////////////////////////////////////////////////////////////
typedef int(__cdecl* _PNH)(size_t);
_CRTIMP int		__cdecl _set_new_mode(int);
_CRTIMP _PNH	__cdecl _set_new_handler(_PNH);

void _terminate( )
{
	if (strstr(GetCommandLine( ), "-silent_error_mode"))
	{
		exit(-1);
	}

	string4096 assertion_info;

	gather_info(
		"<no expression>",
		"Unexpected application termination",
		0,
		0,
		__FILE__,
		__LINE__,
		__FUNCTION__,
		assertion_info
	);

	const char* endline = "\r\n";
	char* buffer = assertion_info + xr_strlen(assertion_info);
	buffer += sprintf(buffer, "Press OK to abort execution%s", endline);

	MessageBox(
		GetTopWindow(NULL),
		assertion_info,
		"Fatal Error",
		MB_OK | MB_ICONERROR | MB_SYSTEMMODAL
	);

	exit(-1);
}

void debug_on_thread_spawn( )
{
	std::set_terminate(_terminate);
}

static void handler_base(const char* reason_string)
{
	bool							ignore_always = false;
	Debug.backend(
		"error handler is invoked!",
		reason_string,
		0,
		0,
		DEBUG_INFO,
		ignore_always
	);
}

static void invalid_parameter_handler(
	const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	U32 line,
	uintptr_t reserved
)
{
	bool							ignore_always = false;

	string4096						expression_;
	string4096						function_;
	string4096						file_;
	size_t							converted_chars = 0;

	if (expression)
		wcstombs_s(
		&converted_chars,
		expression_,
		sizeof(expression_),
		expression,
		(wcslen(expression) + 1) * 2 * sizeof(char)
		);
	else
		strcpy_s(expression_, "");

	if (function)
		wcstombs_s(
		&converted_chars,
		function_,
		sizeof(function_),
		function,
		(wcslen(function) + 1) * 2 * sizeof(char)
		);
	else
		strcpy_s(function_, __FUNCTION__);

	if (file)
		wcstombs_s(
		&converted_chars,
		file_,
		sizeof(file_),
		file,
		(wcslen(file) + 1) * 2 * sizeof(char)
		);
	else
	{
		line = __LINE__;
		strcpy_s(file_, __FILE__);
	}

	Debug.backend(
		"error handler is invoked!",
		expression_,
		0,
		0,
		file_,
		line,
		function_,
		ignore_always
	);
}

static void std_out_of_memory_handler( )
{
	handler_base("std: out of memory");
}

static void pure_call_handler( )
{
	handler_base("pure virtual function call");
}

static void abort_handler(int signal)
{
	handler_base("application is aborting");
}

static void floating_point_handler(int signal)
{
	handler_base("floating point error");
}

static void illegal_instruction_handler(int signal)
{
	handler_base("illegal instruction");
}

static void termination_handler(int signal)
{
	handler_base("termination with exit code 3");
}

void CDebug::_initialize( )
{
	debug_on_thread_spawn( );

	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
	signal(SIGABRT, abort_handler);
	signal(SIGABRT_COMPAT, abort_handler);
	signal(SIGFPE, floating_point_handler);
	signal(SIGILL, illegal_instruction_handler);
	signal(SIGINT, 0);
	signal(SIGTERM, termination_handler);

	_set_invalid_parameter_handler(&invalid_parameter_handler);

	_set_new_mode(1);
	_set_new_handler(&out_of_memory_handler);
	std::set_new_handler(&std_out_of_memory_handler);

	_set_purecall_handler(&pure_call_handler);

	previous_filter = ::SetUnhandledExceptionFilter(UnhandledFilter);	// exception handler to all "unhandled" exceptions
}
