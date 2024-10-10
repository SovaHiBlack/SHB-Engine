////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_factors.h
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade factors class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CTradeFactors {
private:
	f32			m_friend_factor;
	f32			m_enemy_factor;

public:
	IC				CTradeFactors	(const f32& friend_factor = 1.f, const f32& enemy_factor = 1.f);
	IC	const f32&	friend_factor	() const;
	IC	const f32&	enemy_factor	() const;
};

#include "trade_factors_inline.h"