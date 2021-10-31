//	Module		: StringTable.h
//	Description	: Таблица строк используемых в игре

#pragma once

#include "StringTable_defs.h"

using StringTableMap									= xr_map<StringTableID, StringTableValue>;
using StringTableMap_it									= StringTableMap::iterator;

struct SStringTableData
{
	CSharedString										m_sLanguage;
	StringTableMap										m_StringTable;
	StringTableMap										m_string_key_binding;
};

class CStringTable
{
public:
								CStringTable			( );

	static void					Destroy					( );
	StringTableValue			translate				(const StringTableID& str_id) const;

	static BOOL											m_bWriteErrorsToLog;
	static void					ReparseKeyBindings		( );

private:
	void						Init					( );
	void						Load					(const char* xml_file);
	static StringTableValue		ParseLine				(const char* str, const char* key, bool bFirst);
	static SStringTableData*							pData;
};
