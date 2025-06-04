//////////////////////////////////////////////////////////////////////////
// character_community.h:	структура представления группировки
//							
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"

#include "character_info_defs.h"


struct COMMUNITY_DATA
{
	COMMUNITY_DATA (CHARACTER_COMMUNITY_INDEX, CHARACTER_COMMUNITY_ID, pcstr);

	CHARACTER_COMMUNITY_ID		id;
	CHARACTER_COMMUNITY_INDEX	index;
	u8 team;
};


class CCharacterCommunity;

class CCharacterCommunity :
	public CIni_IdToIndex<1, COMMUNITY_DATA, CHARACTER_COMMUNITY_ID, CHARACTER_COMMUNITY_INDEX, CCharacterCommunity>
{
private:
	typedef CIni_IdToIndex<1, COMMUNITY_DATA, CHARACTER_COMMUNITY_ID, CHARACTER_COMMUNITY_INDEX, CCharacterCommunity> inherited;
	friend inherited;

public:
	CCharacterCommunity();
	~CCharacterCommunity();

	void						set				(CHARACTER_COMMUNITY_ID);		
	void						set				(CHARACTER_COMMUNITY_INDEX index) {m_current_index = index;};

	CHARACTER_COMMUNITY_ID		id				() const;
	CHARACTER_COMMUNITY_INDEX	index			() const	{return m_current_index;};
	u8							team			() const;

private:
	CHARACTER_COMMUNITY_INDEX	m_current_index;

	static	void				InitIdToIndex	();

public:
	//отношение между группировками
	static CHARACTER_GOODWILL	relation			(CHARACTER_COMMUNITY_INDEX from, CHARACTER_COMMUNITY_INDEX to);
	CHARACTER_GOODWILL			relation			(CHARACTER_COMMUNITY_INDEX to);
	
	static f32				sympathy			(CHARACTER_COMMUNITY_INDEX);
	
	static void					DeleteIdToIndexData	();
private:
	typedef CIni_Table<CHARACTER_GOODWILL, CCharacterCommunity> GOODWILL_TABLE;
	friend GOODWILL_TABLE;
	static GOODWILL_TABLE m_relation_table;

	//таблица коэффициентов "сочуствия" между участниками группировки
	typedef CIni_Table<f32, CCharacterCommunity> SYMPATHY_TABLE;
	friend SYMPATHY_TABLE;
	static SYMPATHY_TABLE m_sympathy_table;
};