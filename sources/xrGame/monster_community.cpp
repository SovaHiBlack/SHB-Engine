//////////////////////////////////////////////////////////////////////////
// monster_community.cpp: структура представления группировки для монстров
//							
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "monster_community.h"

#define MONSTER_RELATIONS_SECT			"monster_communities"
#define MONSTER_COMMUNITIES				"communities"
#define MONSTER_RELATIONS_TABLE			"monster_relations"

//////////////////////////////////////////////////////////////////////////
MONSTER_COMMUNITY_DATA::MONSTER_COMMUNITY_DATA(MONSTER_COMMUNITY_INDEX idx, MONSTER_COMMUNITY_ID idn, pcstr team_str)
{
	index = idx;
	id = idn;
	team = (u8)atoi(team_str);
}

//////////////////////////////////////////////////////////////////////////
CMonsterCommunity::MONSTER_RELATION_TABLE CMonsterCommunity::m_relation_table;

//////////////////////////////////////////////////////////////////////////
CMonsterCommunity::CMonsterCommunity( )
{
	m_current_index = NO_MONSTER_COMMUNITY_INDEX;
}

CMonsterCommunity::~CMonsterCommunity( )
{ }

void CMonsterCommunity::set(MONSTER_COMMUNITY_ID id)
{
	m_current_index = IdToIndex(id);

}
void CMonsterCommunity::set(MONSTER_COMMUNITY_INDEX index)
{
	m_current_index = index;
}

MONSTER_COMMUNITY_ID		 CMonsterCommunity::id( ) const
{
	return IndexToId(m_current_index);
}

MONSTER_COMMUNITY_INDEX	 CMonsterCommunity::index( ) const
{
	return m_current_index;
}

u8 CMonsterCommunity::team( ) const
{
	return (*m_pItemDataVector)[m_current_index].team;
}

void CMonsterCommunity::InitIdToIndex( )
{
	section_name = MONSTER_RELATIONS_SECT;
	line_name = MONSTER_COMMUNITIES;
	m_relation_table.set_table_params(MONSTER_RELATIONS_TABLE);
}

s32 CMonsterCommunity::relation(MONSTER_COMMUNITY_INDEX to)
{
	return relation(m_current_index, to);
}

s32 CMonsterCommunity::relation(MONSTER_COMMUNITY_INDEX from, MONSTER_COMMUNITY_INDEX to)
{
	VERIFY(from >= 0 && from < (s32)m_relation_table.table( ).size( ));
	VERIFY(to >= 0 && to < (s32)m_relation_table.table( ).size( ));

	return m_relation_table.table( )[from][to];
}

void CMonsterCommunity::DeleteIdToIndexData( )
{
	m_relation_table.clear( );
	inherited::DeleteIdToIndexData( );
}
