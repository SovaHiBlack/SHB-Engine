//	Module 		: TradeFactors.h
//	Description : trade factors class

#pragma once

class CTradeFactors
{
private:
	float			m_friend_factor;
	float			m_enemy_factor;

public:
	inline				CTradeFactors(const float& friend_factor = 1.0f, const float& enemy_factor = 1.0f);
	inline const float& friend_factor( ) const;
	inline const float& enemy_factor( ) const;
};

#include "TradeFactors_inline.h"
