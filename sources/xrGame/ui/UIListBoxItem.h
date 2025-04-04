#pragma once
#include "UILabel.h"

class CUIListBoxItem : public CUILabel, public CUISelectable
{
public:
	using CUILabel::SetTextColor;

	CUIListBoxItem( );
	virtual				~CUIListBoxItem( );

	virtual void		SetSelected(bool b);
	virtual void		Draw( );
	virtual bool		OnMouseDown(s32 mouse_btn);
	virtual bool		OnDbClick( );
	virtual void		OnFocusReceive( );
	virtual CGameFont* GetFont( );
	void		SetTextColor(u32 color, u32 color_s);
	void		InitDefault( );
	void		SetTAG(u32 value);
	u32			GetTAG( );

	void		SetData(pvoid data);
	pvoid GetData( );

	CUIStatic* AddField(pcstr txt, f32 len, pcstr key = "");
	pcstr			GetField(pcstr key);

protected:
	f32		FieldsLength( );
	xr_vector<CUIStatic*>	fields;
	u32				txt_color;
	u32				txt_color_s;
	u32				tag;
	pvoid pData;
};
