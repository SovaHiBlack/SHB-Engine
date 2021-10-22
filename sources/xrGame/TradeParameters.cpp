//	Module 		: trade_parameters.cpp
//	Description : trade parameters class

#include "stdafx.h"

#include "TradeParameters.h"

CTradeParameters* CTradeParameters::m_instance = nullptr;

void CTradeParameters::process(action_show, CConfigurationFile& ini_file, const CSharedString& section)
{
	VERIFY(ini_file.section_exist(section));
	m_show.clear( );
	CConfigurationFile::Sect& S = ini_file.r_section(section);
	CConfigurationFile::SectCIt I = S.Data.begin( );
	CConfigurationFile::SectCIt E = S.Data.end( );
	for (; I != E; ++I)
	{
		if (!(*I).second.size( ))
		{
			m_show.disable((*I).first);
		}
	}
}
