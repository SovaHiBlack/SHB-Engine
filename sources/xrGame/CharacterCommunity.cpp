// CharacterCommunity.cpp:		структура представления группировки

#include "stdafx.h"

#include "CharacterCommunity.h"

SCommunityData::SCommunityData(CharacterCommunityIndex idx, CharacterCommunityId idn, const char* team_str)
{
	index = idx;
	id = idn;
	team = (unsigned char) atoi(team_str);
}

CCharacterCommunity::GoodwillTable CCharacterCommunity::m_relation_table;
CCharacterCommunity::SympathyTable CCharacterCommunity::m_sympathy_table;

CCharacterCommunity::CCharacterCommunity( )
{
	m_current_index = NO_COMMUNITY_INDEX;
}

CCharacterCommunity::~CCharacterCommunity( )
{ }

void CCharacterCommunity::set(CharacterCommunityId id)
{
	m_current_index = IdToIndex(id);
}

CharacterCommunityId CCharacterCommunity::id( ) const
{
	return IndexToId(m_current_index);
}

unsigned char CCharacterCommunity::team( ) const
{
	return (*m_pItemDataVector)[m_current_index].team;
}

void CCharacterCommunity::InitIdToIndex( )
{
	section_name = "game_relations";
	line_name = "communities";

	m_relation_table.set_table_params("communities_relations");
	m_sympathy_table.set_table_params("communities_sympathy", 1);
}

CharacterGoodwill CCharacterCommunity::relation(CharacterCommunityIndex to)
{
	return relation(m_current_index, to);
}

CharacterGoodwill CCharacterCommunity::relation(CharacterCommunityIndex from, CharacterCommunityIndex to)
{
	VERIFY(from >= 0 && from < (int) m_relation_table.table( ).size( ));
	VERIFY(to >= 0 && to < (int) m_relation_table.table( ).size( ));

	return m_relation_table.table( )[from][to];
}

float CCharacterCommunity::sympathy(CharacterCommunityIndex comm)
{
	VERIFY(comm >= 0 && comm < (int) m_sympathy_table.table( ).size( ));
	return m_sympathy_table.table( )[comm][0];
}

void CCharacterCommunity::DeleteIdToIndexData( )
{
	m_relation_table.clear( );
	m_sympathy_table.clear( );
	inherited::DeleteIdToIndexData( );
}
