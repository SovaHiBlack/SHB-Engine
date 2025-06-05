//////////////////////////////////////////////////////////////////////////
// CharacterReputation.cpp:	структура представления репутаций и 
//								отношений между ними		
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CharacterReputation.h"

#define GAME_RELATIONS_SECT		"game_relations"
#define REPUTATION_LINE			"reputation"
#define REPUTATION_TABLE		"reputation_relations"

//////////////////////////////////////////////////////////////////////////
SCharacterReputationData::SCharacterReputationData(s32 idx, shared_str idn, pcstr threshold_str)
{
	index = idx;
	id = idn;
	threshold = (CHARACTER_REPUTATION_VALUE) atoi(threshold_str);
}
//////////////////////////////////////////////////////////////////////////
CCharacterReputation::GOODWILL_TABLE CCharacterReputation::m_relation_table;

//////////////////////////////////////////////////////////////////////////

s32 CCharacterReputation::ValueToIndex(CHARACTER_REPUTATION_VALUE val)
{
	T_VECTOR::iterator it = m_pItemDataVector->begin( );
	T_VECTOR::iterator it_e = m_pItemDataVector->end( );
	for (; it != it_e; ++it)
	{
		if (val < (*it).threshold)
		{
			return (s32) std::distance(m_pItemDataVector->begin( ), it);
		}
	}

	return inherited::GetMaxIndex( );
}

void CCharacterReputation::set(CHARACTER_REPUTATION_VALUE new_val)
{
	m_current_value = new_val;
	m_current_index = ValueToIndex(new_val);
}

shared_str					CCharacterReputation::id( ) const
{
	return IndexToId(m_current_index);
}

void CCharacterReputation::InitIdToIndex( )
{
	section_name = GAME_RELATIONS_SECT;
	line_name = REPUTATION_LINE;

	m_relation_table.set_table_params(REPUTATION_TABLE);
}

CHARACTER_GOODWILL CCharacterReputation::relation(s32 to)
{
	return relation(m_current_index, to);
}

CHARACTER_GOODWILL CCharacterReputation::relation(s32 from, s32 to)
{
	VERIFY(from >= 0 && from < (s32) m_relation_table.table( ).size( ));
	VERIFY(to >= 0 && to < (s32) m_relation_table.table( ).size( ));
	return m_relation_table.table( )[from][to];
}

void CCharacterReputation::DeleteIdToIndexData( )
{
	m_relation_table.clear( );
	inherited::DeleteIdToIndexData( );
}
