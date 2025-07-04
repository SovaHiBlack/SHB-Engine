//////////////////////////////////////////////////////////////////////////
// CharacterInfo.cpp			������� ���������� ��� ���������� � ����

#include "stdafx.h"
#include "CharacterInfo.h"

#ifdef XRGAME_EXPORTS
#	include "ui/UIXml.h"
#	include "PhraseDialog.h"
#	include "xrServer_Objects_ALife_Monsters.h"
#else // XRGAME_EXPORTS
#	include "UIXml.h"
#endif // XRGAME_EXPORTS

//////////////////////////////////////////////////////////////////////////
SCharacterProfile::SCharacterProfile( )
{
	m_CharacterId = NULL;
	m_Rank = NO_RANK;
	m_Reputation = NO_REPUTATION;
}

SCharacterProfile::~SCharacterProfile( )
{ }

//////////////////////////////////////////////////////////////////////////

CCharacterInfo::CCharacterInfo( )
{
	m_ProfileId = NULL;
	m_SpecificCharacterId = NULL;

#ifdef XRGAME_EXPORTS
	m_CurrentRank.set(NO_RANK);
	m_CurrentReputation.set(NO_REPUTATION);
	m_StartDialog = NULL;
#endif

}

CCharacterInfo::~CCharacterInfo( )
{ }

void CCharacterInfo::Load(shared_str id)
{
	m_ProfileId = id;
	inherited_shared::load_shared(m_ProfileId, NULL);
}

#ifdef XRGAME_EXPORTS
void CCharacterInfo::InitSpecificCharacter(shared_str new_id)
{
	R_ASSERT(new_id.size( ));
	m_SpecificCharacterId = new_id;

	m_SpecificCharacter.Load(m_SpecificCharacterId);
	if (Rank( ).value( ) == NO_RANK)
	{
		SetRank(m_SpecificCharacter.Rank( ));
	}

	if (Reputation( ).value( ) == NO_REPUTATION)
	{
		SetReputation(m_SpecificCharacter.Reputation( ));
	}

	if (Community( ).index( ) == NO_COMMUNITY_INDEX)
	{
		SetCommunity(m_SpecificCharacter.Community( ));
	}

	if (!m_StartDialog || !m_StartDialog.size( ))
	{
		m_StartDialog = m_SpecificCharacter.data( )->m_StartDialog;
	}
}
#endif

void CCharacterInfo::load_shared(pcstr)
{
	const ITEM_DATA& item_data = *id_to_index::GetById(m_ProfileId);

	CUIXml* pXML = item_data._xml;
	pXML->SetLocalRoot(pXML->GetRoot( ));

	XML_NODE* item_node = pXML->NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(item_node, "profile id=", *item_data.id);

	pXML->SetLocalRoot(item_node);

	pcstr spec_char = pXML->Read("specific_character", 0, NULL);
	if (!spec_char)
	{
		data( )->m_CharacterId = NULL;

		pcstr char_class = pXML->Read("class", 0, NULL);

		if (char_class)
		{
			pstr buf_str = xr_strdup(char_class);
			xr_strlwr(buf_str);
			data( )->m_Class = buf_str;
			xr_free(buf_str);
		}
		else
		{
			data( )->m_Class = NO_CHARACTER_CLASS;
		}

		data( )->m_Rank = pXML->ReadInt("rank", 0, NO_RANK);
		data( )->m_Reputation = pXML->ReadInt("reputation", 0, NO_REPUTATION);
	}
	else
	{
		data( )->m_CharacterId = spec_char;
	}
}

#ifdef XRGAME_EXPORTS
void CCharacterInfo::Init(CSE_ALifeTraderAbstract* trader)
{
	m_CurrentCommunity.set(trader->m_community_index);
	m_CurrentRank.set(trader->m_rank);
	m_CurrentReputation.set(trader->m_reputation);
	Load(trader->character_profile( ));
	InitSpecificCharacter(trader->specific_character( ));
}

shared_str CCharacterInfo::Profile( ) const
{
	return m_ProfileId;
}

pcstr CCharacterInfo::Name( ) const
{
	R_ASSERT2(m_SpecificCharacterId.size( ), m_SpecificCharacter.Name( ));
	return m_SpecificCharacter.Name( );
}

shared_str CCharacterInfo::Bio( ) const
{
	return m_SpecificCharacter.Bio( );
}

void CCharacterInfo::SetRank(CHARACTER_RANK_VALUE rank)
{
	m_CurrentRank.set(rank);
}

void CCharacterInfo::SetReputation(CHARACTER_REPUTATION_VALUE reputation)
{
	m_CurrentReputation.set(reputation);
}

const shared_str& CCharacterInfo::IconName( ) const
{
	R_ASSERT(m_SpecificCharacterId.size( ));
	return m_SpecificCharacter.IconName( );
}

shared_str CCharacterInfo::StartDialog( ) const
{
	return m_StartDialog;
}

const DIALOG_ID_VECTOR& CCharacterInfo::ActorDialogs( ) const
{
	R_ASSERT(m_SpecificCharacterId.size( ));
	return m_SpecificCharacter.data( )->m_ActorDialogs;
}

void CCharacterInfo::load(IReader& stream)
{
	stream.r_stringZ(m_StartDialog);
}

void CCharacterInfo::save(CNetPacket& stream)
{
	stream.w_stringZ(m_StartDialog);
}
#endif

void CCharacterInfo::InitXmlIdToIndex( )
{
	if (!id_to_index::tag_name)
	{
		id_to_index::tag_name = "character";
	}

	if (!id_to_index::file_str)
	{
		id_to_index::file_str = pSettings->r_string("profiles", "files");
	}
}
