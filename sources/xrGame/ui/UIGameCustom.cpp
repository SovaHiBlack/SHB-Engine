#include "stdafx.h"

#include "UIGameCustom.h"
#include "ui.h"
#include "level.h"
#include "hudmanager.h"
#include "ui/UIMultiTextStatic.h"
#include "ui/UIXmlInit.h"
#include "object_broker.h"
#include "StringTable.h"
#include "ui/UIGameTutorial.h"
#include "ui/uistatic.h"

struct predicate_remove_stat
{
	bool	operator() (SDrawStaticStruct& s)
	{
		return (!s.IsActual( ));
	}
};

CUIGameCustom::CUIGameCustom( )
{
	uFlags = 0;
	shedule.t_min = 5;
	shedule.t_max = 20;
	shedule_register( );
	m_pgameCaptions = xr_new<CUICaption>( );
	m_msgs_xml = xr_new<CUIXml>( );
	m_msgs_xml->Init(CONFIG_PATH, UI_PATH, "ui_custom_msgs.xml");
}

CUIGameCustom::~CUIGameCustom( )
{
	delete_data(m_pgameCaptions);
	shedule_unregister( );
	delete_data(m_custom_statics);
	delete_data(m_msgs_xml);
}

f32 CUIGameCustom::shedule_Scale( )
{
	return 0.5f;
}

void CUIGameCustom::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}

bool g_b_ClearGameCaptions = false;

void CUIGameCustom::OnFrame( )
{
	st_vec::iterator it = m_custom_statics.begin( );
	for (; it != m_custom_statics.end( ); ++it)
	{
		(*it).Update( );
	}

	m_custom_statics.erase(
		std::remove_if(
			m_custom_statics.begin( ),
			m_custom_statics.end( ),
			predicate_remove_stat( )
		),
		m_custom_statics.end( )
	);

	if (g_b_ClearGameCaptions)
	{
		delete_data(m_custom_statics);
		g_b_ClearGameCaptions = false;
	}
}

void CUIGameCustom::Render( )
{
	GameCaptions( )->Draw( );
	st_vec::iterator it = m_custom_statics.begin( );
	for (; it != m_custom_statics.end( ); ++it)
	{
		(*it).Draw( );
	}
}

bool CUIGameCustom::IR_OnKeyboardPress(s32 dik)
{
	return false;
}

bool CUIGameCustom::IR_OnKeyboardRelease(s32 dik)
{
	return false;
}

bool CUIGameCustom::IR_OnMouseMove(s32 dx, s32 dy)
{
	return false;
}

bool CUIGameCustom::IR_OnMouseWheel(s32 direction)
{
	return false;
}

void CUIGameCustom::AddDialogToRender(CUIWindow* pDialog)
{
	HUD( ).GetUI( )->AddDialogToRender(pDialog);
}

void CUIGameCustom::RemoveDialogToRender(CUIWindow* pDialog)
{
	HUD( ).GetUI( )->RemoveDialogToRender(pDialog);
}

CUIDialogWnd* CUIGameCustom::MainInputReceiver( )
{
	return HUD( ).GetUI( )->MainInputReceiver( );
}

void CUIGameCustom::AddCustomMessage(pcstr id, f32 x, f32 y, f32 font_size, CGameFont* pFont, u16 alignment, u32 color)
{
	GameCaptions( )->addCustomMessage(id, x, y, font_size, pFont, (CGameFont::EAligment)alignment, color, "");
}

void CUIGameCustom::AddCustomMessage(pcstr id, f32 x, f32 y, f32 font_size, CGameFont* pFont, u16 alignment, u32 color, f32 flicker)
{
	AddCustomMessage(id, x, y, font_size, pFont, alignment, color);
	GameCaptions( )->customizeMessage(id, CUITextBanner::tbsFlicker)->fPeriod = flicker;
}

void CUIGameCustom::CustomMessageOut(pcstr id, pcstr msg, u32 color)
{
	GameCaptions( )->setCaption(id, msg, color, true);
}

void CUIGameCustom::RemoveCustomMessage(pcstr id)
{
	GameCaptions( )->removeCustomMessage(id);
}

