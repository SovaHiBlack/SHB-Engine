#pragma once

#include "UIWindow.h"//

class CUIStatic;
struct GAME_NEWS_DATA;

class CUINewsItemWnd :public CUIWindow
{
typedef	CUIWindow		inherited;
CUIStatic*				m_UIText;
CUIStatic*				m_UITextDate;
CUIStatic*				m_UIImage;

public:
					CUINewsItemWnd					();
	virtual			~CUINewsItemWnd					();
			void	Init							(const char* xml_name, const char* start_from);
			void	Setup							(GAME_NEWS_DATA& news_data);
};