#include "stdafx.h"

#include "RGD5.h"

CRGD5::CRGD5( )
{
	m_flags.set(Fbelt, TRUE);
	m_weight = 0.1f;
	m_slot = GRENADE_SLOT;
}

CRGD5::~CRGD5( )
{ }

using namespace luabind;

#pragma optimize("s",on)
void CRGD5::script_register(lua_State* L)
{
	module(L)
		[
			class_<CRGD5, CGameObject>("CRGD5")
				.def(constructor<>( ))
		];
}
