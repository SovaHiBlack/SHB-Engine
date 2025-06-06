#pragma once

#include "UIWindow.h"

class CUIStatic;
struct SGameNewsData;

class CUINewsItemWnd : public CUIWindow
{
	typedef	CUIWindow		inherited;
	CUIStatic* m_UIText;
	CUIStatic* m_UITextDate;
	CUIStatic* m_UIImage;

public:
	CUINewsItemWnd( );
	virtual			~CUINewsItemWnd( );
	void	Init(pcstr xml_name, pcstr start_from);
	void	Setup(SGameNewsData& news_data);
};
