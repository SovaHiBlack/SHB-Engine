//  Filename:   UIEncyclopediaWnd.h
//  Encyclopedia window

#pragma once

#include "UIWindow.h"//
#include "..\EncyclopediaArticle_defs.h"

class CEncyclopediaArticle;
class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUIListWnd;
class CUIScrollView;
class CUITreeViewItem;

class CUIEncyclopediaWnd : public CUIWindow
{
private:
	using inherited								= CUIWindow;

	enum
	{
		eNeedReload								= (1 << 0)
	};

	Flags16										m_flags;

public:
						CUIEncyclopediaWnd		( );
	virtual				~CUIEncyclopediaWnd		( );

	virtual void		Init					( );
	virtual void		Show					(bool status);
	virtual void		SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = nullptr);
	virtual void		Draw					( );

	void				AddArticle				(shared_str, bool bReaded);
	void				DeleteArticles			( );
	bool				HasArticle				(shared_str);

	void				ReloadArticles			( );
	virtual void		Reset					( );

protected:
	u32											prevArticlesCount;
	//Элементы графического оформления
	CUIFrameWindow*								UIEncyclopediaIdxBkg;
	CUIFrameWindow*								UIEncyclopediaInfoBkg;
	CUIFrameLineWnd*							UIEncyclopediaIdxHeader;
	CUIFrameLineWnd*							UIEncyclopediaInfoHeader;
	CUIAnimatedStatic*							UIAnimation;
	CUIStatic*									UIArticleHeader;

	//Хранилище статей
	using ArticlesDB							= xr_vector<CEncyclopediaArticle*>;
	using ArticlesDB_it							= ArticlesDB::iterator;

	ArticlesDB									m_ArticlesDB;
	CGameFont*									m_pTreeRootFont;
	u32											m_uTreeRootColor;
	CGameFont*									m_pTreeItemFont;
	u32											m_uTreeItemColor;

	CUIListWnd*									UIIdxList;
	CUIScrollView*								UIInfoList;

	void				SetCurrentArtice		(CUITreeViewItem* pTVItem);
};
