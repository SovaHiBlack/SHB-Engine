#include "stdafx.h"
#include "string_table.h"

#include "ui/xrUIXmlParser.h"
#include "xr_level_controller.h"

STRING_TABLE_DATA* CStringTable::pData = NULL;
BOOL CStringTable::m_bWriteErrorsToLog = FALSE;

CStringTable::CStringTable( )
{
	Init( );
}

void CStringTable::Destroy( )
{
	xr_delete(pData);
}

void CStringTable::Init( )
{
	if (NULL != pData)
	{
		return;
	}

	pData = xr_new<STRING_TABLE_DATA>( );

	//имя языка, если не задано (NULL), то первый <text> в <string> в XML
	pData->m_sLanguage = pSettings->r_string("string_table", "language");

	pcstr S = pSettings->r_string("string_table", "files");
	if (S && S[0])
	{
		string128	xml_file;
		s32			count = _GetItemCount(S);
		for (s32 it = 0; it < count; ++it)
		{
			_GetItem(S, it, xml_file);
			Load(xml_file);
		}
	}
}

void CStringTable::Load(pcstr xml_file)
{
	CUIXml						uiXml;
	string_path					xml_file_full;
	strconcat(sizeof(xml_file_full), xml_file_full, xml_file, ".xml");
	string_path					_s;
	strconcat(sizeof(_s), _s, STRING_TABLE_PATH, "\\", *(pData->m_sLanguage));

	bool xml_result = uiXml.Init(CONFIG_PATH, _s, xml_file_full);
	if (!xml_result)
	{
		Debug.fatal(DEBUG_INFO, "string table xml file not found %s, for language %s", xml_file_full, *(pData->m_sLanguage));
	}

	//общий список всех записей таблицы в файле
	s32 string_num = uiXml.GetNodesNum(uiXml.GetRoot( ), "string");

	for (s32 i = 0; i < string_num; ++i)
	{
		pcstr string_name = uiXml.ReadAttrib(uiXml.GetRoot( ), "string", i, "id", NULL);
		VERIFY3(pData->m_StringTable.find(string_name) == pData->m_StringTable.end( ), "duplicate string table id", string_name);

		pcstr string_text = uiXml.Read(uiXml.GetRoot( ), "string:text", i, NULL);
		if (m_bWriteErrorsToLog && string_text)
		{
			Msg("[string table] '%s' no translation in '%s'", string_name, *(pData->m_sLanguage));
		}

		VERIFY3(string_text, "string table entry does not has a text", string_name);

		STRING_VALUE str_val = ParseLine(string_text, string_name, true);

		pData->m_StringTable[string_name] = str_val;
	}
}

void CStringTable::ReparseKeyBindings( )
{
	if (!pData)
	{
		return;
	}

	STRING_TABLE_MAP_IT it = pData->m_string_key_binding.begin( );
	STRING_TABLE_MAP_IT it_e = pData->m_string_key_binding.end( );

	for (; it != it_e; ++it)
	{
		pData->m_StringTable[it->first] = ParseLine(*it->second, *it->first, false);
	}
}

STRING_VALUE CStringTable::ParseLine(pcstr str, pcstr skey, bool bFirst)
{
#define ACTION_STR "$$ACTION_"
#define LEN			9	// s32 LEN = (s32)xr_strlen(ACTION_STR);

	xr_string			res;
	s32 k				= 0;
	pcstr				b;
	string256			buff;
	string256			srcbuff;
	bool b_hit			= false;

	while ((b = strstr(str + k, ACTION_STR)) != 0)
	{
		buff[0] = 0;
		srcbuff[0] = 0;
		res.append(str + k, b - str - k);
		pcstr e = strstr(b + LEN, "$$");

		s32 len = (s32)(e - b - LEN);

		strncpy(srcbuff, b + LEN, len);
		srcbuff[len] = 0;
		GetActionAllBinding(srcbuff, buff, sizeof(buff));
		res.append(buff, xr_strlen(buff));

		k = (s32)(b - str);
		k += len;
		k += LEN;
		k += 2;
		b_hit = true;
	}

	if (k < (s32)xr_strlen(str))
	{
		res.append(str + k);
	}

	if (b_hit && bFirst)
	{
		pData->m_string_key_binding[skey] = str;
	}

	return STRING_VALUE(res.c_str( ));
}

STRING_VALUE CStringTable::translate(const STRING_ID& str_id) const
{
	VERIFY(pData);

	STRING_VALUE res = pData->m_StringTable[str_id];
	if (!res)
	{
		if (m_bWriteErrorsToLog && *str_id != NULL && xr_strlen(*str_id) > 0)
		{
			Msg("[string table] '%s' has no entry", *str_id);
		}

		return str_id;
	}

	return pData->m_StringTable[str_id];
}
