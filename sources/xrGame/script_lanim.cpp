////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file.cpp
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_lanim.h"
#include "..\XR_3DA\LightAnimLibrary.h"

using namespace luabind;

struct lanim_wrapper
{
	CLightAnimItem* item;

public:
			lanim_wrapper	(pcstr name){load(name);}
	void	load			(pcstr name)
	{
		item				= LALib.FindItem(name);
		R_ASSERT3			(item,"Can't find color anim:",name);
	}
	u32		length			()
	{
		VERIFY				(item);
		return item->Length_ms();
	}
	fColor	calculate		(f32 T)
	{
		s32 frame;
		VERIFY				(item);
		return fColor().set(item->CalculateRGB(T,frame));
	}
};

#pragma optimize("s",on)
void lanim_registrator::script_register(lua_State *L)
{
	module(L)
		[
			class_<lanim_wrapper>("color_animator")
			.def(					constructor<pcstr>())
			.def("load",			&lanim_wrapper::load)
			.def("calculate",		&lanim_wrapper::calculate)
			.def("length",			&lanim_wrapper::length)
		];
}