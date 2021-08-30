#pragma once

#include "UIWindow.h"//

class CUIScrollView;
struct SGameNewsData;

class CUINewsWnd : public CUIWindow
{
	using inherited						= CUIWindow;

	enum eFlag
	{
		eNeedAdd						= (1 << 0)
	};

	Flags16								m_flags;

public:
						CUINewsWnd		( );
	virtual				~CUINewsWnd		( );

	void				Init			( );
	void				Init			(const char* xml_name, const char* start_from);
	void				AddNews			( );
	virtual void		Show			(bool status);
	virtual void		Update			( );

	CUIScrollView*						UIScrollWnd;

private:
	void				LoadNews		( );
	void				AddNewsItem		(SGameNewsData& news_data);
};
