#pragma once

namespace Lua
{
	enum ELuaMessageType
	{
		eLuaMessageTypeInfo = unsigned int(0),
		eLuaMessageTypeError,
		eLuaMessageTypeMessage,
		eLuaMessageTypeHookCall,
		eLuaMessageTypeHookReturn,
		eLuaMessageTypeHookLine,
		eLuaMessageTypeHookCount,
		eLuaMessageTypeHookTailReturn = unsigned int(-1)
	};

	int __cdecl LuaOut(ELuaMessageType tLuaMessageType, const char* caFormat, ...);
}

