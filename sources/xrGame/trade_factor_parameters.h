////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_factor_parameters.h
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade factor parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "associative_vector.h"
#include "trade_factors.h"

class CTradeFactorParameters {
public:
	typedef associative_vector<CSharedString,CTradeFactors>	FACTORS;

private:
	FACTORS					m_factors;

public:
	inline						CTradeFactorParameters	();
	inline	void				clear					();
	inline	void				enable					(const CSharedString& section, const CTradeFactors &factors);
	inline	bool				enabled					(const CSharedString& section) const;
	inline	const CTradeFactors	&factors				(const CSharedString& section) const;
};

#include "trade_factor_parameters_inline.h"
