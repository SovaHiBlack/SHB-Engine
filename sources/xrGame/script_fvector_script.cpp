////////////////////////////////////////////////////////////////////////////
//	Module 		: script_fvector_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float vector script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_fvector.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptFvector::script_register(lua_State *L)
{
	module(L)
	[
		class_<Fvector3>("vector")
			.def_readwrite("x",					&Fvector3::x)
			.def_readwrite("y",					&Fvector3::y)
			.def_readwrite("z",					&Fvector3::z)
			.def(								constructor<>())
			.def("set",							(Fvector3& (Fvector3::*)(float,float,float))(&Fvector3::set),																return_reference_to(_1))
			.def("set",							(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::set),																return_reference_to(_1))
			.def("add",							(Fvector3& (Fvector3::*)(float))(&Fvector3::add),																			return_reference_to(_1))
			.def("add",							(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::add),																return_reference_to(_1))
			.def("add",							(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&))(&Fvector3::add),												return_reference_to(_1))
			.def("add",							(Fvector3& (Fvector3::*)(const Fvector3&, float))(&Fvector3::add),															return_reference_to(_1))
			.def("sub",							(Fvector3& (Fvector3::*)(float))(&Fvector3::sub),																			return_reference_to(_1))
			.def("sub",							(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::sub),																return_reference_to(_1))
			.def("sub",							(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&))(&Fvector3::sub),												return_reference_to(_1))
			.def("sub",							(Fvector3& (Fvector3::*)(const Fvector3&, float))(&Fvector3::sub),															return_reference_to(_1))
			.def("mul",							(Fvector3& (Fvector3::*)(float))(&Fvector3::mul),																			return_reference_to(_1))
			.def("mul",							(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::mul),																return_reference_to(_1))
			.def("mul",							(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&))(&Fvector3::mul),												return_reference_to(_1))
			.def("mul",							(Fvector3& (Fvector3::*)(const Fvector3&, float))(&Fvector3::mul),															return_reference_to(_1))
			.def("div",							(Fvector3& (Fvector3::*)(float))(&Fvector3::div),																			return_reference_to(_1))
			.def("div",							(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::div),																return_reference_to(_1))
			.def("div",							(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&))(&Fvector3::div),												return_reference_to(_1))
			.def("div",							(Fvector3& (Fvector3::*)(const Fvector3&, float))(&Fvector3::div),															return_reference_to(_1))
			.def("invert",						(Fvector3& (Fvector3::*)())(&Fvector3::invert),																			return_reference_to(_1))
			.def("invert",						(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::invert),																return_reference_to(_1))
			.def("min",							(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::min),																return_reference_to(_1))
			.def("min",							(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&))(&Fvector3::min),												return_reference_to(_1))
			.def("max",							(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::max),																return_reference_to(_1))
			.def("max",							(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&))(&Fvector3::max),												return_reference_to(_1))
			.def("abs",							&Fvector3::abs,																											return_reference_to(_1))
			.def("similar",						&Fvector3::similar)
			.def("set_length",					&Fvector3::set_length,																									return_reference_to(_1))
			.def("align",						&Fvector3::align,																										return_reference_to(_1))
//			.def("squeeze",						&Fvector3::squeeze,																										return_reference_to(_1))
			.def("clamp",						(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::clamp),																return_reference_to(_1))
			.def("clamp",						(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&))(&Fvector3::clamp),												return_reference_to(_1))
			.def("inertion",					&Fvector3::inertion,																										return_reference_to(_1))
			.def("average",						(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::average),															return_reference_to(_1))
			.def("average",						(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&))(&Fvector3::average),											return_reference_to(_1))
			.def("lerp",						&Fvector3::lerp,																											return_reference_to(_1))
			.def("mad",							(Fvector3& (Fvector3::*)(const Fvector3&, float))(&Fvector3::mad),															return_reference_to(_1))
			.def("mad",							(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&, float))(&Fvector3::mad),										return_reference_to(_1))
			.def("mad",							(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&))(&Fvector3::mad),												return_reference_to(_1))
			.def("mad",							(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&, const Fvector3&))(&Fvector3::mad),								return_reference_to(_1))
//			.def("square_magnitude",			&Fvector3::square_magnitude)
			.def("magnitude",					&Fvector3::magnitude)
//			.def("normalize_magnitude",			&Fvector3::normalize_magn)
			.def("normalize",					(Fvector3& (Fvector3::*)())(&Fvector3::normalize_safe),																	return_reference_to(_1))
			.def("normalize",					(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::normalize_safe),													return_reference_to(_1))
			.def("normalize_safe",				(Fvector3& (Fvector3::*)())(&Fvector3::normalize_safe),																	return_reference_to(_1))
			.def("normalize_safe",				(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::normalize_safe),													return_reference_to(_1))
//			.def("random_dir",					(Fvector3& (Fvector3::*)())(&Fvector3::random_dir),																		return_reference_to(_1))
//			.def("random_dir",					(Fvector3& (Fvector3::*)(const Fvector3&, float))(&Fvector3::random_dir),													return_reference_to(_1))
//			.def("random_point",				(Fvector3& (Fvector3::*)(const Fvector3&))(&Fvector3::random_point),														return_reference_to(_1))
//			.def("random_point",				(Fvector3& (Fvector3::*)(float))(&Fvector3::random_point),																return_reference_to(_1))
			.def("dotproduct",					&Fvector3::dotproduct)
			.def("crossproduct",				&Fvector3::crossproduct,																									return_reference_to(_1))
			.def("distance_to_xz",				&Fvector3::distance_to_xz)
			.def("distance_to_sqr",				&Fvector3::distance_to_sqr)
			.def("distance_to",					&Fvector3::distance_to)
//			.def("from_bary",					(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&, const Fvector3&, float, float, float))(&Fvector3::from_bary),	return_reference_to(_1))
//			.def("from_bary",					(Fvector3& (Fvector3::*)(const Fvector3&, const Fvector3&, const Fvector3&, const Fvector3&))(&Fvector3::from_bary),		return_reference_to(_1))
//			.def("from_bary4",					&Fvector3::from_bary4,																									return_reference_to(_1))
//			.def("mknormal_non_normalized",		&Fvector3::mknormal_non_normalized,																						return_reference_to(_1))
//			.def("mknormal",					&Fvector3::mknormal,																										return_reference_to(_1))
			.def("setHP",						&Fvector3::setHP,																										return_reference_to(_1))
//			.def("getHP",						&Fvector3::getHP,																																	out_value(_2) + out_value(_3))
			.def("getH",						&Fvector3::getH)
			.def("getP",						&Fvector3::getP)

			.def("reflect",						&Fvector3::reflect,																										return_reference_to(_1))
			.def("slide",						&Fvector3::slide,																										return_reference_to(_1)),
//			.def("generate_orthonormal_basis",	&Fvector3::generate_orthonormal_basis),

		class_<Fbox3>("Fbox")
			.def_readwrite("min",				&Fbox3::min)
			.def_readwrite("max",				&Fbox3::max)
			.def(								constructor<>()),

		class_<Frect>("Frect")
			.def(								constructor<>())
			.def("set",							(Frect & (Frect::*)(float,float,float,float))(&Frect::set),																return_reference_to(_1))
			.def_readwrite("lt",				&Frect::lt)
			.def_readwrite("rb",				&Frect::rb)
			.def_readwrite("x1",				&Frect::x1)
			.def_readwrite("x2",				&Frect::x2)
			.def_readwrite("y1",				&Frect::y1)
			.def_readwrite("y2",				&Frect::y2)
	];
}
