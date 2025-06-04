////////////////////////////////////////////////////////////////////////////
//	Module 		: TradeBoolParameters.h
//	Description : trade bool parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CTradeBoolParameters
{
public:
	typedef xr_vector<shared_str>	SECTIONS;

private:
	SECTIONS	m_sections;

public:
	IC			CTradeBoolParameters( );
	IC	void	clear( );
	IC	void	disable(const shared_str& section);
	IC	bool	disabled(const shared_str& section) const;
};

#include "TradeBoolParameters_inline.h"
