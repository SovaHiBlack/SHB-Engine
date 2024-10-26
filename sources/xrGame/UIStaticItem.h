#pragma once

#include "ui/uiabstract.h"
#include "uicustomitem.h"

void		CreateUIGeom		();
ref_geom	GetUIGeom			();
void		DestroyUIGeom		();

class CUIStaticItem: public IUISimpleTextureControl, public CUICustomItem
{
	ref_shader		hShader;
//	ref_geom		hGeom_fan;	
	fVector2		iPos;
	u32				dwColor;
	int				iTileX;
	int				iTileY;
	f32			iRemX;
	f32			iRemY;
	int				alpha_ref;

protected:
	typedef CUICustomItem inherited;

public:
	using CUICustomItem::SetOriginalRect;

#ifdef DEBUG
	shared_str	dbg_tex_name;
#endif
					CUIStaticItem	();
	virtual			~CUIStaticItem	();

			void	SetAlphaRef		(int val)											{alpha_ref=val;};
	virtual void	CreateShader	(pcstr tex, pcstr sh = "hud\\default");
	virtual void	SetShader		(const ref_shader& sh);
	virtual void	SetTextureColor	(u32 color)											{SetColor(color);}
	virtual u32		GetTextureColor	()											const	{return GetColor();}
	virtual	void	SetOriginalRect	(const fRect& r)									{iOriginalRect = r; uFlags|=flValidOriginalRect;}
	virtual void	SetOriginalRectEx(const fRect& r)									{iOriginalRect = r; uFlags|=flValidOriginalRect; SetRect(0,0,r.width(),r.height());}


	void			Init			(pcstr tex, pcstr sh, f32 left, f32 top, u32 align);
	
	
	
	void			Render			();
	void			Render			(f32 angle);

	IC void			SetTile			(int tile_x, int tile_y, f32 rem_x, f32 rem_y){iTileX=tile_x;iTileY=tile_y;iRemX=rem_x;iRemY=rem_y;}
	IC void			SetPos			(f32 left, f32 top)			{iPos.set(left,top);}
	IC void			SetPosX			(f32 left)					{iPos.x = left;}
	IC void			SetPosY			(f32 top)						{iPos.y = top;}

	IC f32		GetPosX			()							{return iPos.x;}
	IC f32		GetPosY			()							{return iPos.y;}

	IC void			SetColor		(u32 clr)					{dwColor= clr;}
	IC void			SetColor		(fColor clr)				{dwColor= clr.get();}
	IC u32			GetColor		() const					{return dwColor;}
	IC u32&			GetColorRef		()							{return dwColor;}
	IC ref_shader&	GetShader		()							{return hShader;}
};

extern ENGINE_API BOOL g_bRendering; 

