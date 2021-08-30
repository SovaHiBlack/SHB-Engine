// character_rank.cpp:	структура представления рангов и отношений между ними

#include "stdafx.h"

#include "CharacterRank.h"

#define GAME_RELATIONS_SECT		"game_relations"
#define RANK_LINE				"rating"
#define RANK_TABLE				"rank_relations"
#define RANK_KILL_TABLE_SECT	"rank_kill_points"

SRankData::SRankData(int idx, CSharedString idn, const char* threshold_str)
{
	index = idx;
	id = idn;
	threshold = (CharacterRankValue) atoi(threshold_str);
}

CCharacterRank::GoodwillTable		CCharacterRank::m_relation_table;
CCharacterRank::RankKillTable		CCharacterRank::m_rank_kill_table;

int CCharacterRank::ValueToIndex(CharacterRankValue val)
{
	for (int i = 0; i < (int) m_pItemDataVector->size( ); i++)
	{
		if (val < (*m_pItemDataVector)[i].threshold)
		{
			return i;
		}
	}

	return GetMaxIndex( );
}

void CCharacterRank::set(CharacterRankValue new_val)
{
	m_current_value = new_val;
	m_current_index = ValueToIndex(new_val);
}

CSharedString					CCharacterRank::id( ) const
{
	return IndexToId(m_current_index);
}

void CCharacterRank::InitIdToIndex( )
{
	section_name = GAME_RELATIONS_SECT;
	line_name = RANK_LINE;
	m_relation_table.set_table_params(RANK_TABLE);
	m_rank_kill_table.set_table_params(RANK_KILL_TABLE_SECT, 1);
}

CharacterGoodwill CCharacterRank::relation(int to)
{
	return relation(m_current_index, to);
}

CharacterGoodwill CCharacterRank::relation(int from, int to)
{
	VERIFY(from >= 0 && from < (int) m_relation_table.table( ).size( ));
	VERIFY(to >= 0 && to < (int) m_relation_table.table( ).size( ));

	return m_relation_table.table( )[from][to];
}

CharacterRankValue CCharacterRank::rank_kill_points(int rank_index)
{
	return m_rank_kill_table.table( )[rank_index][0];
}

void CCharacterRank::DeleteIdToIndexData( )
{
	m_relation_table.clear( );
	m_rank_kill_table.clear( );
	inherited::DeleteIdToIndexData( );
}
