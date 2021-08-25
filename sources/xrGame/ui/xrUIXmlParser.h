#pragma once

#include "../../Core/xrXMLParser.h"

class CUIXml : public CXml
{
	int											m_dbg_id;

public:
							CUIXml				( );
	virtual					~CUIXml				( );

	virtual CSharedString	correct_file_name	(const char* path, const char* fn);
};
