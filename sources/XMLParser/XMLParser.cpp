#include "stdafx.h"

#include "XMLParser.h"

XMLPARSER_API CXml::CXml( ) : m_root(NULL), m_pLocalRoot(NULL)
{ }

XMLPARSER_API CXml::~CXml( )
{
	ClearInternal( );
}

void CXml::ClearInternal( )
{
	m_Doc.Clear( );
}

void ParseFile(pcstr path, CMemoryWriter& W, IReader* F, CXml* xml)
{
	string4096 str;

	while (!F->eof( ))
	{
		F->r_string(str, sizeof(str));

		if (str[0] && (str[0] == '#') && strstr(str, "#include"))
		{
			string256 inc_name;
			if (_GetItem(str, 1, inc_name, '"'))
			{
				IReader* I = nullptr;
				if (inc_name == strstr(inc_name, "ui\\"))
				{
					shared_str fn = xml->correct_file_name("ui", strchr(inc_name, '\\') + 1);
					string_path buff;
					strconcat(sizeof(buff), buff, "ui\\", fn.c_str( ));
					I = FS.r_open(path, buff);
				}

				if (!I)
				{
					I = FS.r_open(path, inc_name);
				}

				if (!I)
				{
					string1024 str;
					sprintf(str, "XML file[%s] parsing failed. Can't find include file:[%s]", path, inc_name);
					R_ASSERT2(false, str);
				}

				ParseFile(path, W, I, xml);
				FS.r_close(I);
			}
		}
		else
		{
			W.w_string(str);
		}
	}
}

bool CXml::Init(pcstr path_alias, pcstr path, pcstr _xml_filename)
{
	const shared_str fn = correct_file_name(path, _xml_filename);

	string_path str;
	sprintf(str, "%s\\%s", path, *fn);
	return Init(path_alias, str);
}

//инициализаци€ и загрузка XML файла
bool CXml::Init(pcstr path, pcstr xml_filename)
{
	strcpy(m_xml_file_name, xml_filename);
	// Load and parse xml file

	IReader* F = FS.r_open(path, xml_filename);
	if (F == NULL)
	{
		return false;
	}

	CMemoryWriter W;
	ParseFile(path, W, F, this);
	W.w_stringZ("");
	FS.r_close(F);

	m_Doc.Parse((pcstr)W.pointer( ));
	if (m_Doc.Error( ))
	{
		string1024 str;
		sprintf(str, "XML file:%s value:%s errDescr:%s", m_xml_file_name, m_Doc.Value( ), m_Doc.ErrorDesc( ));
		R_ASSERT2(false, str);
	}

	m_root = m_Doc.FirstChildElement( );

	return true;
}

XML_NODE* CXml::NavigateToNode(XML_NODE* start_node, pcstr path, s32 node_index)
{
	R_ASSERT3(start_node && path, "NavigateToNode failed in XML file ", m_xml_file_name);
	XML_NODE* node = NULL;
	XML_NODE* node_parent = NULL;
	string_path buf_str;
	VERIFY(xr_strlen(path) < 200);
	buf_str[0] = 0;
	strcpy(buf_str, path);

	char seps[ ] = ":";
	pstr token;
	s32 tmp = 0;

	//разбить путь на отдельные подпути
	token = strtok(buf_str, seps);

	if (token != NULL)
	{
		node = start_node->FirstChild(token);

		while (tmp++ < node_index && node)
		{
			node = start_node->IterateChildren(token, node);
		}
	}

	while (token != NULL)
	{
		// Get next token:
		token = strtok(NULL, seps);

		if (token != NULL)
		{
			if (node != 0)
			{
				node_parent = node;
				node = node_parent->FirstChild(token);
			}
		}
	}

	return node;
}

XML_NODE* CXml::NavigateToNode(pcstr path, s32 node_index)
{
	return NavigateToNode(GetLocalRoot( ) ? GetLocalRoot( ) : GetRoot( ), path, node_index);
}

XML_NODE* CXml::NavigateToNodeWithAttribute(pcstr tag_name, pcstr attrib_name, pcstr attrib_value)
{
	XML_NODE* root = GetLocalRoot( ) ? GetLocalRoot( ) : GetRoot( );
	const s32 tabsCount = GetNodesNum(root, tag_name);
	for (s32 i = 0; i < tabsCount; ++i)
	{
		pcstr result = ReadAttrib(root, tag_name, i, attrib_name, "");
		if (result && xr_strcmp(result, attrib_value) == 0)
		{
			return NavigateToNode(root, tag_name, i);
		}
	}

	return NULL;
}

