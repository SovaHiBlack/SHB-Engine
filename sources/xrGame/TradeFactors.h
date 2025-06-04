////////////////////////////////////////////////////////////////////////////
//	Module 		: TradeFactors.h
//	Description : trade factors class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CTradeFactors
{
private:
	f32			m_friend_factor;
	f32			m_enemy_factor;

public:
	IC				CTradeFactors(const f32& friend_factor = 1.0f, const f32& enemy_factor = 1.0f);
	IC	const f32& friend_factor( ) const;
	IC	const f32& enemy_factor( ) const;
};

#include "TradeFactors_inline.h"
