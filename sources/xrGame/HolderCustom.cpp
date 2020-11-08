#include "stdafx.h"

#include "HolderCustom.h"
#include "Actor.h"

bool CHolderCustom::attach_Actor(CGameObject* actor)
{
	m_owner						= actor;
	m_ownerActor				= smart_cast<CActor*>(actor);

	return true;
}

void CHolderCustom::detach_Actor()
{
	m_owner						= NULL;
	m_ownerActor				= NULL;
}

using namespace luabind;

#pragma optimize("s",on)
void CHolderCustom::script_register(lua_State* L)
{
	module(L)
		[
			class_<CHolderCustom>("holder")
			.def("engaged", &CHolderCustom::Engaged)
			.def("Action", &CHolderCustom::Action)
//			.def("SetParam",		(void (CHolderCustom::*)(int,Fvector2)) &CHolderCustom::SetParam)
			.def("SetParam", (void (CHolderCustom::*)(int, Fvector)) & CHolderCustom::SetParam)
		];
}
