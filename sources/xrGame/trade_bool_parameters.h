////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_bool_parameters.h
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade bool parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CTradeBoolParameters {
public:
	typedef xr_vector<CSharedString>	SECTIONS;

private:
	SECTIONS	m_sections;

public:
	inline			CTradeBoolParameters	();
	inline	void	clear					();
	inline	void	disable					(const CSharedString& section);
	inline	bool	disabled				(const CSharedString& section) const;
};

#include "trade_bool_parameters_inline.h"
