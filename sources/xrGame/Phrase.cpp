///////////////////////////////////////////////////////////////
// Phrase.cpp
// �����, ����������� ����� (������� �������)
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "phrase.h"

#include "AISpace.h"
#include "gameobject.h"
#include "script_game_object.h"

CPhrase::CPhrase	(void)
{
	m_ID				= "";
	m_iGoodwillLevel	= 0;
}
CPhrase::~CPhrase	(void)
{
}

pcstr CPhrase::GetText		()	const
{
	return m_text.c_str();
}

bool	CPhrase::IsDummy()		const
{
	if( xr_strlen(GetText()) == 0 )
		return true;

	return false;
}
