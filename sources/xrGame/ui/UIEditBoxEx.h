#pragma once

#include "UICustomEdit.h"

class CUIFrameWindow;

class CUIEditBoxEx : public CUIMultiTextureOwner, public CUICustomEdit {
public:
	CUIEditBoxEx();
	virtual ~CUIEditBoxEx();

	// CUIWindow
	virtual void	Init(f32 x, f32 y, f32 width, f32 heigt);

	// CUIMultiTextureOwner
	virtual void	InitTexture(pcstr texture);

protected:
	CUIFrameWindow*	m_pFrameWindow;
};
