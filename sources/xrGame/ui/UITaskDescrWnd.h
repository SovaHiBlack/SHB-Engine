#pragma once
#include "UIWindow.h"

class CUIScrollView;
class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIXml;
class CEncyclopediaArticle;

class CUITaskDescrWnd : public CUIWindow
{
	typedef CUIWindow			inherited;

protected:
	CUIScrollView* m_UITaskInfoWnd;
	CUIFrameWindow* m_UIMainFrame;
	CUIFrameLineWnd* m_UIMainHeader;

public:
	CUITaskDescrWnd( );
	virtual		~CUITaskDescrWnd( );
	virtual void Draw( );
	void		Init(CUIXml* doc, pcstr start_from);
	void		ClearAll( );
	void		AddArticle(pcstr article);
	void		AddArticle(CEncyclopediaArticle* article);
};
