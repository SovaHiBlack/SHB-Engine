////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_script.cpp
//	Created 	: 27.05.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "object_factory.h"
#include "AISpace.h"
#include "script_engine.h"
#include "object_item_script.h"

void CObjectFactory::register_script_class	(pcstr client_class, pcstr server_class, pcstr clsid, pcstr script_clsid)
{
#ifndef NO_XR_GAME
	luabind::object				client;
	if (!ai().script_engine().function_object(client_class,client,LUA_TUSERDATA)) {
		ai().script_engine().script_log	(eLuaMessageTypeError,"Cannot register class %s",client_class);
		return;
	}
#endif
	luabind::object				server;
	if (!ai().script_engine().function_object(server_class,server,LUA_TUSERDATA)) {
		ai().script_engine().script_log	(eLuaMessageTypeError,"Cannot register class %s",server_class);
		return;
	}
	
	add							(
		xr_new<CObjectItemScript>(
#ifndef NO_XR_GAME
			client,
#endif
			server,
			TEXT2CLSID(clsid),
			script_clsid
		)
	);
}

void CObjectFactory::register_script_class			(pcstr unknown_class, pcstr clsid, pcstr script_clsid)
{
	luabind::object				creator;
	if (!ai().script_engine().function_object(unknown_class,creator,LUA_TUSERDATA)) {
		ai().script_engine().script_log	(eLuaMessageTypeError,"Cannot register class %s",unknown_class);
		return;
	}
	add							(
		xr_new<CObjectItemScript>(
#ifndef NO_XR_GAME
			creator,
#endif
			creator,
			TEXT2CLSID(clsid),
			script_clsid
		)
	);
}

void CObjectFactory::register_script_classes()
{
	ai();
}

using namespace luabind;

struct CInternal{};

void CObjectFactory::register_script	() const
{
	actualize					();

	luabind::class_<CInternal>	instance("clsid");

	const_iterator				I = clsids().begin(), B = I;
	const_iterator				E = clsids().end();
	for ( ; I != E; ++I)
		instance.enum_			("_clsid")[luabind::value(*(*I)->script_clsid(), s32(I - B))];

	luabind::module				(ai().script_engine().lua())[instance];
}

#pragma optimize("s",on)
void CObjectFactory::script_register(lua_State *L)
{
	module(L)
	[
		class_<CObjectFactory>("object_factory")
			.def("register",	(void (CObjectFactory::*)(pcstr, pcstr, pcstr, pcstr))(&CObjectFactory::register_script_class))
			.def("register",	(void (CObjectFactory::*)(pcstr, pcstr, pcstr))(&CObjectFactory::register_script_class))
	];
}
