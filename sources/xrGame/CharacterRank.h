//////////////////////////////////////////////////////////////////////////
// CharacterRank.h:	��������� ������������� ������ � ��������� ����� ����

#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"
#include "CharacterInfo_defs.h"

struct SCharacterRankData
{
	SCharacterRankData(s32, shared_str, pcstr);

	shared_str				id;
	s32						index;
	CHARACTER_RANK_VALUE	threshold;
};

class CCharacterRank;

typedef CIni_IdToIndex<1, SCharacterRankData, shared_str, s32, CCharacterRank> CHARACTER_RANK_base;

class CCharacterRank : public CHARACTER_RANK_base
{
private:
	typedef CHARACTER_RANK_base inherited;
	friend inherited;

public:
	CCharacterRank( ) : m_current_value(NO_RANK)
	{ }
	~CCharacterRank( )
	{ }

	void						set(CHARACTER_RANK_VALUE);

	shared_str					id( ) const;
	s32							index( ) const
	{
		return m_current_index;
	}
	CHARACTER_RANK_VALUE		value( ) const
	{
		return m_current_value;
	}

	static s32					ValueToIndex(CHARACTER_RANK_VALUE);

private:
	CHARACTER_RANK_VALUE		m_current_value;
	s32							m_current_index;

	static	void				InitIdToIndex( );

public:
	//��������� ����� �������
	static CHARACTER_GOODWILL	relation(s32 from, s32 to);
	CHARACTER_GOODWILL			relation(s32 to);

	static CHARACTER_RANK_VALUE	rank_kill_points(s32);

	static void					DeleteIdToIndexData( );

private:
	typedef CIni_Table<CHARACTER_GOODWILL, CCharacterRank> GOODWILL_TABLE;
	friend GOODWILL_TABLE;
	static GOODWILL_TABLE m_relation_table;

	//���� �������� ������� ������������ �� �������� ��������� � ������������ ������
	typedef CIni_Table<CHARACTER_RANK_VALUE, CCharacterRank> RANK_KILL_TABLE;
	friend RANK_KILL_TABLE;
	static RANK_KILL_TABLE m_rank_kill_table;
};
