////////////////////////////////////////////////////////////////////////////
//	Module 		: script_flags_script.cpp
//	Created 	: 19.07.2004
//  Modified 	: 19.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script flags script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_flags.h"

using namespace luabind;

template <typename T>
T& set(T *self, const typename T::TYPE mask, bool value)
{
	return	(self->set(mask,value));
}

template <typename T>
bool is(T *self, const typename T::TYPE mask)
{
	return	(!!self->is(mask));
}

template <typename T>
bool is_any(T *self, const typename T::TYPE mask)
{
	return	(!!self->is_any(mask));
}

template <typename T>
bool test(T *self, const typename T::TYPE mask)
{
	return	(!!self->test(mask));
}

template <typename T>
bool equal(T *self, const T &f)
{
	return	(!!self->equal(f));
}

template <typename T>
bool equal(T *self, const T &f, const typename T::TYPE mask)
{
	return	(!!self->equal(f,mask));
}

template <typename T>
void one(T *self)
{
	self->assign(typename T::TYPE(-1));
}

#pragma optimize("s",on)
void CScriptFlags::script_register(lua_State *L)
{
	module(L)
	[
//		class_<flags8>		("flags8")
//			.def(			constructor<>())
//			.def("get",		&flags8::get)
//			.def("zero",	&flags8::zero)
//			.def("one",		&one<flags8>)
//			.def("invert",	(flags8& (flags8::*)())(&flags8::invert))
//			.def("invert",	(flags8& (flags8::*)(const flags8&))(&flags8::invert))
//			.def("invert",	(flags8& (flags8::*)(const flags8::TYPE))(&flags8::invert))
//			.def("assign",	(flags8& (flags8::*)(const flags8&))(&flags8::assign))
//			.def("assign",	(flags8& (flags8::*)(const flags8::TYPE))(&flags8::assign))
//			.def("or",		(flags8& (flags8::*)(const flags8::TYPE))(&flags8::or))
//			.def("or",		(flags8& (flags8::*)(const flags8&,const flags8::TYPE))(&flags8::or))
//			.def("and",		(flags8& (flags8::*)(const flags8::TYPE))(&flags8::and))
//			.def("and",		(flags8& (flags8::*)(const flags8&,const flags8::TYPE))(&flags8::and))
//			.def("set",		&set<flags8>)
//			.def("is",		&is<flags8>)
//			.def("is_any",	&is_any<flags8>)
//			.def("test",	&test<flags8>)
//			.def("equal",	(bool (*)(flags8*,const flags8&))(&equal<flags8>))
//			.def("equal",	(bool (*)(flags8*,const flags8&,const flags8::TYPE))(&equal<flags8>)),
//
		class_<flags16>		("flags16")
			.def(			constructor<>())
			.def("get",		&flags16::get)
			.def("zero",	&flags16::zero)
			.def("one",		&one<flags16>)
			.def("invert",	(flags16 & (flags16::*)())(&flags16::invert))
			.def("invert",	(flags16 & (flags16::*)(const flags16&))(&flags16::invert))
			.def("invert",	(flags16 & (flags16::*)(const flags16::TYPE))(&flags16::invert))
			.def("assign",	(flags16 & (flags16::*)(const flags16&))(&flags16::assign))
			.def("assign",	(flags16 & (flags16::*)(const flags16::TYPE))(&flags16::assign))
			.def("or",		(flags16 & (flags16::*)(const flags16::TYPE))(&flags16::or))
			.def("or",		(flags16 & (flags16::*)(const flags16&,const flags16::TYPE))(&flags16::or))
			.def("and",		(flags16 & (flags16::*)(const flags16::TYPE))(&flags16::and))
			.def("and",		(flags16 & (flags16::*)(const flags16&,const flags16::TYPE))(&flags16::and))
			.def("set",		&set<flags16>)
			.def("is",		&is<flags16>)
			.def("is_any",	&is_any<flags16>)
			.def("test",	&test<flags16>)
			.def("equal",	(bool (*)(flags16*,const flags16&))(&equal<flags16>))
			.def("equal",	(bool (*)(flags16*,const flags16&,const flags16::TYPE))(&equal<flags16>)),

		class_<flags32>		("flags32")
			.def(			constructor<>())
			.def("get",		&flags32::get)
			.def("zero",	&flags32::zero)
			.def("one",		&flags32::one)
			.def("invert",	(flags32 & (flags32::*)())(&flags32::invert))
			.def("invert",	(flags32 & (flags32::*)(const flags32&))(&flags32::invert))
			.def("invert",	(flags32 & (flags32::*)(const flags32::TYPE))(&flags32::invert))
			.def("assign",	(flags32 & (flags32::*)(const flags32&))(&flags32::assign))
			.def("assign",	(flags32 & (flags32::*)(const flags32::TYPE))(&flags32::assign))
			.def("or",		(flags32 & (flags32::*)(const flags32::TYPE))(&flags32::or))
			.def("or",		(flags32 & (flags32::*)(const flags32&,const flags32::TYPE))(&flags32::or))
			.def("and",		(flags32 & (flags32::*)(const flags32::TYPE))(&flags32::and))
			.def("and",		(flags32 & (flags32::*)(const flags32&,const flags32::TYPE))(&flags32::and))
			.def("set",		&set<flags32>)
			.def("is",		&is<flags32>)
			.def("is_any",	&is_any<flags32>)
			.def("test",	&test<flags32>)
			.def("equal",	(bool (*)(flags32*,const flags32&))(&equal<flags32>))
			.def("equal",	(bool (*)(flags32*,const flags32&,const flags32::TYPE))(&equal<flags32>))
	];
}