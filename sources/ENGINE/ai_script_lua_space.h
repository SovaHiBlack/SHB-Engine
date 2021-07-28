#pragma once

namespace Lua
{
	enum ELuaMessageType
	{
		eLuaMessageTypeInfo = U32(0),
		eLuaMessageTypeError,
		eLuaMessageTypeMessage,
		eLuaMessageTypeHookCall,
		eLuaMessageTypeHookReturn,
		eLuaMessageTypeHookLine,
		eLuaMessageTypeHookCount,
		eLuaMessageTypeHookTailReturn = U32(-1)
	};

	int __cdecl LuaOut(ELuaMessageType tLuaMessageType, const char* caFormat, ...);
}

