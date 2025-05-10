#pragma once

#include "UIStatic.h"
#include "UI3tButton.h"
#include "UIFrameLineWnd.h"

#include "../InfoPortion.h"

#include "UICharacterInfo.h"//
#include "UIItemInfo.h"//

#include "UIWndCallback.h"

class CUIScrollView;
class CUIXml;

class CUITalkDialogWnd : public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow inherited;
	CUIXml* m_uiXml;

public:
	CUITalkDialogWnd( );
	virtual ~CUITalkDialogWnd( );

	virtual void Init(f32 x, f32 y, f32 width, f32 height);

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData = NULL);

	virtual void Show( );
	virtual void Hide( );

	u32			GetHeaderColor( )
	{
		return m_iNameTextColor;
	}
	CGameFont* GetHeaderFont( )
	{
		return m_pNameTextFont;
	}
	u32			GetOurReplicsColor( )
	{
		return m_uOurReplicsColor;
	}

	shared_str			m_ClickedQuestionID;

	//список вопросов, которые мы можем задавать персонажу

	//элементы интерфейса диалога
	CUIFrameLineWnd		UIDialogFrame;
	CUIFrameLineWnd		UIOurPhrasesFrame;

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

	CUI3tButton			UIToTradeButton;

	//информаци€ о персонажах
	CUIStatic			UIOurIcon;
	CUIStatic			UIOthersIcon;
	CUICharacterInfo	UICharacterInfoLeft;
	CUICharacterInfo	UICharacterInfoRight;

	void				AddQuestion(pcstr str, pcstr value);
	void				AddAnswer(pcstr SpeakerName, pcstr str, bool bActor);
	void				AddIconedAnswer(pcstr text, pcstr texture_name, fRect texture_rect, pcstr templ_name);
	void				ClearAll( );
	void				ClearQuestions( );

	void				SetOsoznanieMode(bool b);

private:
	CUIScrollView* UIQuestionsList;
	CUIScrollView* UIAnswersList;

	// Ўрифт и цвет текста с именем персонажа
	CGameFont* m_pNameTextFont;
	u32					m_iNameTextColor;
	// ÷вет тeкста и шрифт наших реплик
	u32					m_uOurReplicsColor;

	void __stdcall		OnTradeClicked(CUIWindow* w, pvoid);
	void __stdcall		OnQuestionClicked(CUIWindow* w, pvoid);
};

class CUIQuestionItem :public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	f32			m_min_height;

public:
	CUI3tButton* m_text;
	shared_str		m_s_value;
	CUIQuestionItem(CUIXml* xml_doc, pcstr path);
	void			Init(pcstr val, pcstr text);

	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData = NULL);
	void __stdcall	OnTextClicked(CUIWindow* w, void*);
};

class CUIAnswerItem :public CUIWindow
{
	typedef CUIWindow inherited;

	f32			m_min_height;
	f32			m_bottom_footer;
	CUIStatic* m_text;
	CUIStatic* m_name;

public:
	CUIAnswerItem(CUIXml* xml_doc, pcstr path);
	void			Init(pcstr text, pcstr name);
};

class CUIAnswerItemIconed :public CUIAnswerItem
{
	typedef CUIAnswerItem inherited;
	CUIStatic* m_icon;

public:
	CUIAnswerItemIconed(CUIXml* xml_doc, pcstr path);
	void			Init(pcstr text, pcstr texture_name, fRect texture_rect);
};
