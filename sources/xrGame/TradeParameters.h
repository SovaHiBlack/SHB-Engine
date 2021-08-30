//	Module 		: TradeParameters.h
//	Description : trade parameters class

#pragma once

#include "TradeActionParameters.h"

class CTradeParameters
{
public:
	struct _buy_parameters
	{ };
	typedef _buy_parameters* action_buy;

	struct _sell_parameters
	{ };
	typedef _sell_parameters* action_sell;

	struct _show_parameters
	{ };
	typedef _show_parameters* action_show;

private:
	static CTradeParameters* m_instance;

	CTradeActionParameters		m_buy;
	CTradeActionParameters		m_sell;
	CTradeBoolParameters		m_show;

	inline const CTradeActionParameters& action(action_buy) const;
	inline const CTradeActionParameters& action(action_sell) const;
	inline const CTradeBoolParameters& action(action_show) const;
	inline CTradeActionParameters& action(action_buy);
	inline CTradeActionParameters& action(action_sell);
	inline CTradeBoolParameters& action(action_show);

public:
	inline									CTradeParameters(const CSharedString& section = "trade");
	inline void							clear( );

	inline static CTradeParameters& instance( );
	inline static void						clean( );

	template <typename _action_type>
	inline bool							enabled(_action_type type, const CSharedString& section) const;

	template <typename _action_type>
	inline const CTradeFactors& factors(_action_type type, const CSharedString& section) const;

	template <typename _action_type>
	inline void							process(_action_type type, CIniFile& ini_file, const CSharedString& section);
	void							process(action_show, CIniFile& ini_file, const CSharedString& section);

	template <typename _action_type>
	inline void							default_factors(_action_type type, const CTradeFactors& trade_factors);
};

inline CTradeParameters& default_trade_parameters( );

#include "TradeParameters_inline.h"
