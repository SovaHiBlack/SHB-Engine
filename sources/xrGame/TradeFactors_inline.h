////////////////////////////////////////////////////////////////////////////
//	Module 		: TradeFactors_inline.h
//	Description : trade factors class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CTradeFactors::CTradeFactors(const f32& friend_factor, const f32& enemy_factor)
{
	VERIFY(_valid(friend_factor));
	m_friend_factor = friend_factor;

	VERIFY(_valid(enemy_factor));
	m_enemy_factor = enemy_factor;
}

IC const f32& CTradeFactors::friend_factor( ) const
{
	VERIFY(_valid(m_friend_factor));
	return m_friend_factor;
}

IC const f32& CTradeFactors::enemy_factor( ) const
{
	VERIFY(_valid(m_enemy_factor));
	return m_enemy_factor;
}
