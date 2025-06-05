//////////////////////////////////////////////////////////////////////////
// CharacterCommunity.cpp:		структура представления группировки
//							
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CharacterCommunity.h"

//////////////////////////////////////////////////////////////////////////
SCharacterCommunityData::SCharacterCommunityData(CHARACTER_COMMUNITY_INDEX idx, CHARACTER_COMMUNITY_ID idn, pcstr team_str)
{
	index = idx;
	id = idn;
	team = (u8)atoi(team_str);
}

//////////////////////////////////////////////////////////////////////////
CCharacterCommunity::GOODWILL_TABLE CCharacterCommunity::m_relation_table;
CCharacterCommunity::SYMPATHY_TABLE CCharacterCommunity::m_sympathy_table;

//////////////////////////////////////////////////////////////////////////
CCharacterCommunity::CCharacterCommunity()
{
	m_current_index = NO_COMMUNITY_INDEX;
}
CCharacterCommunity::~CCharacterCommunity()
{
}


void  CCharacterCommunity::set	(CHARACTER_COMMUNITY_ID id)
{
	m_current_index	 = IdToIndex(id);

}

CHARACTER_COMMUNITY_ID		 CCharacterCommunity::id			() const
{
	return IndexToId(m_current_index);
}

u8							 CCharacterCommunity::team			() const
{
	return (*m_pItemDataVector)[m_current_index].team;
}


void CCharacterCommunity::InitIdToIndex	()
{
	section_name = "game_relations";
	line_name = "communities";

	m_relation_table.set_table_params("communities_relations");
	m_sympathy_table.set_table_params("communities_sympathy", 1);
}


CHARACTER_GOODWILL CCharacterCommunity::relation		(CHARACTER_COMMUNITY_INDEX to)
{
	return relation(m_current_index, to);
}

CHARACTER_GOODWILL  CCharacterCommunity::relation		(CHARACTER_COMMUNITY_INDEX from, CHARACTER_COMMUNITY_INDEX to)
{
	VERIFY(from >= 0 && from <(s32)m_relation_table.table().size());
	VERIFY(to >= 0 && to <(s32)m_relation_table.table().size());
	
	return m_relation_table.table()[from][to];
}

f32 CCharacterCommunity::sympathy			(CHARACTER_COMMUNITY_INDEX comm)
{
	VERIFY(comm >= 0 && comm <(s32)m_sympathy_table.table().size());
	return m_sympathy_table.table()[comm][0];
}

void CCharacterCommunity::DeleteIdToIndexData	()
{
	m_relation_table.clear();
	m_sympathy_table.clear();
	inherited::DeleteIdToIndexData();
}
