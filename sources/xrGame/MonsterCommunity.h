//////////////////////////////////////////////////////////////////////////
// MonsterCommunity.h:	структура представления группировки для монстров

#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"

typedef s32			MONSTER_COMMUNITY_INDEX;
typedef shared_str	MONSTER_COMMUNITY_ID;
#define NO_MONSTER_COMMUNITY_INDEX MONSTER_COMMUNITY_INDEX(-1)

struct SMonsterCommunityData
{
	SMonsterCommunityData(MONSTER_COMMUNITY_INDEX, MONSTER_COMMUNITY_ID, pcstr);

	MONSTER_COMMUNITY_ID	id;
	MONSTER_COMMUNITY_INDEX	index;
	u8						team;
};

class CMonsterCommunity;

class CMonsterCommunity : public CIni_IdToIndex<1, SMonsterCommunityData, MONSTER_COMMUNITY_ID, MONSTER_COMMUNITY_INDEX, CMonsterCommunity>
{
private:
	typedef CIni_IdToIndex<1, SMonsterCommunityData, MONSTER_COMMUNITY_ID, MONSTER_COMMUNITY_INDEX, CMonsterCommunity> inherited;
	friend inherited;

public:
	CMonsterCommunity( );
	~CMonsterCommunity( );

	void						set(MONSTER_COMMUNITY_ID);
	void						set(MONSTER_COMMUNITY_INDEX);

	MONSTER_COMMUNITY_ID		id( ) const;
	MONSTER_COMMUNITY_INDEX		index( ) const;
	u8							team( ) const;

private:
	MONSTER_COMMUNITY_INDEX		m_current_index;

	static void				InitIdToIndex( );

public:
	//отношение между группами монстров
	static s32					relation(MONSTER_COMMUNITY_INDEX from, MONSTER_COMMUNITY_INDEX to);
	s32							relation(MONSTER_COMMUNITY_INDEX to);

	static void					DeleteIdToIndexData( );

private:
	typedef CIni_Table<s32, CMonsterCommunity> MONSTER_RELATION_TABLE;
	friend MONSTER_RELATION_TABLE;
	static MONSTER_RELATION_TABLE m_relation_table;
};
