#pragma once

#include "UIFrameLineWnd.h"
#include "UILines.h"

class CLightAnimItem;

class CUILabel : public CUIFrameLineWnd, public CUILinesOwner
{
	CLightAnimItem* m_lanim;
	f32					m_lainm_start_time;

public:
	// IUISimpleWindow
	virtual void SetWidth(f32 width);
	virtual void SetHeight(f32 height);
	virtual void SetFont(CGameFont* pFont);
	// CUIFrameLineWnd
	virtual void			Init(f32 x, f32 y, f32 width, f32 height);
	virtual void			Draw( );
	virtual void			Update( );

	// own
	CUILabel( );
	void			SetLightAnim(pcstr lanim);
};