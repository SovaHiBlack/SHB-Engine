// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: UIXml.h
// Author		: Anahoret
// Description	: 
// ========================================= SovaHiBlack© =========================================

#pragma once
#ifndef GAME_UIXML_H_INCLUDED
#define GAME_UIXML_H_INCLUDED

#include "../../../xrXMLParser/xrXMLParser.h"

class CUIXml : public CXml
{
	s32											m_dbg_id;

public:
							CUIXml				( );
	virtual					~CUIXml				( );

	virtual shared_str		correct_file_name	(pcstr path, pcstr fn);
};

#endif // ndef GAME_UIXML_H_INCLUDED