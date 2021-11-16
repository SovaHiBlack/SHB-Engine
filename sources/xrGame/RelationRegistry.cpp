// RelationRegistry.cpp: реестр для хранения данных об отношении персонажа к другим персонажам

#include "stdafx.h"

#include "RelationRegistry.h"
#include "alife_registry_wrappers.h"
#include "CharacterCommunity.h"
#include "CharacterReputation.h"
#include "CharacterRank.h"

SRelation::SRelation( )
{
	m_iGoodwill = NEUTRAL_GOODWILL;
}

SRelation::~SRelation( )
{ }

void SRelationData::clear( )
{
	personal.clear( );
	communities.clear( );
}

void SRelationData::load(IReader& stream)
{
	load_data(personal, stream);
	load_data(communities, stream);
}

void SRelationData::save(IWriter& stream)
{
	save_data(personal, stream);
	save_data(communities, stream);
}

SRelationRegistry::SRelationMapSpots::SRelationMapSpots( )
{
	spot_names[ALife::eRelationTypeFriend] = "friend_location";
	spot_names[ALife::eRelationTypeNeutral] = "neutral_location";
	spot_names[ALife::eRelationTypeEnemy] = "enemy_location";
	spot_names[ALife::eRelationTypeWorstEnemy] = "enemy_location";
	spot_names[ALife::eRelationTypeLast] = "neutral_location";
}

CRelationRegistryWrapper* SRelationRegistry::m_relation_registry = nullptr;
SRelationRegistry::FightDataVec* SRelationRegistry::m_fight_registry = nullptr;
SRelationRegistry::SRelationMapSpots* SRelationRegistry::m_spot_names = nullptr;

SRelationRegistry::SRelationRegistry( )
{ }

SRelationRegistry::~SRelationRegistry( )
{ }

extern void load_attack_goodwill( );
CRelationRegistryWrapper& SRelationRegistry::relation_registry( )
{
	if (!m_relation_registry)
	{
		m_relation_registry = xr_new<CRelationRegistryWrapper>( );
		load_attack_goodwill( );
	}

	return *m_relation_registry;
}

SRelationRegistry::FightDataVec& SRelationRegistry::fight_registry( )
{
	if (!m_fight_registry)
	{
		m_fight_registry = xr_new<FightDataVec>( );
	}

	return *m_fight_registry;
}

void SRelationRegistry::clear_relation_registry( )
{
	xr_delete(m_relation_registry);
	xr_delete(m_fight_registry);
	xr_delete(m_spot_names);
}

const CSharedString& SRelationRegistry::GetSpotName(ALife::ERelationType& type)
{
	if (!m_spot_names)
	{
		m_spot_names = xr_new<SRelationMapSpots>( );
	}

	return m_spot_names->GetSpotName(type);
}

void SRelationRegistry::ClearRelations(unsigned short person_id)
{
	const SRelationData* relation_data = relation_registry( ).registry( ).objects_ptr(person_id);
	if (relation_data)
	{
		relation_registry( ).registry( ).objects(person_id).clear( );
	}
}

CharacterGoodwill SRelationRegistry::GetGoodwill(unsigned short from, unsigned short to) const
{
	const SRelationData* relation_data = relation_registry( ).registry( ).objects_ptr(from);

	if (relation_data)
	{
		PersonalRelationMap::const_iterator it = relation_data->personal.find(to);
		if (relation_data->personal.end( ) != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill( );
		}
	}

	//если отношение еще не задано, то возвращаем нейтральное
	return NEUTRAL_GOODWILL;
}

void SRelationRegistry::SetGoodwill(unsigned short from, unsigned short to, CharacterGoodwill goodwill)
{
	SRelationData& relation_data = relation_registry( ).registry( ).objects(from);

	static Ivector2 gw_limits = pSettings->r_ivector2(ACTIONS_POINTS_SECT, "personal_goodwill_limits");
	clamp(goodwill, gw_limits.x, gw_limits.y);

	relation_data.personal[to].SetGoodwill(goodwill);
}

void SRelationRegistry::ChangeGoodwill(unsigned short from, unsigned short to, CharacterGoodwill delta_goodwill)
{
	CharacterGoodwill new_goodwill = GetGoodwill(from, to) + delta_goodwill;
	SetGoodwill(from, to, new_goodwill);
}

CharacterGoodwill SRelationRegistry::GetCommunityGoodwill(CharacterCommunityIndex from_community, unsigned short to_character) const
{
	const SRelationData* relation_data = relation_registry( ).registry( ).objects_ptr(to_character);

	if (relation_data)
	{
		CommunityRelationMap::const_iterator it = relation_data->communities.find(from_community);
		if (relation_data->communities.end( ) != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill( );
		}
	}

	//если отношение еще не задано, то возвращаем нейтральное
	return NEUTRAL_GOODWILL;
}

void SRelationRegistry::SetCommunityGoodwill(CharacterCommunityIndex from_community, unsigned short to_character, CharacterGoodwill goodwill)
{
	static Ivector2 gw_limits = pSettings->r_ivector2(ACTIONS_POINTS_SECT, "community_goodwill_limits");
	clamp(goodwill, gw_limits.x, gw_limits.y);
	SRelationData& relation_data = relation_registry( ).registry( ).objects(to_character);

	relation_data.communities[from_community].SetGoodwill(goodwill);
}

void SRelationRegistry::ChangeCommunityGoodwill(CharacterCommunityIndex from_community, unsigned short to_character, CharacterGoodwill delta_goodwill)
{
	CharacterGoodwill gw = GetCommunityGoodwill(from_community, to_character) + delta_goodwill;
	SetCommunityGoodwill(from_community, to_character, gw);
}

CharacterGoodwill SRelationRegistry::GetCommunityRelation(CharacterCommunityIndex index1, CharacterCommunityIndex index2) const
{
	return CCharacterCommunity::relation(index1, index2);
}

CharacterGoodwill SRelationRegistry::GetRankRelation(CharacterRankValue rank1, CharacterRankValue rank2) const
{
	CCharacterRank rank_from;
	CCharacterRank rank_to;
	rank_from.set(rank1);
	rank_to.set(rank2);
	return CCharacterRank::relation(rank_from.index( ), rank_to.index( ));
}

CharacterGoodwill SRelationRegistry::GetReputationRelation(CharacterReputationValue rep1, CharacterReputationValue rep2) const
{
	CCharacterReputation rep_from;
	CCharacterReputation rep_to;
	rep_from.set(rep1);
	rep_to.set(rep2);
	return CCharacterReputation::relation(rep_from.index( ), rep_to.index( ));
}
