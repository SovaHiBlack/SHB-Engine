#pragma once

#include "ui/UIStatic.h"//

class CMapLocation;
class CUIXml;

class CMapSpot : public CUIStatic
{
	using inherited									= CUIStatic;
	CMapLocation*									m_map_location;

public:
	bool											m_bScale;
	Fvector2										m_originSize;

							CMapSpot				(CMapLocation*);
	virtual					~CMapSpot				( );
	virtual void			Load					(CUIXml* xml, const char* path);
	CMapLocation*			MapLocation				( )
	{
		return m_map_location;
	}
	virtual const char*		GetHint					( );
	virtual void			Update					( );
	virtual bool			OnMouseDown				(int mouse_btn);
	virtual void			OnFocusLost				( );
};

class CMapSpotPointer : public CMapSpot
{
	using inherited									= CMapSpot;
	xr_string										m_pointer_hint;

public:
							CMapSpotPointer			(CMapLocation*);
	virtual					~CMapSpotPointer		( );
	virtual const char*		GetHint					( );
};

class CMiniMapSpot : public CMapSpot
{
	using inherited									= CMapSpot;
	ref_shader										m_icon_above;
	ref_shader										m_icon_normal;
	ref_shader										m_icon_below;
	Frect											m_tex_rect_above;
	Frect											m_tex_rect_normal;
	Frect											m_tex_rect_below;

public:
							CMiniMapSpot			(CMapLocation*);
	virtual					~CMiniMapSpot			( );
	virtual void			Load					(CUIXml* xml, const char* path);
	virtual void			Draw					( );
};
