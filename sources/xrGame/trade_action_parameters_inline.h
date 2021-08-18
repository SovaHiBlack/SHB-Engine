////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_action_parameters_inline.h
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade action parameters class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CTradeActionParameters::CTradeActionParameters(const CTradeFactors& default_factors)
{
	m_default = default_factors;
}

inline	void CTradeActionParameters::clear( )
{
	m_enabled.clear( );
	m_disabled.clear( );
}

inline	void CTradeActionParameters::enable(const CSharedString& section, const CTradeFactors& trade_factors)
{
	m_enabled.enable(section, trade_factors);
}

inline	void CTradeActionParameters::disable(const CSharedString& section)
{
	m_disabled.disable(section);
}

inline	bool CTradeActionParameters::enabled(const CSharedString& section) const
{
	return m_enabled.enabled(section);
}

inline	bool CTradeActionParameters::disabled(const CSharedString& section) const
{
	return m_disabled.disabled(section);
}

inline	const CTradeFactors& CTradeActionParameters::factors(const CSharedString& section) const
{
	return m_enabled.factors(section);
}

inline	const CTradeFactors& CTradeActionParameters::default_factors( ) const
{
	return m_default;
}

inline	void CTradeActionParameters::default_factors(const CTradeFactors& trade_factors)
{
	m_default = trade_factors;
}
