////////////////////////////////////////////////////////////////////////////
//	Module 		: TradeParameters.h
//	Description : trade parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TradeActionParameters.h"

class CTradeParameters
{
public:
	struct _buy_parameters
	{ };
	typedef _buy_parameters* action_buy;

public:
	struct _sell_parameters
	{ };
	typedef _sell_parameters* action_sell;

public:
	struct _show_parameters
	{ };
	typedef _show_parameters* action_show;

private:
	static CTradeParameters* m_instance;

private:
	CTradeActionParameters		m_buy;
	CTradeActionParameters		m_sell;
	CTradeBoolParameters		m_show;

private:
	IC	const CTradeActionParameters& action(action_buy) const;
	IC	const CTradeActionParameters& action(action_sell) const;
	IC	const CTradeBoolParameters& action(action_show) const;
	IC	CTradeActionParameters& action(action_buy);
	IC	CTradeActionParameters& action(action_sell);
	IC	CTradeBoolParameters& action(action_show);

public:
	IC									CTradeParameters(const shared_str& section = "trade");
	IC	void							clear( );

public:
	IC	static CTradeParameters& instance( );
	IC	static void						clean( );

public:
	template <typename _action_type>
	IC	bool							enabled(_action_type type, const shared_str& section) const;

	template <typename _action_type>
	IC	const CTradeFactors& factors(_action_type type, const shared_str& section) const;

	template <typename _action_type>
	IC	void							process(_action_type type, CIniFile& ini_file, const shared_str& section);
	void							process(action_show, CIniFile& ini_file, const shared_str& section);

	template <typename _action_type>
	IC	void							default_factors(_action_type type, const CTradeFactors& trade_factors);
};

IC	CTradeParameters& default_trade_parameters( );

#include "TradeParameters_inline.h"
