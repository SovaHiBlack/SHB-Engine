// CharacterCommunity.h	структура представления группировки

#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"
#include "CharacterInfo_defs.h"

struct SCommunityData
{
	SCommunityData(CharacterCommunityIndex, CharacterCommunityId, const char*);

	CharacterCommunityId		id;
	CharacterCommunityIndex		index;
	unsigned char				team;
};

class CCharacterCommunity;

class CCharacterCommunity : public CIni_IdToIndex<1, SCommunityData, CharacterCommunityId, CharacterCommunityIndex, CCharacterCommunity>
{
private:
	using inherited = CIni_IdToIndex<1, SCommunityData, CharacterCommunityId, CharacterCommunityIndex, CCharacterCommunity>;
	friend inherited;

public:
								CCharacterCommunity		( );
								~CCharacterCommunity	( );

	void						set						(CharacterCommunityId);
	void						set						(CharacterCommunityIndex index)
	{
		m_current_index = index;
	}

	CharacterCommunityId		id						( ) const;
	CharacterCommunityIndex		index					( ) const
	{
		return m_current_index;
	}
	unsigned char				team					( ) const;

private:
	CharacterCommunityIndex								m_current_index;

	static void					InitIdToIndex			( );

public:
	// отношение между группировками
	static CharacterGoodwill	relation				(CharacterCommunityIndex from, CharacterCommunityIndex to);
	CharacterGoodwill			relation				(CharacterCommunityIndex to);

	static float				sympathy				(CharacterCommunityIndex);

	static void					DeleteIdToIndexData		( );

private:
	using GoodwillTable									= CIni_Table<CharacterGoodwill, CCharacterCommunity>;
	friend GoodwillTable;
	static GoodwillTable								m_relation_table;

	// таблица коэффициентов "сочуствия" между участниками группировки
	using SympathyTable									= CIni_Table<float, CCharacterCommunity>;
	friend SympathyTable;
	static SympathyTable								m_sympathy_table;
};
