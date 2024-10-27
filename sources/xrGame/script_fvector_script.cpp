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
		class_<fVector3>("vector")
			.def_readwrite("x",					&fVector3::x)
			.def_readwrite("y",					&fVector3::y)
			.def_readwrite("z",					&fVector3::z)
			.def(								constructor<>())
			.def("set",							(fVector3 & (fVector3::*)(f32, f32, f32))(&fVector3::set),																return_reference_to(_1))
			.def("set",							(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::set),																return_reference_to(_1))
			.def("add",							(fVector3 & (fVector3::*)(f32))(&fVector3::add),																			return_reference_to(_1))
			.def("add",							(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::add),																return_reference_to(_1))
			.def("add",							(fVector3 & (fVector3::*)(const fVector3&, const fVector3&))(&fVector3::add),												return_reference_to(_1))
			.def("add",							(fVector3 & (fVector3::*)(const fVector3&, f32))(&fVector3::add),															return_reference_to(_1))
			.def("sub",							(fVector3 & (fVector3::*)(f32))(&fVector3::sub),																			return_reference_to(_1))
			.def("sub",							(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::sub),																return_reference_to(_1))
			.def("sub",							(fVector3 & (fVector3::*)(const fVector3&, const fVector3&))(&fVector3::sub),												return_reference_to(_1))
			.def("sub",							(fVector3 & (fVector3::*)(const fVector3&, f32))(&fVector3::sub),															return_reference_to(_1))
			.def("mul",							(fVector3 & (fVector3::*)(f32))(&fVector3::mul),																			return_reference_to(_1))
			.def("mul",							(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::mul),																return_reference_to(_1))
			.def("mul",							(fVector3 & (fVector3::*)(const fVector3&, const fVector3&))(&fVector3::mul),												return_reference_to(_1))
			.def("mul",							(fVector3 & (fVector3::*)(const fVector3&, f32))(&fVector3::mul),															return_reference_to(_1))
			.def("div",							(fVector3 & (fVector3::*)(f32))(&fVector3::div),																			return_reference_to(_1))
			.def("div",							(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::div),																return_reference_to(_1))
			.def("div",							(fVector3 & (fVector3::*)(const fVector3&, const fVector3&))(&fVector3::div),												return_reference_to(_1))
			.def("div",							(fVector3 & (fVector3::*)(const fVector3&, f32))(&fVector3::div),															return_reference_to(_1))
			.def("invert",						(fVector3 & (fVector3::*)())(&fVector3::invert),																			return_reference_to(_1))
			.def("invert",						(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::invert),																return_reference_to(_1))
			.def("min",							(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::min),																return_reference_to(_1))
			.def("min",							(fVector3 & (fVector3::*)(const fVector3&, const fVector3&))(&fVector3::min),												return_reference_to(_1))
			.def("max",							(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::max),																return_reference_to(_1))
			.def("max",							(fVector3 & (fVector3::*)(const fVector3&, const fVector3&))(&fVector3::max),												return_reference_to(_1))
			.def("abs",							&fVector3::abs,																											return_reference_to(_1))
			.def("similar",						&fVector3::similar)
			.def("set_length",					&fVector3::set_length,																									return_reference_to(_1))
			.def("align",						&fVector3::align,																										return_reference_to(_1))
//			.def("squeeze",						&fVector3::squeeze,																										return_reference_to(_1))
			.def("clamp",						(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::clamp),																return_reference_to(_1))
			.def("clamp",						(fVector3 & (fVector3::*)(const fVector3&, const fVector3&))(&fVector3::clamp),												return_reference_to(_1))
			.def("inertion",					&fVector3::inertion,																										return_reference_to(_1))
			.def("average",						(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::average),															return_reference_to(_1))
			.def("average",						(fVector3 & (fVector3::*)(const fVector3&, const fVector3&))(&fVector3::average),											return_reference_to(_1))
			.def("lerp",						&fVector3::lerp,																											return_reference_to(_1))
			.def("mad",							(fVector3 & (fVector3::*)(const fVector3&, f32))(&fVector3::mad),															return_reference_to(_1))
			.def("mad",							(fVector3 & (fVector3::*)(const fVector3&, const fVector3&, f32))(&fVector3::mad),										return_reference_to(_1))
			.def("mad",							(fVector3 & (fVector3::*)(const fVector3&, const fVector3&))(&fVector3::mad),												return_reference_to(_1))
			.def("mad",							(fVector3 & (fVector3::*)(const fVector3&, const fVector3&, const fVector3&))(&fVector3::mad),								return_reference_to(_1))
