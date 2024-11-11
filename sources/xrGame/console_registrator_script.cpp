#include "stdafx.h"

#include "console_registrator.h"
#include "..\XR_3DA\Console.h"

using namespace luabind;

CConsole*	console()
{
	return Console;
}

int get_console_integer(CConsole* c, pcstr cmd)
{
	int val=0,min=0,max=0;
	c->GetInteger ( cmd, val, min, max);
	return val;
}

f32 get_console_float(CConsole* c, pcstr cmd)
{
	f32 val = 0.0f;
	f32 min = 0.0f;
	f32 max = 0.0f;
	c->GetFloat ( cmd, val, min, max);
	return val;
}

bool get_console_bool(CConsole* c, pcstr cmd)
{
	BOOL val;
	val = c->GetBool (cmd, val);
	return !!val;
}

#pragma optimize("s",on)
void console_registrator::script_register(lua_State *L)
{
	module(L)
	[
		def("get_console",					&console),
		class_<CConsole>("CConsole")
		.def("execute",						&CConsole::Execute)
		.def("execute_script",				&CConsole::ExecuteScript)
		.def("show",						&CConsole::Show)
		.def("hide",						&CConsole::Hide)
//		.def("save",						&CConsole::Save)
		.def("get_string",					&CConsole::GetString)
		.def("get_integer",					&get_console_integer)
		.def("get_bool",					&get_console_bool)
		.def("get_float",					&get_console_float)
		.def("get_token",					&CConsole::GetToken)
//		.def("",				&CConsole::)
	];
}