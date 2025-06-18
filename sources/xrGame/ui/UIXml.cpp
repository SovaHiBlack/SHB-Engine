// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: UIXml.cpp
// Author		: Anahoret
// Description	: 
// ========================================= SovaHiBlack© =========================================

#include "stdafx.h"
#pragma hdrstop

#include "UIXml.h"

#ifdef XRGAME_EXPORTS
#include "ui_base.h"
#endif

shared_str CUIXml::correct_file_name(pcstr path, pcstr fn)
{

#ifdef XRGAME_EXPORTS
	if (0 == xr_strcmp(path, "ui") || 0 == xr_strcmp(path, "UI"))
	{
		return UI( )->get_xml_name(fn);
	}
	else
	{
		return fn;
	}
#else
	return fn;
#endif

}

//#define LOG_ALL_XMLS
#ifdef LOG_ALL_XMLS
s32 ListXmlCount = 0;
struct SDBGList
{
	s32 num;
	bool closed;
};

xr_vector<SDBGList>	dbg_list_xmls;
void dump_list_xmls( )
{
	Msg("------Total  xmls %d", dbg_list_xmls.size( ));
	xr_vector<SDBGList>::iterator _it = dbg_list_xmls.begin( );
	for (; _it != dbg_list_xmls.end( ); ++_it)
	{
		if (!(*_it).closed)
		{
			Msg("--leak detected ---- xml = %d", (*_it).num);
		}
	}
}
#else
void dump_list_xmls( )
{ }
#endif

CUIXml::CUIXml( )
{

#ifdef LOG_ALL_XMLS
	ListXmlCount++;
	m_dbg_id = ListXmlCount;
	dbg_list_xmls.push_back(SDBGList( ));
	dbg_list_xmls.back( ).num = m_dbg_id;
	dbg_list_xmls.back( ).closed = false;
#endif

}

CUIXml::~CUIXml( )
{

#ifdef LOG_ALL_XMLS
	xr_vector<SDBGList>::iterator _it = dbg_list_xmls.begin( );
	bool bOK = false;
	for (; _it != dbg_list_xmls.end( ); ++_it)
	{
		if ((*_it).num == m_dbg_id && !(*_it).closed)
		{
			bOK = true;
			(*_it).closed = true;
			dbg_list_xmls.erase(_it);
			break;
		}

		if ((*_it).num == m_dbg_id && (*_it).closed)
		{
			Msg("--XML [%d] already deleted", m_dbg_id);
			bOK = true;
		}
	}

	if (!bOK)
	{
		Msg("CUIXml::~CUIXml.[%d] cannot find xml in list", m_dbg_id);
	}
#endif

}
