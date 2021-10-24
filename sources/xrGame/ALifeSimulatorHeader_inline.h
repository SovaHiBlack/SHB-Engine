//	Module 		: ALifeSimulatorHeader_inline.h
//	Description : ALife Simulator header inline functions

#pragma once

inline CALifeSimulatorHeader::CALifeSimulatorHeader(const char* section)
{
	m_version = ALIFE_VERSION;
}

inline u32 CALifeSimulatorHeader::version( ) const
{
	return m_version;
}
