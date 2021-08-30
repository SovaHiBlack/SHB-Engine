//	Module 		: TradeFactorParameters.h
//	Description : CTradeFactorParameters class

#pragma once

#include "associative_vector.h"
#include "TradeFactors.h"

class CTradeFactorParameters
{
public:
	using FACTORS = associative_vector<CSharedString, CTradeFactors>;

private:
	FACTORS					m_factors;

public:
	inline						CTradeFactorParameters( );
	inline void				clear( );
	inline void				enable(const CSharedString& section, const CTradeFactors& factors);
	inline bool				enabled(const CSharedString& section) const;
	inline const CTradeFactors& factors(const CSharedString& section) const;
};

#include "TradeFactorParameters_inline.h"
