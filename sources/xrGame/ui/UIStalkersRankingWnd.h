#pragma once

#include "UIWindow.h"//

class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUICharacterInfo;
class CUIScrollView;
class CUIXml;
class CSE_ALifeTraderAbstract;

class CUIStalkersRankingWnd : public CUIWindow
{
	using inherited												= CUIWindow;

public:
	void					Init								( );
	virtual void			Show								(bool status);

protected:
	CUIFrameWindow*												UIInfoFrame;
	CUIFrameWindow*												UICharIconFrame;
	CUIFrameLineWnd*											UIInfoHeader;
	CUIFrameLineWnd*											UICharIconHeader;
	CUIAnimatedStatic*											UIAnimatedIcon;
	// информация о персонаже
	CUIWindow*													UICharacterWindow;
	CUICharacterInfo*											UICharacterInfo;
	void					FillList							( );
	CUIScrollView*												UIList;
	void					AddStalkerItem						(CUIXml* xml, int num, CSE_ALifeTraderAbstract* t);
	void					AddActorItem						(CUIXml* xml, int num, CSE_ALifeTraderAbstract* t);

public:
	CUIScrollView&			GetTopList							( )
	{
		return *UIList;
	}
	void					ShowHumanInfo						(U16 id);
	virtual void			Reset								( );
};

class CUIStalkerRankingInfoItem : public CUIWindow, public CUISelectable
{
	CUIStalkersRankingWnd*										m_StalkersRankingWnd;
	u32															m_stored_alpha;

public:
	U16															m_humanID;
	CUIStatic*													m_text1;
	CUIStatic*													m_text2;
	CUIStatic*													m_text3;

							CUIStalkerRankingInfoItem			(CUIStalkersRankingWnd*);

	void					Init								(CUIXml* xml, const char* path, int idx);
	virtual void			SetSelected							(bool b);
	virtual bool			OnMouseDown							(int mouse_btn);
};

class CUIStalkerRankingElipsisItem : public CUIStalkerRankingInfoItem
{
	using inherited												= CUIStalkerRankingInfoItem;

public:
							CUIStalkerRankingElipsisItem		(CUIStalkersRankingWnd*);
	virtual void			SetSelected							(bool b);
	virtual bool			OnMouseDown							(int mouse_btn);
};
