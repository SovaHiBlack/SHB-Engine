////////////////////////////////////////////////////////////////////////////
//	Module 		: TradeFactorParameters.h
//	Description : trade factor parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "associative_vector.h"
#include "TradeFactors.h"

class CTradeFactorParameters
{
public:
	typedef associative_vector<shared_str, CTradeFactors>	FACTORS;

private:
	FACTORS					m_factors;

public:
	IC						CTradeFactorParameters( );
	IC	void				clear( );
	IC	void				enable(const shared_str& section, const CTradeFactors& factors);
	IC	bool				enabled(const shared_str& section) const;
	IC	const CTradeFactors& factors(const shared_str& section) const;
};

#include "TradeFactorParameters_inline.h"
