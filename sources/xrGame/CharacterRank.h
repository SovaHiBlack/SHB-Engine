// CharacterRank.h:	структура представления рангов и отношений между ними

#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"
#include "CharacterInfo_defs.h"

struct SRankData
{
	SRankData(int, CSharedString, const char*);

	CSharedString			id;
	int						index;
	CharacterRankValue		threshold;
};

class CCharacterRank;

using CharacterRankBase = CIni_IdToIndex<1, SRankData, CSharedString, int, CCharacterRank>;

class CCharacterRank : public CharacterRankBase
{
private:
	using inherited = CharacterRankBase;
	friend inherited;

public:
	CCharacterRank( ) : m_current_value(NO_RANK)
	{ }
	~CCharacterRank( )
	{ }

	void						set(CharacterRankValue);

	CSharedString				id( ) const;
	int							index( ) const
	{
		return m_current_index;
	}
	CharacterRankValue			value( ) const
	{
		return m_current_value;
	}

	static int					ValueToIndex(CharacterRankValue);

private:
	CharacterRankValue		m_current_value;
	int							m_current_index;

	static	void				InitIdToIndex( );

public:
	// отношение между рангами
	static CharacterGoodwill	relation(int from, int to);
	CharacterGoodwill			relation(int to);

	static CharacterRankValue	rank_kill_points(int);

	static void					DeleteIdToIndexData( );

private:
	using GoodwillTable = CIni_Table<CharacterGoodwill, CCharacterRank>;
	friend GoodwillTable;
	static GoodwillTable m_relation_table;

	// очки рейтинга которые прибавляются за убийство персонажа с определенным рангом
	using RankKillTable = CIni_Table<CharacterRankValue, CCharacterRank>;
	friend RankKillTable;
	static RankKillTable m_rank_kill_table;
};
