#pragma once

#include "UIWindow.h"//

class CUIStatic;
class CEncyclopediaArticle;

class CUIEncyclopediaArticleWnd :public CUIWindow
{
typedef	CUIWindow		inherited;

CUIStatic*				m_UIImage;
CUIStatic*				m_UIText;
CEncyclopediaArticle*	m_Article;

protected:
			void		AdjustLauout				();

public:
					CUIEncyclopediaArticleWnd		();
	virtual			~CUIEncyclopediaArticleWnd		();
			void	Init							(const char* xml_name, const char* start_from);
			void	SetArticle						(CEncyclopediaArticle*);
			void	SetArticle						(const char*);
};