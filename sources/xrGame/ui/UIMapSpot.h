#pragma once
#include "ui/UIStatic.h"

class CMapLocation;
class CUIXml;

class CUIMapSpot :public CUIStatic
{
	typedef CUIStatic inherited;
	CMapLocation* m_map_location;

public:
	bool					m_bScale;
	fVector2				m_originSize;
	CUIMapSpot(CMapLocation*);
	virtual					~CUIMapSpot( );
	virtual void		Load(CUIXml* xml, pcstr path);
	CMapLocation* MapLocation( )
	{
		return m_map_location;
	}
	virtual pcstr			GetHint( );
	virtual void		Update( );
	virtual bool		OnMouseDown(s32 mouse_btn);
	virtual void		OnFocusLost( );
};

class CUIMapSpotPointer : public CUIMapSpot
{
	typedef CUIMapSpot inherited;
	xr_string				m_pointer_hint;

public:
	CUIMapSpotPointer(CMapLocation*);
	virtual					~CUIMapSpotPointer( );
	virtual pcstr		GetHint( );
};

class CUIMiniMapSpot : public CUIMapSpot
{
	typedef CUIMapSpot inherited;
	ref_shader				m_icon_above, m_icon_normal, m_icon_below;
	fRect					m_tex_rect_above;
	fRect					m_tex_rect_normal;
	fRect					m_tex_rect_below;

public:
	CUIMiniMapSpot(CMapLocation*);
	virtual				~CUIMiniMapSpot( );
	virtual void		Load(CUIXml* xml, pcstr path);
	virtual void		Draw( );
};
