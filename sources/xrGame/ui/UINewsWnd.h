#pragma once

#include "UIWindow.h"

class CUIScrollView;
struct SGameNewsData;

class CUINewsWnd : public CUIWindow
{
	typedef CUIWindow inherited;
	enum eFlag
	{
		eNeedAdd = (1 << 0)
	};
	flags16			m_flags;

public:
	CUINewsWnd( );
	virtual			~CUINewsWnd( );

	void	Init( );
	void	Init(pcstr xml_name, pcstr start_from);
	void			AddNews( );
	virtual void	Show(bool status);
	virtual void	Update( );

	CUIScrollView* UIScrollWnd;

private:
	void			LoadNews( );
	void			AddNewsItem(SGameNewsData& news_data);
};
