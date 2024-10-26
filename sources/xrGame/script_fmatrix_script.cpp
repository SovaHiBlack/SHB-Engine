////////////////////////////////////////////////////////////////////////////
//	Module 		: script_fmatrix_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float matrix script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_fmatrix.h"

using namespace luabind;
void get_matrix_hpb(fMatrix4x4* self, f32* h, f32* p, f32* b)
{
	self->getHPB	(*h, *p, *b);
}
void matrix_transform (fMatrix4x4* self, Fvector* v)
{
	self->transform (*v);
}

#pragma optimize("s",on)
void CScriptFmatrix::script_register(lua_State *L)
{
	module(L)
	[
		class_<fMatrix4x4>("matrix")
			.def_readwrite("i",					&fMatrix4x4::i)
			.def_readwrite("_14_",				&fMatrix4x4::_14_)
			.def_readwrite("j",					&fMatrix4x4::j)
			.def_readwrite("_24_",				&fMatrix4x4::_24_)
			.def_readwrite("k",					&fMatrix4x4::k)
			.def_readwrite("_34_",				&fMatrix4x4::_34_)
			.def_readwrite("c",					&fMatrix4x4::c)
			.def_readwrite("_44_",				&fMatrix4x4::_44_)
			.def(								constructor<>())
			.def("set",							(fMatrix4x4 & (fMatrix4x4::*)(const fMatrix4x4&))(&fMatrix4x4::set),																return_reference_to(_1))
			.def("set",							(fMatrix4x4 & (fMatrix4x4::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(&fMatrix4x4::set),				return_reference_to(_1))
			.def("identity",					&fMatrix4x4::identity,																										return_reference_to(_1))
			.def("mk_xform",					&fMatrix4x4::mk_xform,																										return_reference_to(_1))
			.def("mul",							(fMatrix4x4 & (fMatrix4x4::*)(const fMatrix4x4&, const fMatrix4x4&))(&fMatrix4x4::mul),												return_reference_to(_1))
			.def("mul",							(fMatrix4x4 & (fMatrix4x4::*)(const fMatrix4x4&, f32))(&fMatrix4x4::mul),															return_reference_to(_1))
			.def("mul",							(fMatrix4x4 & (fMatrix4x4::*)(f32))(&fMatrix4x4::mul),																			return_reference_to(_1))
			.def("div",							(fMatrix4x4 & (fMatrix4x4::*)(const fMatrix4x4&, f32))(&fMatrix4x4::div),															return_reference_to(_1))
			.def("div",							(fMatrix4x4 & (fMatrix4x4::*)(f32))(&fMatrix4x4::div),																			return_reference_to(_1))
//			.def("invert",						(fMatrix4x4 & (fMatrix4x4::*)())(&fMatrix4x4::invert),																			return_reference_to(_1))
//			.def("invert",						(fMatrix4x4 & (fMatrix4x4::*)(const fMatrix4x4 &))(&fMatrix4x4::invert),																return_reference_to(_1))
//			.def("transpose",					(fMatrix4x4 & (fMatrix4x4::*)())(&fMatrix4x4::transpose),																			return_reference_to(_1))
//			.def("transpose",					(fMatrix4x4 & (fMatrix4x4::*)(const fMatrix4x4 &))(&fMatrix4x4::transpose),															return_reference_to(_1))
//			.def("translate",					(fMatrix4x4 & (fMatrix4x4::*)(const Fvector &))(&fMatrix4x4::translate),															return_reference_to(_1))
//			.def("translate",					(fMatrix4x4 & (fMatrix4x4::*)(f32, f32, f32))(&fMatrix4x4::translate),														return_reference_to(_1))
//			.def("translate_over",				(fMatrix4x4 & (fMatrix4x4::*)(const Fvector &))(&fMatrix4x4::translate_over),														return_reference_to(_1))
//			.def("translate_over",				(fMatrix4x4 & (fMatrix4x4::*)(f32, f32, f32))(&fMatrix4x4::translate_over),													return_reference_to(_1))
//			.def("translate_add",				&fMatrix4x4::translate_add,																								return_reference_to(_1))
//			.def("scale",						(fMatrix4x4 & (fMatrix4x4::*)(const Fvector &))(&fMatrix4x4::scale),																return_reference_to(_1))
//			.def("scale",						(fMatrix4x4 & (fMatrix4x4::*)(f32, f32, f32))(&fMatrix4x4::scale),															return_reference_to(_1))
//			.def("rotateX",						&fMatrix4x4::rotateX,																										return_reference_to(_1))
//			.def("rotateY",						&fMatrix4x4::rotateY,																										return_reference_to(_1))
//			.def("rotateZ",						&fMatrix4x4::rotateZ,																										return_reference_to(_1))
//			.def("rotation",					(fMatrix4x4 & (fMatrix4x4::*)(const Fvector &, const Fvector &))(&fMatrix4x4::rotation),											return_reference_to(_1))
//			.def("rotation",					(fMatrix4x4 & (fMatrix4x4::*)(const Fvector &, f32))(&fMatrix4x4::rotation),													return_reference_to(_1))
//			.def("rotation",					&fMatrix4x4::rotation,																										return_reference_to(_1))
/*
			.def("mapXYZ",						&fMatrix4x4::mapXYZ,																										return_reference_to(_1))
			.def("mapXZY",						&fMatrix4x4::mapXZY,																										return_reference_to(_1))
			.def("mapYXZ",						&fMatrix4x4::mapYXZ,																										return_reference_to(_1))
			.def("mapYZX",						&fMatrix4x4::mapYZX,																										return_reference_to(_1))
			.def("mapZXY",						&fMatrix4x4::mapZXY,																										return_reference_to(_1))
			.def("mapZYX",						&fMatrix4x4::mapZYX,																										return_reference_to(_1))
			.def("mirrorX",						&fMatrix4x4::mirrorX,																										return_reference_to(_1))
			.def("mirrorX_over",				&fMatrix4x4::mirrorX_over,																									return_reference_to(_1))
			.def("mirrorX_add ",				&fMatrix4x4::mirrorX_add,																									return_reference_to(_1))
			.def("mirrorY",						&fMatrix4x4::mirrorY,																										return_reference_to(_1))
			.def("mirrorY_over",				&fMatrix4x4::mirrorY_over,																									return_reference_to(_1))
			.def("mirrorY_add ",				&fMatrix4x4::mirrorY_add,																									return_reference_to(_1))
			.def("mirrorZ",						&fMatrix4x4::mirrorZ,																										return_reference_to(_1))
			.def("mirrorZ_over",				&fMatrix4x4::mirrorZ_over,																									return_reference_to(_1))
			.def("mirrorZ_add ",				&fMatrix4x4::mirrorZ_add,																									return_reference_to(_1))
*/
//			.def("build_projection",			&fMatrix4x4::build_projection,																								return_reference_to(_1))
//			.def("build_projection_HAT",		&fMatrix4x4::build_projection_HAT,																							return_reference_to(_1))
//			.def("build_projection_ortho",		&fMatrix4x4::build_projection_ortho,																						return_reference_to(_1))
//			.def("build_camera",				&fMatrix4x4::build_camera,																									return_reference_to(_1))
//			.def("build_camera_dir",			&fMatrix4x4::build_camera_dir,																								return_reference_to(_1))
//			.def("inertion",					&fMatrix4x4::inertion,																										return_reference_to(_1))
//			.def("transform_tiny32",			&fMatrix4x4::transform_tiny32)
//			.def("transform_tiny23",			&fMatrix4x4::transform_tiny23)
//			.def("transform_tiny",				(void	   (fMatrix4x4::*)(Fvector &) const)(&fMatrix4x4::transform_tiny),																					out_value(_2))
//			.def("transform_tiny",				(void	   (fMatrix4x4::*)(Fvector &, const Fvector &) const)(&fMatrix4x4::transform_tiny),																out_value(_2))
//			.def("transform_dir",				(void	   (fMatrix4x4::*)(Fvector &) const)(&fMatrix4x4::transform_dir),																					out_value(_2))
//			.def("transform_dir",				(void	   (fMatrix4x4::*)(Fvector &, const Fvector &) const)(&fMatrix4x4::transform_dir),																	out_value(_2))
//			.def("transform",					(void	   (fMatrix4x4::*)(Fvector &) const)(&fMatrix4x4::transform),																						out_value(_2))
//			.def("transform",					&matrix_transform)
			.def("setHPB",						&fMatrix4x4::setHPB,																										return_reference_to(_1))
//			.def("setXYZ",						(fMatrix4x4 & (fMatrix4x4::*)(Fvector &))(&fMatrix4x4::setXYZ),																	return_reference_to(_1)	+	out_value(_2))
			.def("setXYZ",						(fMatrix4x4 & (fMatrix4x4::*)(f32, f32, f32))(&fMatrix4x4::setXYZ),															return_reference_to(_1))
//			.def("setXYZi",						(fMatrix4x4 & (fMatrix4x4::*)(Fvector &))(&fMatrix4x4::setXYZi),																	return_reference_to(_1) +	out_value(_2))
			.def("setXYZi",						(fMatrix4x4 & (fMatrix4x4::*)(f32, f32, f32))(&fMatrix4x4::setXYZi),														return_reference_to(_1))
//			.def("getHPB",						(void	   (fMatrix4x4::*)(Fvector &) const)(&fMatrix4x4::getHPB),																							out_value(_2))
			.def("getHPB",						&get_matrix_hpb)
//			.def("getXYZ",						(void	   (fMatrix4x4::*)(Fvector &) const)(&fMatrix4x4::getXYZ),																							out_value(_2))
//			.def("getXYZ",						(void	   (fMatrix4x4::*)(f32 &, f32 &, f32 &) const)(&fMatrix4x4::getXYZ))
//			.def("getXYZi",						(void	   (fMatrix4x4::*)(Fvector &) const)(&fMatrix4x4::getXYZi),																						out_value(_2))
//			.def("getXYZi",						(void	   (fMatrix4x4::*)(f32 &, f32 &, f32 &) const)(&fMatrix4x4::getXYZi))
	];
}
