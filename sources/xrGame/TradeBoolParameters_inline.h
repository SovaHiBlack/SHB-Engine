//	Module 		: TradeBoolParameters_inline.h
//	Description : trade bool parameters class

#pragma once

inline CTradeBoolParameters::CTradeBoolParameters( )
{ }

inline void CTradeBoolParameters::clear( )
{
	m_sections.clear( );
}

inline void CTradeBoolParameters::disable(const CSharedString& section)
{
	SECTIONS::iterator I = std::find(m_sections.begin( ), m_sections.end( ), section);
	VERIFY(I == m_sections.end( ));
	m_sections.push_back(section);
}

inline bool CTradeBoolParameters::disabled(const CSharedString& section) const
{
	SECTIONS::const_iterator I = std::find(m_sections.begin( ), m_sections.end( ), section);
	return (I != m_sections.end( ));
}
