#pragma once

#include "UIWindow.h"//
#include "UIFrameRect.h"//

class CUIStatic;

class CUIFrameWindow : public CUIWindow, public CUIMultiTextureOwner
{
private:
	using inherited						= CUIWindow;

public:
	using CUIWindow::Draw;
					CUIFrameWindow		( );

	virtual void	Init				(const char* base_name, float x, float y, float width, float height);
	virtual void	Init				(float x, float y, float width, float height);
	virtual void	Init				(const char* base_name, Frect* pRect);

	virtual void	InitTexture			(const char* texture);
	void			SetTextureColor		(unsigned int color)
	{
		m_UIWndFrame.SetTextureColor(color);
	}

	virtual void	SetWidth			(float width);
	virtual void	SetHeight			(float height);

	void			SetColor			(unsigned int cl);

	virtual void	Draw				( );
	virtual void	Update				( );

	//текст заголовка
	CUIStatic*							UITitleText;
	CUIStatic*		GetTitleStatic		( )
	{
		return UITitleText;
	}
	void			SetVisiblePart		(CUIFrameRect::EFramePart p, BOOL b)
	{
		m_UIWndFrame.SetVisiblePart(p, b);
	}

protected:
	CUIFrameRect						m_UIWndFrame;

	void			FrameClip			(const Frect parentAbsR);

private:
	inline void		ClampMax_Zero		(Frect& r);
};
