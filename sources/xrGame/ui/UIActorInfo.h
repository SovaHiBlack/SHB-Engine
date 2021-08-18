#pragma once

#include "UIWindow.h"//

class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUICharacterInfo;
class CUIScrollView;
class CUIXml;

class CUIActorInfoWnd : public CUIWindow
{
	using inherited										= CUIWindow;

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
	void					FillPointsDetail			(const CSharedString& idx);
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
	void					FillReputationDetails		(CUIXml* xml, const char* path);
	void					FillMasterPart				(CUIXml* xml, const CSharedString& key_name);
};

class CUIActorStaticticHeader : public CUIWindow, public CUISelectable
{
	CUIActorInfoWnd*									m_actorInfoWnd;

protected:
	u32													m_stored_alpha;

public:
	CUIStatic*											m_text1;
	CUIStatic*											m_text2;

							CUIActorStaticticHeader		(CUIActorInfoWnd* w);
	void					Init						(CUIXml* xml, const char* path, int idx_in_xml);
	virtual bool			OnMouseDown					(int mouse_btn);
	virtual void			SetSelected					(bool b);

	CSharedString											m_id;
};

class CUIActorStaticticDetail : public CUIWindow
{
public:
	CUIStatic*											m_text0;
	CUIStatic*											m_text1;
	CUIStatic*											m_text2;
	CUIStatic*											m_text3;

	void					Init						(CUIXml* xml, const char* path, int xml_idx);
};
