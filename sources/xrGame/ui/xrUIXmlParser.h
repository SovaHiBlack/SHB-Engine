#pragma once

#include "../../../xrXMLParser/xrXMLParser.h"

class CUIXml : public CXml
{
	s32						m_dbg_id;

public:
							CUIXml				( );
	virtual					~CUIXml				( );

	virtual shared_str		correct_file_name	(pcstr path, pcstr fn);
};
