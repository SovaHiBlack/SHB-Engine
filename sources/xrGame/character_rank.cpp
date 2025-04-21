//////////////////////////////////////////////////////////////////////////
// character_rank.cpp:	структура представления рангов и отношений между 
//						ними		
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_rank.h"

#define GAME_RELATIONS_SECT		"game_relations"
#define RANK_LINE				"rating"
#define RANK_TABLE				"rank_relations"
#define RANK_KILL_TABLE_SECT	"rank_kill_points"

//////////////////////////////////////////////////////////////////////////
RANK_DATA::RANK_DATA(s32 idx, shared_str idn, pcstr threshold_str)
{
	index = idx;
	id = idn;
	threshold = (CHARACTER_RANK_VALUE)atoi(threshold_str);
}
//////////////////////////////////////////////////////////////////////////
CHARACTER_RANK::GOODWILL_TABLE		CHARACTER_RANK::m_relation_table;
CHARACTER_RANK::RANK_KILL_TABLE		CHARACTER_RANK::m_rank_kill_table;

s32 CHARACTER_RANK::ValueToIndex(CHARACTER_RANK_VALUE val)
{
	for (s32 i = 0; i < (s32)m_pItemDataVector->size( ); i++)
	{
		if (val < (*m_pItemDataVector)[i].threshold)
		{
			return i;
		}
	}

	return GetMaxIndex( );
}

void CHARACTER_RANK::set(CHARACTER_RANK_VALUE new_val)
{
	m_current_value = new_val;
	m_current_index = ValueToIndex(new_val);
}

shared_str CHARACTER_RANK::id( ) const
{
	return IndexToId(m_current_index);
}

void CHARACTER_RANK::InitIdToIndex( )
{
	section_name = GAME_RELATIONS_SECT;
	line_name = RANK_LINE;
	m_relation_table.set_table_params(RANK_TABLE);
	m_rank_kill_table.set_table_params(RANK_KILL_TABLE_SECT, 1);
}

CHARACTER_GOODWILL CHARACTER_RANK::relation(s32 to)
{
	return relation(m_current_index, to);
}

CHARACTER_GOODWILL  CHARACTER_RANK::relation(s32 from, s32 to)
{
	VERIFY(from >= 0 && from < (s32)m_relation_table.table( ).size( ));
	VERIFY(to >= 0 && to < (s32)m_relation_table.table( ).size( ));

	return m_relation_table.table( )[from][to];
}

CHARACTER_RANK_VALUE CHARACTER_RANK::rank_kill_points(s32 rank_index)
{
	return m_rank_kill_table.table( )[rank_index][0];
}

void CHARACTER_RANK::DeleteIdToIndexData( )
{
	m_relation_table.clear( );
	m_rank_kill_table.clear( );
	inherited::DeleteIdToIndexData( );
}
