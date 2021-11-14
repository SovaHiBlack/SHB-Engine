#pragma once

#include "UIStatic.h"//
#include "UI3tButton.h"//
#include "UIFrameLineWnd.h"//

#include "..\InfoPortion.h"

#include "UICharacterInfo.h"//
#include "UIItemInfo.h"//

#include "UIWndCallback.h"

class CUIScrollView;
class CUIXml;

class CUITalkDialogWnd : public CUIWindow, public CUIWndCallback
{
private:
	using inherited								= CUIWindow;
	CUIXml*										m_uiXml;

public:
						CUITalkDialogWnd		( );
	virtual				~CUITalkDialogWnd		( );

	virtual void		Init					(float x, float y, float width, float height);

	virtual void		SendMessage				(CUIWindow* pWnd, signed short msg, void* pData = nullptr);

	virtual void		Show					( );
	virtual void		Hide					( );

	u32					GetHeaderColor			( )
	{
		return m_iNameTextColor;
	}
	CGameFont*			GetHeaderFont			( )
	{
		return m_pNameTextFont;
	}
	u32					GetOurReplicsColor		( )
	{
		return m_uOurReplicsColor;
	}

	//список вопросов, которые мы можем задавать персонажу
	CSharedString									m_ClickedQuestionID;

	//элементы интерфейса диалога
	CUIFrameLineWnd								UIDialogFrame;
	CUIFrameLineWnd								UIOurPhrasesFrame;

	CUIStatic									UIStaticTop;
	CUIStatic									UIStaticBottom;

	CUI3tButton									UIToTradeButton;

	//информаци€ о персонажах
	CUIStatic									UIOurIcon;
	CUIStatic									UIOthersIcon;
	CUICharacterInfo							UICharacterInfoLeft;
	CUICharacterInfo							UICharacterInfoRight;

	void				AddQuestion				(const char* str, const char* value);
	void				AddAnswer				(const char* SpeakerName, const char* str, bool bActor);
	void				AddIconedAnswer			(const char* text, const char* texture_name, Frect texture_rect, const char* templ_name);
	void				ClearAll				( );
	void				ClearQuestions			( );

	void				SetOsoznanieMode		(bool b);

private:
	CUIScrollView*								UIQuestionsList;
	CUIScrollView*								UIAnswersList;

	//Ўрифт и цвет текста с именем персонажа
	CGameFont*									m_pNameTextFont;
	u32											m_iNameTextColor;
	//÷вет тeкста и шрифт наших реплик
	u32											m_uOurReplicsColor;

	void __stdcall		OnTradeClicked			(CUIWindow* w, void*);
	void __stdcall		OnQuestionClicked		(CUIWindow* w, void*);
};

class CUIQuestionItem : public CUIWindow, public CUIWndCallback
{
	using inherited								= CUIWindow;
	float										m_min_height;

public:
	CUI3tButton*								m_text;
	CSharedString									m_s_value;
						CUIQuestionItem			(CUIXml* xml_doc, const char* path);
	void				Init					(const char* val, const char* text);

	virtual void		SendMessage				(CUIWindow* pWnd, signed short msg, void* pData = nullptr);
	void __stdcall		OnTextClicked			(CUIWindow* w, void*);
};

class CUIAnswerItem : public CUIWindow
{
	using inherited								= CUIWindow;

	float										m_min_height;
	float										m_bottom_footer;
	CUIStatic*									m_text;
	CUIStatic*									m_name;

public:
						CUIAnswerItem			(CUIXml* xml_doc, const char* path);
	void				Init					(const char* text, const char* name);
};

class CUIAnswerItemIconed : public CUIAnswerItem
{
	using inherited								= CUIAnswerItem;
	CUIStatic*									m_icon;

public:
						CUIAnswerItemIconed		(CUIXml* xml_doc, const char* path);
	void				Init					(const char* text, const char* texture_name, Frect texture_rect);
};
