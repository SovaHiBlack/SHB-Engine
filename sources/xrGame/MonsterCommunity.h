// MonsterCommunity.h:	структура представления группировки для монстров

#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"

using MonsterCommunityIndex = int;
using MonsterCommunityId = CSharedString;
#define NO_MONSTER_COMMUNITY_INDEX MonsterCommunityIndex(-1)

struct SMonsterCommunityData
{
	SMonsterCommunityData(MonsterCommunityIndex, MonsterCommunityId, const char*);

	MonsterCommunityId		id;
	MonsterCommunityIndex	index;
	unsigned char			team;
};

class CMonsterCommunity;

class CMonsterCommunity : public CIni_IdToIndex<1, SMonsterCommunityData, MonsterCommunityId, MonsterCommunityIndex, CMonsterCommunity>
{
private:
	using inherited = CIni_IdToIndex<1, SMonsterCommunityData, MonsterCommunityId, MonsterCommunityIndex, CMonsterCommunity>;
	friend inherited;

public:
	CMonsterCommunity( );
	~CMonsterCommunity( );

	void						set(MonsterCommunityId);
	void						set(MonsterCommunityIndex);

	MonsterCommunityId			id( ) const;
	MonsterCommunityIndex		index( ) const;
	unsigned char				team( ) const;

private:
	MonsterCommunityIndex		m_current_index;

	static void					InitIdToIndex( );

public:
	// отношение между группами монстров
	static int					relation(MonsterCommunityIndex from, MonsterCommunityIndex to);
	int							relation(MonsterCommunityIndex to);

	static void					DeleteIdToIndexData( );

private:
	using MonsterRelationTable = CIni_Table<int, CMonsterCommunity>;
	friend MonsterRelationTable;
	static MonsterRelationTable m_relation_table;
};
