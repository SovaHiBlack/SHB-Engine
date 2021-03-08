////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_action_parameters.h
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade action parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "trade_factor_parameters.h"
#include "trade_bool_parameters.h"

class CTradeActionParameters {
private:
	CTradeFactorParameters	m_enabled;
	CTradeBoolParameters	m_disabled;
	CTradeFactors			m_default;

public:
	inline						CTradeActionParameters	(const CTradeFactors &default_factors = CTradeFactors());
	inline	void				clear					();
	inline	void				enable					(const shared_str &section, const CTradeFactors &trade_factors);
	inline	void				disable					(const shared_str &section);
	inline	bool				enabled					(const shared_str &section) const;
	inline	bool				disabled				(const shared_str &section) const;
	inline	const CTradeFactors	&factors				(const shared_str &section) const;
	inline	const CTradeFactors	&default_factors		() const;
	inline	void				default_factors			(const CTradeFactors &trade_factors);
};

#include "trade_action_parameters_inline.h"