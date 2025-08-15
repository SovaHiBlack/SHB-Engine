////////////////////////////////////////////////////////////////////////////
//	Module 		: saved_game_wrapper_script.cpp
//	Created 	: 21.02.2006
//  Modified 	: 21.02.2006
//	Author		: Dmitriy Iassenev
//	Description : saved game wrapper class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "saved_game_wrapper.h"
#include "AISpace.h"
#include "game_graph.h"
#include "xr_time.h"

using namespace luabind;

�Time CSavedGameWrapper__game_time(const CSavedGameWrapper* self)
{
	return �Time(self->game_time( ));
}

pcstr CSavedGameWrapper__level_name(const CSavedGameWrapper* self)
{
	return *ai( ).game_graph( ).header( ).level(self->level_id( )).name( );
}

#pragma optimize("s",on)
void CSavedGameWrapper::script_register(lua_State* L)
{
	module(L)
		[
			class_<CSavedGameWrapper>("CSavedGameWrapper")
				.def(constructor<pcstr>( ))
				.def("game_time", &CSavedGameWrapper__game_time)
				.def("level_name", &CSavedGameWrapper__level_name)
				.def("level_id", &CSavedGameWrapper::level_id)
				.def("actor_health", &CSavedGameWrapper::actor_health),

				def("valid_saved_game", (bool (*)(pcstr))(&valid_saved_game))
		];
}
