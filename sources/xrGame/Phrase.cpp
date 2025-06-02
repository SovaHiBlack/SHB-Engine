///////////////////////////////////////////////////////////////
// Phrase.cpp
// класс, описывающий фразу (элемент диалога)
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Phrase.h"

#include "AISpace.h"
#include "GameObject.h"
#include "script_game_object.h"

CPhrase::CPhrase( )
{
	m_ID = "";
	m_iGoodwillLevel = 0;
}

CPhrase::~CPhrase( )
{ }

pcstr CPhrase::GetText( ) const
{
	return m_text.c_str( );
}

bool CPhrase::IsDummy( ) const
{
	if (xr_strlen(GetText( )) == 0)
	{
		return true;
	}

	return false;
}
