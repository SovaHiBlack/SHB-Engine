#pragma once

#define CONFIG_PATH "$game_config$"
#define UI_PATH "ui"

#include "tinyxml.h"

using XML_NODE = TiXmlNode;
using XML_ATTRIBUTE = TiXmlAttribute;

class CORE_API CXml
{
public:
	string_path			m_xml_file_name;
	CXml( );
	virtual				~CXml( );
	void				ClearInternal( );

	bool 				Init(Pcstr path_alias, Pcstr path, Pcstr xml_filename);
	bool 				Init(Pcstr path_alias, Pcstr xml_filename);

	// чтение элементов
	const char* Read(Pcstr path, int index, Pcstr default_str_val);
	const char* Read(XML_NODE* start_node, Pcstr path, int index, Pcstr default_str_val);
	const char* Read(XML_NODE* node, Pcstr default_str_val);

	int   				ReadInt(Pcstr path, int index, int default_int_val);
	int   				ReadInt(XML_NODE* start_node, Pcstr path, int index, int default_int_val);
	int   				ReadInt(XML_NODE* node, int default_int_val);

	float   			ReadFlt(Pcstr path, int index, float default_flt_val);
	float   			ReadFlt(XML_NODE* start_node, Pcstr path, int index, float default_flt_val);
	float   			ReadFlt(XML_NODE* node, float default_flt_val);

	const char* ReadAttrib(Pcstr path, int index, Pcstr attrib, Pcstr default_str_val = "");
	const char* ReadAttrib(XML_NODE* start_node, Pcstr path, int index, Pcstr attrib, Pcstr default_str_val = "");
	const char* ReadAttrib(XML_NODE* node, Pcstr attrib, Pcstr default_str_val);

	int					ReadAttribInt(Pcstr path, int index, Pcstr attrib, int default_int_val = 0);
	int					ReadAttribInt(XML_NODE* start_node, Pcstr path, int index, Pcstr attrib, int default_int_val = 0);
	int					ReadAttribInt(XML_NODE* node, Pcstr attrib, int default_int_val);

	float   			ReadAttribFlt(Pcstr path, int index, Pcstr attrib, float default_flt_val = 0.0f);
	float   			ReadAttribFlt(XML_NODE* start_node, Pcstr path, int index, Pcstr attrib, float default_flt_val = 0.0f);
	float   			ReadAttribFlt(XML_NODE* node, Pcstr attrib, float default_flt_val = 0.0f);

	XML_NODE* SearchForAttribute(Pcstr path, int index, Pcstr tag_name, Pcstr attrib, Pcstr attrib_value_pattern);
	XML_NODE* SearchForAttribute(XML_NODE* start_node, Pcstr tag_name, Pcstr attrib, Pcstr attrib_value_pattern);

	//возвращает количество узлов с заданым именем
	int					GetNodesNum(Pcstr path, int index, Pcstr tag_name);
	int					GetNodesNum(XML_NODE* node, Pcstr tag_name);


#ifdef DEBUG // debug & mixed
	//проверка того, что аттрибуты у тегов уникальны
	//(если не NULL, то уникальность нарушена и возврашается имя 
	//повторяющегося атрибута)
	const char* CheckUniqueAttrib(XML_NODE* start_node, Pcstr tag_name, Pcstr attrib_name);
#endif

	//переместиться по XML дереву 
	//путь задается в форме PARENT:CHILD:CHIDLS_CHILD
	//node_index - номер, если узлов с одним именем несколько
	XML_NODE* NavigateToNode(Pcstr path, int node_index = 0);
	XML_NODE* NavigateToNode(XML_NODE* start_node, Pcstr path, int node_index = 0);
	XML_NODE* NavigateToNodeWithAttribute(Pcstr tag_name, Pcstr attrib_name, Pcstr attrib_value);

	void				SetLocalRoot(XML_NODE* pLocalRoot)
	{
		m_pLocalRoot = pLocalRoot;
	}
	XML_NODE* GetLocalRoot( )
	{
		return m_pLocalRoot;
	}

	XML_NODE* GetRoot( )
	{
		return m_root;
	}

protected:
	XML_NODE* m_root;
	XML_NODE* m_pLocalRoot;

#ifdef DEBUG
	//буфферный вектор для проверки уникальность аттрибутов
	xr_vector<shared_str> m_AttribValues;
#endif // def DEBUG

public:
	virtual shared_str correct_file_name(Pcstr path, Pcstr fn)
	{
		return fn;
	}

private:
	CXml(const CXml& copy);
	void				operator=				(const CXml& copy);

	using XML_ELEM = TiXmlElement;
	TiXmlDocument								m_Doc;
};
