///////////////////////////////////////////////////////////////
// Phrase.h
// класс, описывающий фразу (элемент диалога)
///////////////////////////////////////////////////////////////

#pragma once

#include "PhraseScript.h"

class CPhraseDialog;
class CGameObject;

class CPhrase
{
private:
	friend CPhraseDialog;

public:
	CPhrase( );
	virtual ~CPhrase( );

	void				SetText(pcstr text)
	{
		m_text = text;
	}
	pcstr				GetText( ) const;

	void				SetID(const shared_str& id)
	{
		m_ID = id;
	}
	const shared_str& GetID( ) const
	{
		return m_ID;
	}

	s32					GoodwillLevel( ) const
	{
		return m_iGoodwillLevel;
	}

	bool				IsDummy( ) const;
	CPhraseScript* GetPhraseScript( )
	{
		return &m_PhraseScript;
	}

protected:
	//уникальный индекс в списке фраз диалога
	shared_str		m_ID;
	//текстовое представление фразы
	xr_string		m_text;

	//минимальный уровень благосклоггости, необходимый для того чтоб фразу можно было сказать
	s32				m_iGoodwillLevel;

	//для вызова скриптовых функций
	CPhraseScript	m_PhraseScript;
};
