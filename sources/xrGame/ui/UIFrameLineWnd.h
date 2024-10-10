#pragma once

#include "UIFrameLine.h"
#include "UIWindow.h"
#include "UIStatic.h"

class CUIFrameLineWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
	CUIFrameLineWnd();
	virtual void Init(f32 x, f32 y, f32 width, f32 height);
	virtual void Init(pcstr base_name, f32 x, f32 y, f32 width, f32 height, bool horizontal = true);
	virtual void InitTexture(pcstr tex_name, bool horizontal = true);
	virtual void Draw();
	virtual void SetWidth(f32 width);
	virtual void SetHeight(f32 height);
	virtual void SetOrientation(bool horizontal);
	f32 GetTextureHeight();
	void SetColor(u32 cl);

	// Also we can display textual caption on the frame
	CUIStatic		UITitleText;
	CUIStatic*		GetTitleStatic(){return &UITitleText;};

protected:
	bool			bHorizontal;
	bool			m_bTextureAvailable;
	CUIFrameLine	UIFrameLine;	
};
