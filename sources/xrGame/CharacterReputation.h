//////////////////////////////////////////////////////////////////////////
// CharacterReputation.h:		структура представления репутаций и 
//								отношений между ними		
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"
#include "CharacterInfo_defs.h"

struct SCharacterReputationData
{
	SCharacterReputationData(s32, shared_str, pcstr);

	shared_str					id;
	s32							index;
	CHARACTER_REPUTATION_VALUE	threshold;
};

class CCharacterReputation;

class CCharacterReputation : public CIni_IdToIndex<1, SCharacterReputationData, shared_str, s32, CCharacterReputation>
{
private:
	typedef CIni_IdToIndex<1, SCharacterReputationData, shared_str, s32, CCharacterReputation> inherited;
	friend inherited;

public:
	CCharacterReputation( ) : m_current_value(NO_REPUTATION)
	{ }
	~CCharacterReputation( )
	{ }

	void						set(CHARACTER_REPUTATION_VALUE);

	shared_str					id( ) const;
	s32							index( ) const
	{
		return m_current_index;
	}
	CHARACTER_REPUTATION_VALUE	value( ) const
	{
		return m_current_value;
	}

	static s32					ValueToIndex(CHARACTER_REPUTATION_VALUE);

private:
	CHARACTER_REPUTATION_VALUE	m_current_value;
	s32							m_current_index;

	static	void				InitIdToIndex( );

public:
	//отношение между репутациями
	static CHARACTER_GOODWILL	relation(s32 from, s32 to);
	CHARACTER_GOODWILL			relation(s32 to);

	static void					DeleteIdToIndexData( );

private:
	typedef CIni_Table<CHARACTER_GOODWILL, CCharacterReputation> GOODWILL_TABLE;
	friend GOODWILL_TABLE;
	static GOODWILL_TABLE m_relation_table;
};
