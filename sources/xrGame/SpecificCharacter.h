// SpecificCharacter.h:	игровая информация о конкретном персонаже в игре

#pragma once

#include "CharacterInfo_defs.h"
#include "shared_data.h"
#include "XML_IdToIndex.h"
#include "PhraseDialog_defs.h"
#include "CharacterCommunity.h"

// SSpecificCharacterData: данные о конкретном персонаже
struct SSpecificCharacterData : CSharedResource
{
	SSpecificCharacterData( );
	virtual									~SSpecificCharacterData( );

	// игровое имя персонажа
	xr_string								m_sGameName;
	// текст с биографией персонажа (линк на string table)
	CSharedString								m_sBioText;
	// строка содержащая предметы, которые нужно проспавнить
	xr_string								m_sSupplySpawn;
	// имя секции конфигурации настроек NPC для персонажа
	xr_string								m_sNpcConfigSect;
	// имя секции конфигурации звука для NPC персонажа
	xr_string								m_sound_voice_prefix;

	float									m_fPanic_threshold;
	float									m_fHitProbabilityFactor;
	int										m_crouch_type;

	xr_string								m_critical_wound_weights;

	CSharedString							m_terrain_sect;

	// имя модели
	xr_string								m_sVisual;

	// начальный диалог
	CSharedString							m_StartDialog;
	// диалоги актера, которые будут доступны только при встрече с данным персонажем
	DialogIdVec								m_ActorDialogs;

	CSharedString							m_icon_name;
	// команда
	CCharacterCommunity						m_Community;
	// ранг
	CharacterRankValue						m_Rank;
	// репутация
	CharacterReputationValue				m_Reputation;

	// классы персонажа (военные-ветераны, ученые и т.д.)
	// к которым он принадлежит
	xr_vector<CharacterClass>				m_Classes;

	// указание на то что персонаж не предназначен для случайного выбора
	// и задается только через явное указание ID
	bool									m_bNoRandom;
	//если персонаж является заданым по умолчанию для своей команды
	bool									m_bDefaultForCommunity;

	struct SMoneyDef
	{
		u32									min_money;
		u32									max_money;
		bool								inf_money;
	};
	SMoneyDef								money_def;
};

class CInventoryOwner;
class CCharacterInfo;
class CSE_ALifeTraderAbstract;

class CSpecificCharacter : public CSharedClass<SSpecificCharacterData, CSharedString, false>, public CXML_IdToIndex<CSpecificCharacter>
{
private:
	using inherited_shared = CSharedClass<SSpecificCharacterData, CSharedString, false>;
	using id_to_index = CXML_IdToIndex<CSpecificCharacter>;

	friend id_to_index;
	friend CInventoryOwner;
	friend CCharacterInfo;
	friend CSE_ALifeTraderAbstract;

public:
	CSpecificCharacter( );
	~CSpecificCharacter( );

	virtual void							Load(CSharedString id);

protected:
	const SSpecificCharacterData* data( ) const
	{
		VERIFY(inherited_shared::get_sd( ));
		return inherited_shared::get_sd( );
	}
	SSpecificCharacterData* data( )
	{
		VERIFY(inherited_shared::get_sd( ));
		return inherited_shared::get_sd( );
	}

	// загрузка из XML файла
	virtual void							load_shared(const char*);
	static void								InitXmlIdToIndex( );

	CSharedString														m_OwnId;

public:
	const char* Name( ) const;
	CSharedString							Bio( ) const;
	const CCharacterCommunity& Community( ) const;
	SSpecificCharacterData::SMoneyDef& MoneyDef( )
	{
		return data( )->money_def;
	}

	CharacterRankValue					Rank( ) const;
	CharacterReputationValue				Reputation( ) const;
	const char* Visual( ) const;

	const char* SupplySpawn( ) const;
	const char* NpcConfigSect( ) const;
	const char* sound_voice_prefix( ) const;
	float									panic_threshold( ) const;
	float									hit_probability_factor( ) const;
	int										crouch_type( ) const;
	const char* critical_wound_weights( ) const;

	const CSharedString& IconName( ) const
	{
		return data( )->m_icon_name;
	}

	CSharedString								terrain_sect( ) const;
};
