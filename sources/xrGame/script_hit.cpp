////////////////////////////////////////////////////////////////////////////
//	Module 		: script_hit.cpp
//	Created 	: 06.02.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script hit class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_hit.h"

CScriptHit::~CScriptHit	()
{
}
#include "script_game_object.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptHit::script_register(lua_State* L)
{
	module(L)
		[
			class_<CScriptHit>("hit")
			.enum_("hit_type")
		[
			value("burn", s32(ALife::eHitTypeBurn)),
			value("shock", s32(ALife::eHitTypeShock)),
			value("strike", s32(ALife::eHitTypeStrike)),
			value("wound", s32(ALife::eHitTypeWound)),
			value("radiation", s32(ALife::eHitTypeRadiation)),
			value("telepatic", s32(ALife::eHitTypeTelepatic)),
			value("chemical_burn", s32(ALife::eHitTypeChemicalBurn)),
			value("explosion", s32(ALife::eHitTypeExplosion)),
			value("fire_wound", s32(ALife::eHitTypeFireWound)),
			value("dummy", s32(ALife::eHitTypeMax))
		]
	.def_readwrite("power", &CScriptHit::m_fPower)
		.def_readwrite("direction", &CScriptHit::m_tDirection)
		.def_readwrite("draftsman", &CScriptHit::m_tpDraftsman)
		.def_readwrite("impulse", &CScriptHit::m_fImpulse)
		.def_readwrite("type", &CScriptHit::m_tHitType)
		.def(constructor<>( ))
		.def(constructor<const CScriptHit*>( ))
		.def("bone", &CScriptHit::set_bone_name)
		];
}
