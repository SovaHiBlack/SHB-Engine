#include "stdafx.h"

#include "GrenadeRGD5.h"

CGrenadeRGD5::CGrenadeRGD5( )
{
	m_flags.set				(Fbelt, TRUE);
	m_weight				= .1f;
	m_slot					= GRENADE_SLOT;
}

CGrenadeRGD5::~CGrenadeRGD5( )
{ }

using namespace luabind;

#pragma optimize("s",on)
void CGrenadeRGD5::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CGrenadeRGD5,CGameObject>("CGrenadeRGD5")
			.def(constructor<>())
	];
}
