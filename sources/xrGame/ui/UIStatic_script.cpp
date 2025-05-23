#include "stdafx.h"

#include "UIStatic.h"

using namespace luabind;

#pragma optimize("s",on)
void CUIStatic::script_register(lua_State* L)
{
	module(L)
		[
			class_<CUIStatic, CUIWindow>("CUIStatic")
				.def(constructor<>( ))

				.def("SetText", (void (CUIStatic::*)(pcstr)) (&CUIStatic::SetText))
				.def("SetTextST", (void (CUIStatic::*)(pcstr)) (&CUIStatic::SetTextST))
				.def("GetText", &CUIStatic::GetText)

				.def("SetTextX", &CUIStatic::SetTextX)
				.def("SetTextY", &CUIStatic::SetTextY)
				.def("GetTextX", &CUIStatic::GetTextX)
				.def("GetTextY", &CUIStatic::GetTextY)

				.def("SetColor", &CUIStatic::SetColor)
				.def("GetColor", &CUIStatic::GetColor)
				.def("SetTextColor", &CUIStatic::SetTextColor_script)
				.def("Init", (void(CUIStatic::*)(f32, f32, f32, f32)) & CUIStatic::Init)
				.def("Init", (void(CUIStatic::*)(pcstr, f32, f32, f32, f32)) & CUIStatic::Init)
				.def("InitTexture", &CUIStatic::InitTexture)
				.def("SetTextureOffset", &CUIStatic::SetTextureOffset)

				.def("SetOriginalRect", (void(CUIStatic::*)(f32, f32, f32, f32)) & CUIStatic::SetOriginalRect)
				.def("SetStretchTexture", &CUIStatic::SetStretchTexture)
				.def("GetStretchTexture", &CUIStatic::GetStretchTexture)

				.def("SetTextAlign", &CUIStatic::SetTextAlign_script)
				.def("GetTextAlign", &CUIStatic::GetTextAlign_script)

				.def("SetHeading", &CUIStatic::SetHeading)
				.def("GetHeading", &CUIStatic::GetHeading)

				.def("ClipperOn", &CUIStatic::ClipperOn)
				.def("ClipperOff", (void(CUIStatic::*)(void)) & CUIStatic::ClipperOff)
				.def("GetClipperState", &CUIStatic::GetClipperState)
		];
}
