// CharacterReputation.cpp:	структура представления репутаций и отношений между ними

#include "stdafx.h"

#include "CharacterReputation.h"

#define GAME_RELATIONS_SECT		"game_relations"
#define REPUTATION_LINE			"reputation"
#define REPUTATION_TABLE		"reputation_relations"

SReputationData::SReputationData(int idx, CSharedString idn, const char* threshold_str)
{
	index = idx;
	id = idn;
	threshold = (CharacterReputationValue) atoi(threshold_str);
}

CCharacterReputation::GoodwillTable CCharacterReputation::m_relation_table;

int CCharacterReputation::ValueToIndex(CharacterReputationValue val)
{
	T_VECTOR::iterator it = m_pItemDataVector->begin( );
	T_VECTOR::iterator it_e = m_pItemDataVector->end( );
	for (; it != it_e; ++it)
	{
		if (val < (*it).threshold)
		{
			return (int) std::distance(m_pItemDataVector->begin( ), it);
		}
	}

	return inherited::GetMaxIndex( );
}

void CCharacterReputation::set(CharacterReputationValue new_val)
{
	m_current_value = new_val;
	m_current_index = ValueToIndex(new_val);
}

CSharedString CCharacterReputation::id( ) const
{
	return IndexToId(m_current_index);
}

void CCharacterReputation::InitIdToIndex( )
{
	section_name = GAME_RELATIONS_SECT;
	line_name = REPUTATION_LINE;

	m_relation_table.set_table_params(REPUTATION_TABLE);
}

CharacterGoodwill CCharacterReputation::relation(int to)
{
	return relation(m_current_index, to);
}

CharacterGoodwill CCharacterReputation::relation(int from, int to)
{
	VERIFY(from >= 0 && from < (int) m_relation_table.table( ).size( ));
	VERIFY(to >= 0 && to < (int) m_relation_table.table( ).size( ));
	return m_relation_table.table( )[from][to];
}

void CCharacterReputation::DeleteIdToIndexData( )
{
	m_relation_table.clear( );
	inherited::DeleteIdToIndexData( );
}
