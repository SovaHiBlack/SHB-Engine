//////////////////////////////////////////////////////////////////////////
// character_reputation.h:		структура представления репутаций и 
//								отношений между ними		
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"
#include "character_info_defs.h"

struct REPUTATION_DATA
{
	REPUTATION_DATA (s32, shared_str, pcstr);

	shared_str					id;
	s32							index;
	CHARACTER_REPUTATION_VALUE	threshold;
};

class CHARACTER_REPUTATION;

class CHARACTER_REPUTATION: 
	public CIni_IdToIndex<1, REPUTATION_DATA, shared_str, s32, CHARACTER_REPUTATION>
{
private:
	typedef CIni_IdToIndex<1, REPUTATION_DATA, shared_str, s32, CHARACTER_REPUTATION> inherited;
	friend inherited;

public:
	CHARACTER_REPUTATION		():m_current_value(NO_REPUTATION){};
	~CHARACTER_REPUTATION		(){};

	void						set				(CHARACTER_REPUTATION_VALUE);

	shared_str					id				() const;
	s32							index			() const	{return m_current_index;};
	CHARACTER_REPUTATION_VALUE	value			() const	{return m_current_value;};

	static s32					ValueToIndex    (CHARACTER_REPUTATION_VALUE);

private:
	CHARACTER_REPUTATION_VALUE	m_current_value;
	s32							m_current_index;

	static	void				InitIdToIndex	();
public:
	//отношение между репутациями
	static CHARACTER_GOODWILL	relation			(s32 from, s32 to);
	CHARACTER_GOODWILL			relation			(s32 to);

	static void					DeleteIdToIndexData	();

private:
	typedef CIni_Table<CHARACTER_GOODWILL, CHARACTER_REPUTATION> GOODWILL_TABLE;
	friend GOODWILL_TABLE;
	static GOODWILL_TABLE m_relation_table;
};
