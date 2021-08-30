//	Module 		: TradeBoolParameters.h
//	Description : trade bool parameters class

#pragma once

class CTradeBoolParameters
{
public:
	using SECTIONS = xr_vector<CSharedString>;

private:
	SECTIONS	m_sections;

public:
	inline			CTradeBoolParameters( );
	inline void	clear( );
	inline void	disable(const CSharedString& section);
	inline bool	disabled(const CSharedString& section) const;
};

#include "TradeBoolParameters_inline.h"