SDrawStaticStruct* CUIGameCustom::AddCustomStatic(pcstr id, bool bSingleInstance)
{
	if (bSingleInstance)
	{
		st_vec::iterator it = std::find(m_custom_statics.begin( ), m_custom_statics.end( ), id);
		if (it != m_custom_statics.end( ))
		{
			return &(*it);
		}
	}

	CUIXmlInit xml_init;
	m_custom_statics.push_back(SDrawStaticStruct( ));
	SDrawStaticStruct& sss = m_custom_statics.back( );

	sss.m_static = xr_new<CUIStatic>( );
	sss.m_name = id;
	xml_init.InitStatic(*m_msgs_xml, id, 0, sss.m_static);
	f32 ttl = m_msgs_xml->ReadAttribFlt(id, 0, "ttl", -1);
	if (ttl > 0.0f)
	{
		sss.m_endTime = Device.fTimeGlobal + ttl;
	}

	return &sss;
}

SDrawStaticStruct* CUIGameCustom::GetCustomStatic(pcstr id)
{
	st_vec::iterator it = std::find(m_custom_statics.begin( ), m_custom_statics.end( ), id);
	if (it != m_custom_statics.end( ))
	{
		return &(*it);
	}

	return NULL;
}

void CUIGameCustom::RemoveCustomStatic(pcstr id)
{
	st_vec::iterator it = std::find(m_custom_statics.begin( ), m_custom_statics.end( ), id);
	if (it != m_custom_statics.end( ))
	{
		xr_delete((*it).m_static);
		m_custom_statics.erase(it);
	}
}

extern CUISequencer* g_tutorial;
extern CUISequencer* g_tutorial2;

void CUIGameCustom::reset_ui( )
{
	if (g_tutorial2)
	{
		g_tutorial2->Destroy( );
		xr_delete(g_tutorial2);
	}

	if (g_tutorial)
	{
		g_tutorial->Destroy( );
		xr_delete(g_tutorial);
	}
}

SDrawStaticStruct::SDrawStaticStruct( )
{
	m_static = NULL;
	m_endTime = -1.0f;
}

void SDrawStaticStruct::destroy( )
{
	delete_data(m_static);
}

bool SDrawStaticStruct::IsActual( )
{
	if (m_endTime < 0.0f)
	{
		return true;
	}

	return Device.fTimeGlobal < m_endTime;
}

void SDrawStaticStruct::Draw( )
{
	if (m_static)
	{
		m_static->Draw( );
	}
}

void SDrawStaticStruct::Update( )
{
	if (!IsActual( ))
	{
		delete_data(m_static);
	}
	else
	{
		m_static->Update( );
	}
}

using namespace luabind;

CUIGameCustom* get_hud( )
{
	return HUD( ).GetUI( )->UIGame( );
}

#pragma optimize("s",on)
void CUIGameCustom::script_register(lua_State* L)
{
	module(L)
		[
			class_< SDrawStaticStruct >("SDrawStaticStruct")
				.def_readwrite("m_endTime", &SDrawStaticStruct::m_endTime)
				.def("wnd", &SDrawStaticStruct::wnd),

				class_< CUIGameCustom >("CUIGameCustom")
				.def("AddDialogToRender", &CUIGameCustom::AddDialogToRender)
				.def("RemoveDialogToRender", &CUIGameCustom::RemoveDialogToRender)
				.def("AddCustomMessage", (void(CUIGameCustom::*)(pcstr, f32, f32, f32, CGameFont*, u16, u32/*, pcstr*/)) & CUIGameCustom::AddCustomMessage)
				.def("AddCustomMessage", (void(CUIGameCustom::*)(pcstr, f32, f32, f32, CGameFont*, u16, u32/*, pcstr*/, f32)) & CUIGameCustom::AddCustomMessage)
				.def("CustomMessageOut", &CUIGameCustom::CustomMessageOut)
				.def("RemoveCustomMessage", &CUIGameCustom::RemoveCustomMessage)
				.def("AddCustomStatic", &CUIGameCustom::AddCustomStatic)
				.def("RemoveCustomStatic", &CUIGameCustom::RemoveCustomStatic)
				.def("GetCustomStatic", &CUIGameCustom::GetCustomStatic),
				def("get_hud", &get_hud)
		];
}
