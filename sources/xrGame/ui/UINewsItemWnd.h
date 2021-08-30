#pragma once

#include "UIWindow.h"//

class CUIStatic;
struct SGameNewsData;

class CUINewsItemWnd : public CUIWindow
{
	using inherited						= CUIWindow;

	CUIStatic*							m_UIText;
	CUIStatic*							m_UITextDate;
	CUIStatic*							m_UIImage;

public:
					CUINewsItemWnd		( );
	virtual			~CUINewsItemWnd		( );
	void			Init				(const char* xml_name, const char* start_from);
	void			Setup				(SGameNewsData& news_data);
};
