////////////////////////////////////////////////////////////////////////////
//	Module 		: TradeActionParametersTradeActionParameters.h
//	Description : trade action parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TradeFactorParameters.h"
#include "TradeBoolParameters.h"

class CTradeActionParameters
{
private:
	CTradeFactorParameters	m_enabled;
	CTradeBoolParameters	m_disabled;
	CTradeFactors			m_default;

public:
	IC						CTradeActionParameters(const CTradeFactors& default_factors = CTradeFactors( ));
	IC	void				clear( );
	IC	void				enable(const shared_str& section, const CTradeFactors& trade_factors);
	IC	void				disable(const shared_str& section);
	IC	bool				enabled(const shared_str& section) const;
	IC	bool				disabled(const shared_str& section) const;
	IC	const CTradeFactors& factors(const shared_str& section) const;
	IC	const CTradeFactors& default_factors( ) const;
	IC	void				default_factors(const CTradeFactors& trade_factors);
};

#include "TradeActionParameters_inline.h"
