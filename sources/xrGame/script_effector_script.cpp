////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_effector.h"
#include "script_effector_wrapper.h"

using namespace luabind;

void SPPInfo_assign(SPostProcessInfo* self, SPostProcessInfo* obj)
{
	*self	= *obj;
}

void add_effector(CScriptEffector *self)
{
	self->Add		();
}

void remove_effector(CScriptEffector *self)
{
	self->Remove	();
}

#pragma optimize("s",on)
void CScriptEffector::script_register(lua_State *L)
{
	module(L)
	[
		class_<SPostProcessInfo::SDuality>("duality")
			.def_readwrite("h",					&SPostProcessInfo::SDuality::h)
			.def_readwrite("v",					&SPostProcessInfo::SDuality::v)
			.def(								constructor<>())
			.def(								constructor<float,float>())
			.def("set",							&SPostProcessInfo::SDuality::set),

		class_<SPostProcessInfo::SColor>	("color")
			.def_readwrite("r",					&SPostProcessInfo::SColor::r)
			.def_readwrite("g",					&SPostProcessInfo::SColor::g)
			.def_readwrite("b",					&SPostProcessInfo::SColor::b)
			.def(								constructor<>())
			.def(								constructor<float,float,float>())
			.def("set",							&SPostProcessInfo::SColor::set),

		class_<SPostProcessInfo::SNoise>("noise")
			.def_readwrite("intensity",			&SPostProcessInfo::SNoise::intensity)
			.def_readwrite("grain",				&SPostProcessInfo::SNoise::grain)
			.def_readwrite("fps",				&SPostProcessInfo::SNoise::fps)
			.def(								constructor<>())
			.def(								constructor<float,float,float>())
			.def("set",							&SPostProcessInfo::SNoise::set),

		class_<SPostProcessInfo>("effector_params")
			.def_readwrite("blur",				&SPostProcessInfo::blur)
			.def_readwrite("gray",				&SPostProcessInfo::gray)
			.def_readwrite("dual",				&SPostProcessInfo::duality)
			.def_readwrite("noise",				&SPostProcessInfo::noise)
			.def_readwrite("color_base",		&SPostProcessInfo::color_base)
			.def_readwrite("color_gray",		&SPostProcessInfo::color_gray)
			.def_readwrite("color_add",			&SPostProcessInfo::color_add)
			.def(								constructor<>())
			.def("assign",						&SPPInfo_assign),

		class_<CScriptEffector, CScriptEffectorWrapper>("effector")
			.def(								constructor<int,float>())
			.def("start",						&add_effector,		adopt(_1))
			.def("finish",						&remove_effector,	adopt(_1))
			.def("process",	 					&CScriptEffector::process,	&CScriptEffectorWrapper::process_static)
	];
}