//			.def("square_magnitude",			&fVector3::square_magnitude)
			.def("magnitude",					&fVector3::magnitude)
//			.def("normalize_magnitude",			&fVector3::normalize_magn)
			.def("normalize",					(fVector3 & (fVector3::*)())(&fVector3::normalize_safe),																	return_reference_to(_1))
			.def("normalize",					(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::normalize_safe),													return_reference_to(_1))
			.def("normalize_safe",				(fVector3 & (fVector3::*)())(&fVector3::normalize_safe),																	return_reference_to(_1))
			.def("normalize_safe",				(fVector3 & (fVector3::*)(const fVector3&))(&fVector3::normalize_safe),													return_reference_to(_1))
//			.def("random_dir",					(fVector3 & (fVector3::*)())(&fVector3::random_dir),																		return_reference_to(_1))
//			.def("random_dir",					(fVector3 & (fVector3::*)(const fVector3 &, f32))(&fVector3::random_dir),													return_reference_to(_1))
//			.def("random_point",				(fVector3 & (fVector3::*)(const fVector3 &))(&fVector3::random_point),														return_reference_to(_1))
//			.def("random_point",				(fVector3 & (fVector3::*)(f32))(&fVector3::random_point),																return_reference_to(_1))
			.def("dotproduct",					&fVector3::dotproduct)
			.def("crossproduct",				&fVector3::crossproduct,																									return_reference_to(_1))
			.def("distance_to_xz",				&fVector3::distance_to_xz)
			.def("distance_to_sqr",				&fVector3::distance_to_sqr)
			.def("distance_to",					&fVector3::distance_to)
//			.def("from_bary",					(fVector3 & (fVector3::*)(const fVector3 &, const fVector3 &, const fVector3 &, f32, f32, f32))(&fVector3::from_bary),	return_reference_to(_1))
//			.def("from_bary",					(fVector3 & (fVector3::*)(const fVector3 &, const fVector3 &, const fVector3 &, const fVector3 &))(&fVector3::from_bary),		return_reference_to(_1))
//			.def("from_bary4",					&fVector3::from_bary4,																									return_reference_to(_1))
//			.def("mknormal_non_normalized",		&fVector3::mknormal_non_normalized,																						return_reference_to(_1))
//			.def("mknormal",					&fVector3::mknormal,																										return_reference_to(_1))
			.def("setHP",						&fVector3::setHP,																										return_reference_to(_1))
//			.def("getHP",						&fVector3::getHP,																																	out_value(_2) + out_value(_3))
			.def("getH",						&fVector3::getH)
			.def("getP",						&fVector3::getP)

			.def("reflect",						&fVector3::reflect,																										return_reference_to(_1))
			.def("slide",						&fVector3::slide,																										return_reference_to(_1)),
//			.def("generate_orthonormal_basis",	&fVector3::generate_orthonormal_basis),

		class_<fBox3>("fBox3")
			.def_readwrite("min",				&fBox3::min)
			.def_readwrite("max",				&fBox3::max)
			.def(								constructor<>()),

		class_<fRect>("fRect")
			.def(								constructor<>())
			.def("set",							(fRect & (fRect::*)(f32, f32, f32, f32))(&fRect::set),																return_reference_to(_1))
			.def_readwrite("lt",				&fRect::lt)
			.def_readwrite("rb",				&fRect::rb)
			.def_readwrite("x1",				&fRect::x1)
			.def_readwrite("x2",				&fRect::x2)
			.def_readwrite("y1",				&fRect::y1)
			.def_readwrite("y2",				&fRect::y2)
	];
}
