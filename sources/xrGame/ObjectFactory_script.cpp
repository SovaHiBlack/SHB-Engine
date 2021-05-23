//	Module 		: object_factory_script.cpp
//	Description : Object factory script export

#include "stdafx.h"

#include "ObjectFactory.h"
#include "ai_space.h"
#include "script_engine.h"
#include "ObjectItemScript.h"

void CObjectFactory::register_script_class(const char* client_class, const char* server_class, const char* clsid, const char* script_clsid)
{
	luabind::object				client;
	if (!ai( ).script_engine( ).function_object(client_class, client, LUA_TUSERDATA))
	{
		ai( ).script_engine( ).script_log(eLuaMessageTypeError, "Cannot register class %s", client_class);
		return;
	}

	luabind::object				server;
	if (!ai( ).script_engine( ).function_object(server_class, server, LUA_TUSERDATA))
	{
		ai( ).script_engine( ).script_log(eLuaMessageTypeError, "Cannot register class %s", server_class);
		return;
	}

	add(xr_new<CObjectItemScript>(client, server, TEXT2CLSID(clsid), script_clsid));
}

void CObjectFactory::register_script_class(const char* unknown_class, const char* clsid, const char* script_clsid)
{
	luabind::object				creator;
	if (!ai( ).script_engine( ).function_object(unknown_class, creator, LUA_TUSERDATA))
	{
		ai( ).script_engine( ).script_log(eLuaMessageTypeError, "Cannot register class %s", unknown_class);
		return;
	}

	add(xr_new<CObjectItemScript>(creator, creator, TEXT2CLSID(clsid), script_clsid));
}

void CObjectFactory::register_script_classes( )
{
	ai( );
}

using namespace luabind;

struct CInternal
{ };

void CObjectFactory::register_script( ) const
{
	actualize( );

	luabind::class_<CInternal>	instance("clsid");

	const_iterator				I = clsids( ).begin( ), B = I;
	const_iterator				E = clsids( ).end( );
	for (; I != E; ++I)
		instance.enum_("_clsid")[luabind::value(*(*I)->script_clsid( ), int(I - B))];

	luabind::module(ai( ).script_engine( ).lua( ))[instance];
}

#pragma optimize("s",on)
void CObjectFactory::script_register(lua_State* L)
{
	module(L)
		[
			class_<CObjectFactory>("object_factory")
			.def("register", (void (CObjectFactory::*)(const char*, const char*, const char*, const char*))(&CObjectFactory::register_script_class))
		.def("register", (void (CObjectFactory::*)(const char*, const char*, const char*))(&CObjectFactory::register_script_class))
		];
}
