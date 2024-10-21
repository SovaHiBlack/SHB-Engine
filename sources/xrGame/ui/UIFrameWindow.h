#pragma once

#include "uiwindow.h"

#include "../uiframerect.h"

class CUIStatic;


class CUIFrameWindow: public CUIWindow,
					  public CUIMultiTextureOwner
{
private:
	typedef CUIWindow inherited;
public:
	using CUIWindow::Draw;
					CUIFrameWindow				();

	virtual void	Init						(pcstr base_name, f32 x, f32 y, f32 width, f32 height);
	virtual void	Init						(f32 x, f32 y, f32 width, f32 height);
	virtual void	Init						(pcstr base_name, fRect* pRect);

	virtual void	InitTexture					(pcstr texture);
			void	SetTextureColor				(u32 color)										{m_UIWndFrame.SetTextureColor(color);}

	virtual void	SetWidth					(f32 width);
	virtual void	SetHeight					(f32 height);
	
			void	SetColor					(u32 cl);

	virtual void	Draw						();
	virtual void	Update						();
	
	//текст заголовка
	CUIStatic*		UITitleText;
	CUIStatic*		GetTitleStatic				()										{return UITitleText;};
	void			SetVisiblePart				(CUIFrameRect::EFramePart p, BOOL b)	{m_UIWndFrame.SetVisiblePart(p,b);};

protected:
	CUIFrameRect	m_UIWndFrame;

	void			FrameClip					(const fRect parentAbsR);
	
private:
	inline void		ClampMax_Zero				(fRect& r);
};
