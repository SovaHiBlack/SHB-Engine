#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"
#include "../EncyclopediaArticle_defs.h"

class CUINewsWnd;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUIAnimatedStatic;
class CUIStatic;
class CUITabControl;
class CUIScrollView;
class CUIListWnd;
class CEncyclopediaArticle;

class CUIDiaryWnd : public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	enum EDiaryFilter
	{
		eJournal = 0,
		eNews,
		eNone
	};

protected:
	EDiaryFilter m_currFilter;

	CUINewsWnd* m_UINewsWnd;

	CUIWindow* m_UILeftWnd;
	CUIWindow* m_UIRightWnd;
	CUIFrameWindow* m_UILeftFrame;
	CUIFrameLineWnd* m_UILeftHeader;
	CUIFrameWindow* m_UIRightFrame;
	CUIFrameLineWnd* m_UIRightHeader;
	CUIAnimatedStatic* m_UIAnimation;
	CUITabControl* m_FilterTab;
	CUIListWnd* m_SrcListWnd;
	CUIScrollView* m_DescrView;
	CGameFont* m_pTreeRootFont;
	u32 m_uTreeRootColor;
	CGameFont* m_pTreeItemFont;
	u32 m_uTreeItemColor;

	xr_vector<fVector2>	m_sign_places;
	CUIStatic* m_updatedSectionImage;
	CUIStatic* m_oldSectionImage;

	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;
	ArticlesDB				m_ArticlesDB;

	void __stdcall	OnFilterChanged(CUIWindow*, pvoid);
	void __stdcall	OnSrcListItemClicked(CUIWindow*, pvoid);
	void		UnloadJournalTab( );
	void		LoadJournalTab(ARTICLE_DATA::EArticleType _type);
	void		UnloadInfoTab( );
	void		LoadInfoTab( );
	void		UnloadNewsTab( );
	void		LoadNewsTab( );
	void		Reload(EDiaryFilter new_filter);

public:
	CUIDiaryWnd( );
	virtual				~CUIDiaryWnd( );

	virtual void		SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData);
	virtual	void		Draw( );
	virtual	void		Reset( );

	void				Init( );
	void				AddNews( );
	void				MarkNewsAsRead(bool status);
	virtual void		Show(bool status);
};
