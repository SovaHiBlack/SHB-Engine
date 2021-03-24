#pragma once

#include "uiabstract.h"
#include "UICustomItem.h"//

void						CreateUIGeom			( );
ref_geom					GetUIGeom				( );
void						DestroyUIGeom			( );

class CUIStaticItem : public IUISimpleTextureControl, public CUICustomItem
{
	ref_shader										hShader;
//	ref_geom										hGeom_fan;
	Fvector2										iPos;
	u32												dwColor;
	int												iTileX;
	int												iTileY;
	float											iRemX;
	float											iRemY;
	int												alpha_ref;

protected:
	using inherited									= CUICustomItem;

public:
	using CUICustomItem::SetOriginalRect;

#ifdef DEBUG
	shared_str										dbg_tex_name;
#endif // def DEBUG

							CUIStaticItem			( );
	virtual					~CUIStaticItem			( );

	void					SetAlphaRef				(int val)
	{
		alpha_ref = val;
	}
	virtual void			CreateShader			(const char* tex, const char* sh = "hud\\default");
	virtual void			SetShader				(const ref_shader& sh);
	virtual void			SetTextureColor			(u32 color)
	{
		SetColor(color);
	}
	virtual u32				GetTextureColor			( ) const
	{
		return GetColor( );
	}
	virtual void			SetOriginalRect			(const Frect& r)
	{
		iOriginalRect = r;
		uFlags |= flValidOriginalRect;
	}
	virtual void			SetOriginalRectEx		(const Frect& r)
	{
		iOriginalRect = r;
		uFlags |= flValidOriginalRect;
		SetRect(0, 0, r.width( ), r.height( ));
	}

	void					Init					(const char* tex, const char* sh, float left, float top, u32 align);

	void					Render					( );
	void					Render					(float angle);

	inline void				SetTile					(int tile_x, int tile_y, float rem_x, float rem_y)
	{
		iTileX = tile_x; iTileY = tile_y; iRemX = rem_x; iRemY = rem_y;
	}
	inline void				SetPos					(float left, float top)
	{
		iPos.set(left, top);
	}
	inline void				SetPosX					(float left)
	{
		iPos.x = left;
	}
	inline void				SetPosY					(float top)
	{
		iPos.y = top;
	}

	inline float			GetPosX					( )
	{
		return iPos.x;
	}
	inline float			GetPosY					( )
	{
		return iPos.y;
	}

	inline void				SetColor				(u32 clr)
	{
		dwColor = clr;
	}
	inline void				SetColor				(Fcolor clr)
	{
		dwColor = clr.get( );
	}
	inline u32				GetColor				( ) const
	{
		return dwColor;
	}
	inline u32&				GetColorRef				( )
	{
		return dwColor;
	}
	inline ref_shader&		GetShader				( )
	{
		return hShader;
	}
};

extern ENGINE_API BOOL								g_bRendering;
