#pragma once

#include "uistaticitem.h"
#include "ui/uiabstract.h"

class CUIFrameRect: public CUISimpleWindow, CUIMultiTextureOwner //public CUICustomItem
{
public:
	enum EFramePart{
		fmBK=0,
		fmL, fmR, fmT, fmB, fmLT, fmRB, fmRT, fmLB, fmMax
	};
	Flags16		m_itm_mask;

	friend class CUIFrameWindow;
	using CUISimpleWindow::Init;

						CUIFrameRect	();
	virtual void		Init			(pcstr base_name, f32 x, f32 y, f32 w, f32 h);//, DWORD align);
	virtual void		InitTexture		(pcstr texture);
	virtual void		Draw			();
	virtual void		Draw			(f32 x, f32 y);
	virtual void		SetWndPos		(f32 x, f32 y);
	virtual void		SetWndPos		(const Fvector2& pos);
	virtual void		SetWndSize		(const Fvector2& size);
	virtual void		SetWndRect		(const Frect& rect);
	virtual void		SetWidth		(f32 width);
	virtual void		SetHeight		(f32 height);
	virtual void		Update			();
			void		SetTextureColor	(u32 cl);
			void		SetVisiblePart	(EFramePart p, BOOL b)	{m_itm_mask.set(u16(1<<p), b);};

protected:
	CUIStaticItem	frame[fmMax];

	enum {
		flValidSize	= (1<<0),
		flSingleTex	= (1<<1),
	};
	Flags8			uFlags;
	void			UpdateSize		();
};
