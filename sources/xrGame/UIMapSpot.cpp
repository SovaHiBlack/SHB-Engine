#include "stdafx.h"
#include "UIMapSpot.h"
#include "map_location.h"

#include "ui/UIXmlInit.h"
#include "ui/UIMapWnd.h"//
#include "level.h"
#include "..\XR_3DA\Object.h"
#include "object_broker.h"
#include "ui/UITextureMaster.h"

CUIMapSpot::CUIMapSpot(CMapLocation* ml) : m_map_location(ml)
{
	ClipperOn( );
	m_bScale = false;
}

CUIMapSpot::~CUIMapSpot( )
{ }

void CUIMapSpot::Load(CUIXml* xml, pcstr path)
{
	CUIXmlInit::InitStatic(*xml, path, 0, this);
	s32 i = xml->ReadAttribInt(path, 0, "scale", 0);
	m_bScale = (i == 1);

	m_originSize = GetWndSize( );
}

pcstr CUIMapSpot::GetHint( )
{
	return MapLocation( )->GetHint( );
}

void CUIMapSpot::Update( )
{
	inherited::Update( );
	if (m_bCursorOverWindow)
	{
		VERIFY(m_dwFocusReceiveTime >= 0);
		if (Device.dwTimeGlobal > (m_dwFocusReceiveTime + 500))
		{
			GetMessageTarget( )->SendMessage(this, MAP_SHOW_HINT, NULL);
		}
	}
}

bool CUIMapSpot::OnMouseDown(s32 mouse_btn)
{
	return false;
}

void CUIMapSpot::OnFocusLost( )
{
	inherited::OnFocusLost( );
	GetMessageTarget( )->SendMessage(this, MAP_HIDE_HINT, NULL);
}

CUIMapSpotPointer::CUIMapSpotPointer(CMapLocation* ml) :inherited(ml)
{
	ClipperOn( );
}

CUIMapSpotPointer::~CUIMapSpotPointer( )
{ }

pcstr CUIMapSpotPointer::GetHint( )
{
	return NULL;
}

CUIMiniMapSpot::CUIMiniMapSpot(CMapLocation* ml) : inherited(ml)
{ }

CUIMiniMapSpot::~CUIMiniMapSpot( )
{ }

void CUIMiniMapSpot::Load(CUIXml* xml, pcstr path)
{
	inherited::Load(xml, path);

	string256 buf;
	XML_NODE* n = NULL;

	fRect base_rect;
	base_rect.x1 = 0;
	base_rect.y1 = 0;
	base_rect.x2 = xml->ReadAttribFlt(path, 0, "width", 0);
	base_rect.y2 = xml->ReadAttribFlt(path, 0, "height", 0);

	fRect _stored_rect = m_UIStaticItem.GetOriginalRect( );

	strconcat(sizeof(buf), buf, path, ":texture_above");
	n = xml->NavigateToNode(buf, 0);
	if (n)
	{
		pcstr texture = xml->Read(buf, 0, NULL);
		CUITextureMaster::InitTexture(texture, "hud\\default", &m_UIStaticItem);
		if (strchr(texture, '\\'))
		{
			f32 x = xml->ReadAttribFlt(buf, 0, "x", base_rect.x1);
			f32 y = xml->ReadAttribFlt(buf, 0, "y", base_rect.y1);
			f32 width = xml->ReadAttribFlt(buf, 0, "width", base_rect.width( ));
			f32 height = xml->ReadAttribFlt(buf, 0, "height", base_rect.height( ));
			m_tex_rect_above.set(x, y, x + width, y + height);
		}
		else
		{
			m_tex_rect_above = m_UIStaticItem.GetOriginalRect( );
		}

		m_icon_above = m_UIStaticItem.GetShader( );
	}

	strconcat(sizeof(buf), buf, path, ":texture_below");
	n = xml->NavigateToNode(buf, 0);
	if (n)
	{
		pcstr texture = xml->Read(buf, 0, NULL);
		CUITextureMaster::InitTexture(texture, "hud\\default", &m_UIStaticItem);
		if (strchr(texture, '\\'))
		{
			f32 x = xml->ReadAttribFlt(buf, 0, "x", base_rect.x1);
			f32 y = xml->ReadAttribFlt(buf, 0, "y", base_rect.y1);
			f32 width = xml->ReadAttribFlt(buf, 0, "width", base_rect.width( ));
			f32 height = xml->ReadAttribFlt(buf, 0, "height", base_rect.height( ));
			m_tex_rect_below.set(x, y, x + width, y + height);
		}
		else
		{
			m_tex_rect_below = m_UIStaticItem.GetOriginalRect( );
		}

		m_icon_below = m_UIStaticItem.GetShader( );
	}

	strconcat(sizeof(buf), buf, path, ":texture");
	n = xml->NavigateToNode(buf, 0);
	if (n)
	{
		pcstr texture = xml->Read(buf, 0, NULL);
		CUITextureMaster::InitTexture(texture, "hud\\default", &m_UIStaticItem);
		if (strchr(texture, '\\'))
		{
			f32 x = xml->ReadAttribFlt(buf, 0, "x", base_rect.x1);
			f32 y = xml->ReadAttribFlt(buf, 0, "y", base_rect.y1);
			f32 width = xml->ReadAttribFlt(buf, 0, "width", base_rect.width( ));
			f32 height = xml->ReadAttribFlt(buf, 0, "height", base_rect.height( ));
			m_tex_rect_normal.set(x, y, x + width, y + height);
		}
		else
		{
			m_tex_rect_normal = m_UIStaticItem.GetOriginalRect( );
		}

		m_icon_normal = m_UIStaticItem.GetShader( );
	}

	m_UIStaticItem.SetOriginalRect(_stored_rect);
}

void CUIMiniMapSpot::Draw( )
{
	CObject* O = Level( ).CurrentViewEntity( );
	if (O && m_icon_above && m_icon_below)
	{
		f32 ml_y = MapLocation( )->GetLastPosition( ).y;
		f32 d = O->Position( ).y - ml_y;

		if (d > 1.8f)
		{
			GetUIStaticItem( ).SetShader(m_icon_below);
			GetUIStaticItem( ).SetOriginalRect(m_tex_rect_below.x1, m_tex_rect_below.y1, m_tex_rect_below.width( ), m_tex_rect_below.height( ));
		}
		else if (d < -1.8f)
		{
			GetUIStaticItem( ).SetShader(m_icon_above);
			GetUIStaticItem( ).SetOriginalRect(m_tex_rect_above.x1, m_tex_rect_above.y1, m_tex_rect_above.width( ), m_tex_rect_above.height( ));
		}
		else
		{
			GetUIStaticItem( ).SetShader(m_icon_normal);
			GetUIStaticItem( ).SetOriginalRect(m_tex_rect_normal.x1, m_tex_rect_normal.y1, m_tex_rect_normal.width( ), m_tex_rect_normal.height( ));
		}
	}

	inherited::Draw( );
}
