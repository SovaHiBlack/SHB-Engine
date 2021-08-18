//  Filename:   UIMultiTextStatic.cpp
//  Статик контрол на котором можно выводить множество надписей различными
//	шрифтами, цветами и даже с анимацией

#pragma once

#include "UITextBanner.h"//

class CUIMultiTextStatic : public CUIStatic
{
	using inherited									= CUIStatic;

public:
	typedef struct SPh
	{
		float										outX;
		float										outY;
		float										maxWidth;
		CUIStatic::EElipsisPosition					elipsisPos;
		CUITextBanner								effect;
		CSharedString									str;

		void						SetText			(const char* fmt, ...);

		// Ctor		
									SPh				( );
									CSharedString									key;
	} SinglePhrase;

	using Phrases									= xr_vector<SinglePhrase>;
	using Phrases_it								= Phrases::iterator;

protected:
	Phrases											m_vPhrases;

public:
						CUIMultiTextStatic			( );
	virtual				~CUIMultiTextStatic			( );

	virtual void		Draw						( );
	virtual void		Update						( );
	// Добавить надпись
	// Return:	Указатель на добавленную запись
	SinglePhrase*		AddPhrase					( );
	// Получить запись по номеру
	SinglePhrase*		GetPhraseByIndex			(u32 idx);
	void				RemovePhraseByIndex			(u32 idx);
};

class CUICaption :protected CUIMultiTextStatic
{
	using inherited									= CUIMultiTextStatic;

	u32					findIndexOf					(const CSharedString& key);
	u32					findIndexOf_				(const CSharedString& key);

public:
	virtual void		Draw						( );
	void				addCustomMessage			(const CSharedString& msg_name, float x, float y, float font_size, CGameFont* pFont, CGameFont::EAligment al, u32 color, const char* def_str = "");
	EffectParams*		customizeMessage			(const CSharedString& msg_name, const CUITextBanner::TextBannerStyles styleName);
	void				setCaption					(const CSharedString& msg_name, const char* message_to_out, u32 color = 0, bool replaceColor = false);
	void				removeCustomMessage			(const CSharedString& msg_name);
};
