// CharacterReputation.h:		структура представления репутаций и отношений между ними
#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"
#include "CharacterInfo_defs.h"

struct SReputationData
{
	SReputationData(int, CSharedString, const char*);

	CSharedString					id;
	int							index;
	CharacterReputationValue	threshold;
};

class CCharacterReputation;

class CCharacterReputation : public CIni_IdToIndex<1, SReputationData, CSharedString, int, CCharacterReputation>
{
private:
	using inherited = CIni_IdToIndex<1, SReputationData, CSharedString, int, CCharacterReputation>;
	friend inherited;

public:
	CCharacterReputation( ) : m_current_value(NO_REPUTATION)
	{ }
	~CCharacterReputation( )
	{ }

	void						set(CharacterReputationValue);

	CSharedString				id( ) const;
	int							index( ) const
	{
		return m_current_index;
	}
	CharacterReputationValue	value( ) const
	{
		return m_current_value;
	}

	static int					ValueToIndex(CharacterReputationValue);

private:
	CharacterReputationValue	m_current_value;
	int							m_current_index;

	static	void				InitIdToIndex( );

public:
	// отношение между репутациями
	static CharacterGoodwill	relation(int from, int to);
	CharacterGoodwill			relation(int to);

	static void					DeleteIdToIndexData( );

private:
	using GoodwillTable = CIni_Table<CharacterGoodwill, CCharacterReputation>;
	friend GoodwillTable;
	static GoodwillTable m_relation_table;
};