pcstr CXml::Read(pcstr path, s32 index, pcstr default_str_val)
{
	XML_NODE* node = NavigateToNode(path, index);
	pcstr result = Read(node, default_str_val);
	return result;
}

pcstr CXml::Read(XML_NODE* start_node, pcstr path, s32 index, pcstr default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	pcstr result = Read(node, default_str_val);
	return result;
}

pcstr CXml::Read(XML_NODE* node, pcstr default_str_val)
{
	if (node == NULL)
	{
		return default_str_val;
	}
	else
	{
		node = node->FirstChild( );
		if (!node)
		{
			return default_str_val;
		}

		TiXmlText* text = node->ToText( );
		if (text)
		{
			return text->Value( );
		}
		else
		{
			return default_str_val;
		}
	}
}

s32 CXml::ReadInt(XML_NODE* node, s32 default_int_val)
{
	pcstr result_str = Read(node, NULL);
	if (result_str == NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}

s32 CXml::ReadInt(pcstr path, s32 index, s32 default_int_val)
{
	pcstr result_str = Read(path, index, NULL);
	if (result_str == NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}

s32 CXml::ReadInt(XML_NODE* start_node, pcstr path, s32 index, s32 default_int_val)
{
	pcstr result_str = Read(start_node, path, index, NULL);
	if (result_str == NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}

f32 CXml::ReadFlt(pcstr path, s32 index, f32 default_flt_val)
{
	pcstr result_str = Read(path, index, NULL);
	if (result_str == NULL)
	{
		return default_flt_val;
	}

	return (f32)atof(result_str);
}

f32 CXml::ReadFlt(XML_NODE* start_node, pcstr path, s32 index, f32 default_flt_val)
{
	pcstr result_str = Read(start_node, path, index, NULL);
	if (result_str == NULL)
	{
		return default_flt_val;
	}

	return (f32)atof(result_str);
}

f32 CXml::ReadFlt(XML_NODE* node, f32 default_flt_val)
{
	pcstr result_str = Read(node, NULL);
	if (result_str == NULL)
	{
		return default_flt_val;
	}

	return (f32)atof(result_str);
}

pcstr CXml::ReadAttrib(XML_NODE* start_node, pcstr path, s32 index, pcstr attrib, pcstr default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	pcstr result = ReadAttrib(node, attrib, default_str_val);
	return result;
}

pcstr CXml::ReadAttrib(pcstr path, s32 index, pcstr attrib, pcstr default_str_val)
{
	XML_NODE* node = NavigateToNode(path, index);
	pcstr result = ReadAttrib(node, attrib, default_str_val);
	return result;
}

pcstr CXml::ReadAttrib(XML_NODE* node, pcstr attrib, pcstr default_str_val)
{
	if (node == NULL)
	{
		return default_str_val;
	}
	else
	{
		/*
				//об€зательно делаем ref_str, а то
				//не сможем запомнить строку и return вернет левый указатель
				shared_str result_str;
		*/
		pcstr result_str = NULL;
		//  астаем ниже по иерархии

		const TiXmlElement* el = node->ToElement( );

		if (el)
		{
			result_str = el->Attribute(attrib);
			if (result_str)
			{
				return result_str;
			}
			else
			{
				return default_str_val;
			}
		}
		else
		{
			return default_str_val;
		}
	}
}

s32 CXml::ReadAttribInt(XML_NODE* node, pcstr attrib, s32 default_int_val)
{
	pcstr result_str = ReadAttrib(node, attrib, NULL);
	if (result_str == NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}

s32 CXml::ReadAttribInt(pcstr path, s32 index, pcstr attrib, s32 default_int_val)
{
	pcstr result_str = ReadAttrib(path, index, attrib, NULL);
	if (result_str == NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}

s32 CXml::ReadAttribInt(XML_NODE* start_node, pcstr path, s32 index, pcstr attrib, s32 default_int_val)
{
	pcstr result_str = ReadAttrib(start_node, path, index, attrib, NULL);
	if (result_str == NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}

f32 CXml::ReadAttribFlt(pcstr path, s32 index, pcstr attrib, f32 default_flt_val)
{
	pcstr result_str = ReadAttrib(path, index, attrib, NULL);
	if (result_str == NULL)
	{
		return default_flt_val;
	}

	return (f32)atof(result_str);
}

f32 CXml::ReadAttribFlt(XML_NODE* start_node, pcstr path, s32 index, pcstr attrib, f32 default_flt_val)
{
	pcstr result_str = ReadAttrib(start_node, path, index, attrib, NULL);
	if (result_str == NULL)
	{
		return default_flt_val;
	}

	return (f32)atof(result_str);
}

f32 CXml::ReadAttribFlt(XML_NODE* node, pcstr attrib, f32 default_flt_val)
{
	pcstr result_str = ReadAttrib(node, attrib, NULL);
	if (result_str == NULL)
	{
		return default_flt_val;
	}

	return (f32)atof(result_str);
}

s32 CXml::GetNodesNum(pcstr path, s32 index, pcstr tag_name)
{
	XML_NODE* node = NULL;
	XML_NODE* root = GetLocalRoot( ) ? GetLocalRoot( ) : GetRoot( );
	if (path != NULL)
	{
		node = NavigateToNode(path, index);
		if (node == NULL)
		{
			node = root;
		}
	}
	else
	{
		node = root;
	}

	if (node == NULL)
	{
		return 0;
	}

	return GetNodesNum(node, tag_name);
}

s32 CXml::GetNodesNum(XML_NODE* node, pcstr  tag_name)
{
	if (node == NULL)
	{
		return 0;
	}

	XML_NODE* el = NULL;

	if (!tag_name)
	{
		el = node->FirstChild( );
	}
	else
	{
		el = node->FirstChild(tag_name);
	}

	s32 result = 0;

	while (el)
	{
		++result;
		if (!tag_name)
		{
			el = el->NextSibling( );
		}
		else
		{
			el = el->NextSibling(tag_name);
		}
	}

	return result;
}

//нахождение элемнета по его атрибуту
XML_NODE* CXml::SearchForAttribute(pcstr path, s32 index, pcstr tag_name, pcstr attrib, pcstr attrib_value_pattern)
{
	XML_NODE* start_node = NavigateToNode(path, index);
	XML_NODE* result = SearchForAttribute(start_node, tag_name, attrib, attrib_value_pattern);
	return	result;
}

XML_NODE* CXml::SearchForAttribute(XML_NODE* start_node, pcstr tag_name, pcstr attrib, pcstr attrib_value_pattern)
{
	while (start_node)
	{
		TiXmlElement* el = start_node->ToElement( );
		if (el)
		{
			pcstr attribStr = el->Attribute(attrib);
			pcstr valueStr = el->Value( );

			if (attribStr && 0 == xr_strcmp(attribStr, attrib_value_pattern) &&
				valueStr && 0 == xr_strcmp(valueStr, tag_name))
			{
				return el;
			}
		}

		XML_NODE* newEl = start_node->FirstChild(tag_name);
		newEl = SearchForAttribute(newEl, tag_name, attrib, attrib_value_pattern);
		if (newEl)
		{
			return newEl;
		}

		start_node = start_node->NextSibling(tag_name);
	}

	return NULL;
}

#ifdef DEBUG
pcstr CXml::CheckUniqueAttrib(XML_NODE* start_node, pcstr tag_name, pcstr attrib_name)
{
	m_AttribValues.clear_not_free( );

	s32 tags_num = GetNodesNum(start_node, tag_name);
	for (s32 i = 0; i < tags_num; i++)
	{
		pcstr attrib = ReadAttrib(start_node, tag_name, i, attrib_name, NULL);
		xr_vector<shared_str>::iterator it = std::find(m_AttribValues.begin( ), m_AttribValues.end( ), attrib);
		if (m_AttribValues.end( ) != it)
		{
			return attrib;
		}

		m_AttribValues.push_back(attrib);
	}

	return NULL;
}
#endif // def DEBUG

BOOL APIENTRY DllMain(HANDLE hModule, u32 ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
		}
		break;
		case DLL_PROCESS_DETACH:
		{
		}
		break;
	}

	return TRUE;
}
