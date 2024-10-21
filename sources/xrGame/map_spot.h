#pragma once
#include "ui/UIStatic.h"

class CMapLocation;
class CUIXml;

class CMapSpot :public CUIStatic
{
	typedef CUIStatic inherited;
	CMapLocation* m_map_location;

public:
	bool					m_bScale;
	fVector2				m_originSize;
	CMapSpot(CMapLocation*);
	virtual					~CMapSpot( );
	virtual		void		Load(CUIXml* xml, pcstr path);
	CMapLocation* MapLocation( )
	{
		return m_map_location;
	}
	virtual pcstr			GetHint( );
	virtual		void		Update( );
	virtual		bool		OnMouseDown(int mouse_btn);
	virtual		void		OnFocusLost( );
};

class CMapSpotPointer :public CMapSpot
{
	typedef CMapSpot inherited;
	xr_string				m_pointer_hint;

public:
	CMapSpotPointer(CMapLocation*);
	virtual					~CMapSpotPointer( );
	virtual		pcstr		GetHint( );
};

class CMiniMapSpot :public CMapSpot
{
	typedef CMapSpot inherited;
	ref_shader				m_icon_above, m_icon_normal, m_icon_below;
	fRect					m_tex_rect_above;
	fRect					m_tex_rect_normal;
	fRect					m_tex_rect_below;

public:
	CMiniMapSpot(CMapLocation*);
	virtual					~CMiniMapSpot( );
	virtual		void		Load(CUIXml* xml, pcstr path);
	virtual		void		Draw( );
};
