// StringTable.h: таблица строк используемых в игре
#pragma once

#include "StringTable_defs.h"

using STRING_TABLE_MAP									= xr_map<STRING_ID, STRING_VALUE>;
using STRING_TABLE_MAP_IT								= STRING_TABLE_MAP::iterator;

struct STRING_TABLE_DATA
{
	CSharedString											m_sLanguage;
	STRING_TABLE_MAP									m_StringTable;
	STRING_TABLE_MAP									m_string_key_binding;
};

class CStringTable
{
public:
								CStringTable			( );

	static void					Destroy					( );
	STRING_VALUE				translate				(const STRING_ID& str_id) const;

	static BOOL											m_bWriteErrorsToLog;
	static void					ReparseKeyBindings		( );

private:
	void						Init					( );
	void						Load					(const char* xml_file);
	static STRING_VALUE			ParseLine				(const char* str, const char* key, bool bFirst);
	static STRING_TABLE_DATA*							pData;
};
