// CharacterInfo.h			шаблон, для представления абстрактного песонажа

#pragma once

#include "CharacterInfo_defs.h"
#include "shared_data.h"
#include "XML_IdToIndex.h"

class CNetPacket;

#include "SpecificCharacter.h"
#include "PhraseDialog_defs.h"
#include "CharacterCommunity.h"
#include "CharacterRank.h"
#include "CharacterReputation.h"

class CSE_ALifeTraderAbstract;

// данные профиля персонажа
struct SCharacterProfile : CSharedResource
{
				SCharacterProfile	( );
	virtual		~SCharacterProfile	( );

	// если задано, то выбирается именно такой профиль,
	// иначе ищется случайно, удовлетворяющее шаблону
	CSharedString					m_CharacterId;

	// требуемые параметры персонажа
	CharacterClass					m_Class;
	CharacterRankValue				m_Rank;
	CharacterReputationValue		m_Reputation;
};

class CInventoryOwner;
class CSE_ALifeTraderAbstract;

class CCharacterInfo : public CSharedClass<SCharacterProfile, CSharedString, false>, public CXML_IdToIndex<CCharacterInfo>
{
private:
	using inherited_shared		= CSharedClass<SCharacterProfile, CSharedString, false>;
	using id_to_index			= CXML_IdToIndex<CCharacterInfo>;

	friend id_to_index;
	friend CInventoryOwner;
	friend CSE_ALifeTraderAbstract;

public:
								CCharacterInfo			( );
								~CCharacterInfo			( );

	virtual void				Load					(CSharedString id);

	void 						load					(IReader&);
	void 						save					(CNetPacket&);

	// инициализация профиля подразумевает
	// загрузку соответствующего CSpecificCharacter, по
	// указанному индексу
	void						Init					(CSE_ALifeTraderAbstract* trader);
	void						InitSpecificCharacter	(CSharedString new_id);

protected:
	const SCharacterProfile*	data					( ) const
	{
		VERIFY(inherited_shared::get_sd( ));
		return inherited_shared::get_sd( );
	}
	SCharacterProfile*			data					( )
	{
		VERIFY(inherited_shared::get_sd( ));
		return inherited_shared::get_sd( );
	}

	static void					InitXmlIdToIndex		( );

	// загрузка из XML файла
	virtual void				load_shared				(const char*);

	// индекс загруженного профиля
	CSharedString										m_ProfileId;

	// индекс данных о конкретном персонаже, который
	// используется в данном экземпляре класса
	CSharedString										m_SpecificCharacterId;

	CSharedString										m_StartDialog;

	// загруженная информация о конкретном персонаже
	CSpecificCharacter									m_SpecificCharacter;

public:
	CSharedString				Profile					( ) const;
	const char*					Name					( ) const;
	CSharedString				Bio						( ) const;

	const CCharacterCommunity&	Community				( ) const
	{
		return m_CurrentCommunity;
	}
	const CCharacterRank&		Rank					( ) const
	{
		return m_CurrentRank;
	}
	const CCharacterReputation&	Reputation				( ) const
	{
		return m_CurrentReputation;
	}

protected:
	// доступут только у InventoryOwner
	void						SetRank					(CharacterRankValue rank);
	void						SetReputation			(CharacterReputationValue reputation);
	void						SetCommunity			(const CCharacterCommunity& community)
	{
		m_CurrentCommunity = community;
	}

public:
	const CSharedString&		IconName				( ) const;

	CSharedString				StartDialog				( ) const;
	const DialogIdVec&			ActorDialogs			( ) const;

protected:
	CCharacterRank										m_CurrentRank;
	CCharacterReputation								m_CurrentReputation;
	CCharacterCommunity									m_CurrentCommunity;
};
