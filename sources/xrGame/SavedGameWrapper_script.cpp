//	Module 		: SavedGameWrapper_script.cpp
//	Description : saved game wrapper class script export

#include "stdafx.h"

#include "SavedGameWrapper.h"
#include "ai_space.h"
#include "game_graph.h"
#include "xrTime.h"

using namespace luabind;

xrTime CSavedGameWrapper__game_time(const CSavedGameWrapper* self)
{
	return xrTime(self->game_time( ));
}

const char* CSavedGameWrapper__level_name(const CSavedGameWrapper* self)
{
	return *ai( ).game_graph( ).header( ).level(self->level_id( )).name( );
}

#pragma optimize("s", on)
void CSavedGameWrapper::script_register(lua_State* L)
{
	module(L)
		[
			class_<CSavedGameWrapper>("CSavedGameWrapper")
			.def(constructor<const char*>( ))
			.def("game_time",			&CSavedGameWrapper__game_time)
			.def("level_name",			&CSavedGameWrapper__level_name)
			.def("level_id",			&CSavedGameWrapper::level_id)
			.def("actor_health",		&CSavedGameWrapper::actor_health),

			def("valid_saved_game",		(bool (*)(const char*))(&valid_saved_game))
		];
}
