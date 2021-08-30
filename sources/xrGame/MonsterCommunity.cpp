// MonsterCommunity.cpp: структура представления группировки для монстров

#include "stdafx.h"

#include "MonsterCommunity.h"

#define MONSTER_RELATIONS_SECT			"monster_communities"
#define MONSTER_COMMUNITIES				"communities"
#define MONSTER_RELATIONS_TABLE			"monster_relations"

SMonsterCommunityData::SMonsterCommunityData(MonsterCommunityIndex idx, MonsterCommunityId idn, const char* team_str)
{
	index = idx;
	id = idn;
	team = (U8) atoi(team_str);
}

CMonsterCommunity::MonsterRelationTable CMonsterCommunity::m_relation_table;

CMonsterCommunity::CMonsterCommunity( )
{
	m_current_index = NO_MONSTER_COMMUNITY_INDEX;
}

CMonsterCommunity::~CMonsterCommunity( )
{ }

void CMonsterCommunity::set(MonsterCommunityId id)
{
	m_current_index = IdToIndex(id);
}

void CMonsterCommunity::set(MonsterCommunityIndex index)
{
	m_current_index = index;
}

MonsterCommunityId CMonsterCommunity::id( ) const
{
	return IndexToId(m_current_index);
}

MonsterCommunityIndex CMonsterCommunity::index( ) const
{
	return m_current_index;
}

U8 CMonsterCommunity::team( ) const
{
	return (*m_pItemDataVector)[m_current_index].team;
}

void CMonsterCommunity::InitIdToIndex( )
{
	section_name = MONSTER_RELATIONS_SECT;
	line_name = MONSTER_COMMUNITIES;
	m_relation_table.set_table_params(MONSTER_RELATIONS_TABLE);
}

int CMonsterCommunity::relation(MonsterCommunityIndex to)
{
	return relation(m_current_index, to);
}

int CMonsterCommunity::relation(MonsterCommunityIndex from, MonsterCommunityIndex to)
{
	VERIFY(from >= 0 && from < (int) m_relation_table.table( ).size( ));
	VERIFY(to >= 0 && to < (int) m_relation_table.table( ).size( ));

	return m_relation_table.table( )[from][to];
}

void CMonsterCommunity::DeleteIdToIndexData( )
{
	m_relation_table.clear( );
	inherited::DeleteIdToIndexData( );
}
