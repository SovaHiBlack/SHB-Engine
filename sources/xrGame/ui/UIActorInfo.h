#pragma once

#include "UIWindow.h"

class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUICharacterInfo;
class CUIScrollView;
class CUIXml;

class CUIActorInfoWnd : public CUIWindow
{
	typedef CUIWindow									inherited;

public:
							CUIActorInfoWnd				( );
	virtual void			Init						( );
	virtual void			Show						(bool status);
	CUIScrollView&			DetailList					( )
	{
		return *UIDetailList;
	}
	CUIScrollView&			MasterList					( )
	{
		return *UIMasterList;
	}
	void					FillPointsDetail			(const shared_str& idx);
	virtual void			Reset						( );

protected:
	CUIFrameWindow*										UIInfoFrame;
	CUIFrameLineWnd*									UIInfoHeader;
	CUIFrameWindow*										UICharIconFrame;
	CUIFrameLineWnd*									UICharIconHeader;
	CUIAnimatedStatic*									UIAnimatedIcon;

	CUIWindow*											UICharacterWindow;
	CUICharacterInfo*									UICharacterInfo;

	CUIScrollView*										UIMasterList;
	CUIScrollView*										UIDetailList;

	void					FillPointsInfo				( );
	void					FillReputationDetails		(CUIXml* xml, pcstr path);
	void					FillMasterPart				(CUIXml* xml, const shared_str& key_name);
};

class CUIActorStaticticHeader : public CUIWindow, public CUISelectable
{
	CUIActorInfoWnd*									m_actorInfoWnd;

protected:
	u32													m_stored_alpha;

public:
	CUIStatic*											m_text1;
	CUIStatic*											m_text2;

public:
							CUIActorStaticticHeader		(CUIActorInfoWnd* w);
	void					Init						(CUIXml* xml, pcstr path, s32 idx_in_xml);
	virtual bool			OnMouseDown					(s32 mouse_btn);
	virtual void			SetSelected					(bool b);

	shared_str											m_id;
};

class CUIActorStaticticDetail : public CUIWindow
{
public:
	CUIStatic*											m_text0;
	CUIStatic*											m_text1;
	CUIStatic*											m_text2;
	CUIStatic*											m_text3;

public:
	void					Init						(CUIXml* xml, pcstr path, s32 xml_idx);
};
