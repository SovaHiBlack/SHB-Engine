#include "stdafx.h"

#include "alife_space.h"
#include "Car.h"
#include "CarWeapon.h"
#include "script_game_object.h"

using namespace luabind;

#pragma optimize("s",on)
void CCar::script_register(lua_State *L)
{
	module(L)
	[
		class_<CCar,bases<CGameObject,CHolderCustom> >("CCar")
			.enum_("wpn_action")
				[
					value("eWpnDesiredDir", s32(CCarWeapon::eWpnDesiredDir)),
					value("eWpnDesiredPos", s32(CCarWeapon::eWpnDesiredPos)),
					value("eWpnActivate", s32(CCarWeapon::eWpnActivate)),
					value("eWpnFire", s32(CCarWeapon::eWpnFire)),
					value("eWpnAutoFire", s32(CCarWeapon::eWpnAutoFire)),
					value("eWpnToDefaultDir", s32(CCarWeapon::eWpnToDefaultDir))
				]
		.def("Action",			&CCar::Action)
//		.def("SetParam",		(void (CCar::*)(s32,fVector2)) &CCar::SetParam)
		.def("SetParam",		(void (CCar::*)(s32, fVector3)) &CCar::SetParam)
		.def("CanHit",			&CCar::WpnCanHit)
		.def("FireDirDiff",		&CCar::FireDirDiff)
		.def("IsObjectVisible",	&CCar::isObjectVisible)
		.def("HasWeapon",		&CCar::HasWeapon)
		.def("CurrentVel",		&CCar::CurrentVel)
		.def("GetfHealth",		&CCar::GetfHealth)
		.def("SetfHealth",		&CCar::SetfHealth)
		.def("SetExplodeTime",	&CCar::SetExplodeTime)
		.def("ExplodeTime",		&CCar::ExplodeTime)
		.def("CarExplode",		&CCar::CarExplode)
		.def(constructor<>())
	];
}
