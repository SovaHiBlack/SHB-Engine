////////////////////////////////////////////////////////////////////////////
//	Module 		: particle_params.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Particle parameters class
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "particle_params.h"

using namespace luabind;

#pragma optimize("s",on)
void CParticleParams::script_register(lua_State *L)
{
	module(L)
	[
		class_<CParticleParams>("particle_params")
			.def(								constructor<>())
			.def(								constructor<const fVector3&>())
			.def(								constructor<const fVector3&,const fVector3&>())
			.def(								constructor<const fVector3&,const fVector3&,const fVector3&>())
	];
}
