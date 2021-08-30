//	Module 		: TradeActionParameters.h
//	Description : trade action parameters class

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
	inline						CTradeActionParameters(const CTradeFactors& default_factors = CTradeFactors( ));
	inline void				clear( );
	inline void				enable(const CSharedString& section, const CTradeFactors& trade_factors);
	inline void				disable(const CSharedString& section);
	inline bool				enabled(const CSharedString& section) const;
	inline bool				disabled(const CSharedString& section) const;
	inline const CTradeFactors& factors(const CSharedString& section) const;
	inline const CTradeFactors& default_factors( ) const;
	inline void				default_factors(const CTradeFactors& trade_factors);
};

#include "TradeActionParameters_inline.h"
