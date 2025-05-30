//////////////////////////////////////////////////////////////////////////
// relation_registry.cpp:	������ ��� �������� ������ �� ��������� ��������� � 
//							������ ����������
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "relation_registry.h"
#include "alife_registry_wrappers.h"

#include "character_community.h"
#include "character_reputation.h"
#include "character_rank.h"


//////////////////////////////////////////////////////////////////////////

SRelation::SRelation()
{
	m_iGoodwill = NEUTRAL_GOODWILL;
}

SRelation::~SRelation()
{
}

//////////////////////////////////////////////////////////////////////////

void RELATION_DATA::clear	()
{
	personal.clear();
	communities.clear();
}

void RELATION_DATA::load (IReader& stream)
{
	load_data(personal, stream);
	load_data(communities, stream);
}

void RELATION_DATA::save (IWriter& stream)
{
	save_data(personal, stream);
	save_data(communities, stream);
}

//////////////////////////////////////////////////////////////////////////

RELATION_REGISTRY::RELATION_MAP_SPOTS::RELATION_MAP_SPOTS()
{
	spot_names[ALife::eRelationTypeFriend]		= "friend_location";
	spot_names[ALife::eRelationTypeNeutral]		= "neutral_location";
	spot_names[ALife::eRelationTypeEnemy]		= "enemy_location";
	spot_names[ALife::eRelationTypeWorstEnemy]	= "enemy_location";
	spot_names[ALife::eRelationTypeWorstEnemy]	= "enemy_location";
	spot_names[ALife::eRelationTypeLast]		= "neutral_location";
}
//////////////////////////////////////////////////////////////////////////

CRelationRegistryWrapper*					RELATION_REGISTRY::m_relation_registry	= NULL;
RELATION_REGISTRY::FIGHT_VECTOR*			RELATION_REGISTRY::m_fight_registry		= NULL;
RELATION_REGISTRY::RELATION_MAP_SPOTS*		RELATION_REGISTRY::m_spot_names			= NULL;


//////////////////////////////////////////////////////////////////////////


RELATION_REGISTRY::RELATION_REGISTRY  ()
{
}

RELATION_REGISTRY::~RELATION_REGISTRY ()
{
}

//////////////////////////////////////////////////////////////////////////

extern void load_attack_goodwill();
CRelationRegistryWrapper& RELATION_REGISTRY::relation_registry()
{
	if(!m_relation_registry){
		m_relation_registry = xr_new<CRelationRegistryWrapper>();
		load_attack_goodwill();
	}

	return *m_relation_registry;
}


RELATION_REGISTRY::FIGHT_VECTOR& RELATION_REGISTRY::fight_registry()
{
	if(!m_fight_registry)
		m_fight_registry = xr_new<FIGHT_VECTOR>();

	return *m_fight_registry;
}

void RELATION_REGISTRY::clear_relation_registry()
{
	xr_delete(m_relation_registry);
	xr_delete(m_fight_registry);
	xr_delete(m_spot_names);
}

const shared_str& RELATION_REGISTRY::GetSpotName(ALife::ERelationType& type)
{
	if(!m_spot_names)
		m_spot_names = xr_new<RELATION_MAP_SPOTS>();
	return m_spot_names->GetSpotName(type);
}

//////////////////////////////////////////////////////////////////////////

void RELATION_REGISTRY::ClearRelations	(u16 person_id)
{
	const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(person_id);
	if(relation_data)
	{
		relation_registry().registry().objects(person_id).clear();
	}
}



//////////////////////////////////////////////////////////////////////////
CHARACTER_GOODWILL	 RELATION_REGISTRY::GetGoodwill			(u16 from, u16 to) const 
{
	const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(from);

	if(relation_data)
	{
		PERSONAL_RELATION_MAP::const_iterator it = relation_data->personal.find(to);
		if(relation_data->personal.end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill();
		}
	}
	//���� ��������� ��� �� ������, �� ���������� �����������
	return NEUTRAL_GOODWILL;
}

void RELATION_REGISTRY::SetGoodwill 	(u16 from, u16 to, CHARACTER_GOODWILL goodwill)
{
	RELATION_DATA& relation_data = relation_registry().registry().objects(from);

	static iVector2 gw_limits		= pSettings->r_ivector2(ACTIONS_POINTS_SECT, "personal_goodwill_limits");
	clamp							(goodwill, gw_limits.x, gw_limits.y);

	relation_data.personal[to].SetGoodwill(goodwill);
}


void RELATION_REGISTRY::ChangeGoodwill 	(u16 from, u16 to, CHARACTER_GOODWILL delta_goodwill)
{
	CHARACTER_GOODWILL new_goodwill		= GetGoodwill(from, to)+ delta_goodwill;
	SetGoodwill							(from, to, new_goodwill);
}

//////////////////////////////////////////////////////////////////////////
CHARACTER_GOODWILL	 RELATION_REGISTRY::GetCommunityGoodwill (CHARACTER_COMMUNITY_INDEX from_community, u16 to_character) const 
{
	const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(to_character);

	if(relation_data)
	{
		COMMUNITY_RELATION_MAP::const_iterator it = relation_data->communities.find(from_community);
		if(relation_data->communities.end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill();
		}
	}
	//���� ��������� ��� �� ������, �� ���������� �����������
	return NEUTRAL_GOODWILL;
}

void RELATION_REGISTRY::SetCommunityGoodwill 	(CHARACTER_COMMUNITY_INDEX from_community, u16 to_character, CHARACTER_GOODWILL goodwill)
{
	static iVector2 gw_limits		= pSettings->r_ivector2(ACTIONS_POINTS_SECT, "community_goodwill_limits");
	clamp							(goodwill, gw_limits.x, gw_limits.y);
	RELATION_DATA& relation_data	= relation_registry().registry().objects(to_character);

	relation_data.communities[from_community].SetGoodwill(goodwill);
}

void RELATION_REGISTRY::ChangeCommunityGoodwill (CHARACTER_COMMUNITY_INDEX from_community, u16 to_character, CHARACTER_GOODWILL delta_goodwill)
{
	CHARACTER_GOODWILL gw = GetCommunityGoodwill(from_community, to_character)+ delta_goodwill;
	SetCommunityGoodwill	(from_community, to_character, gw);
}
//////////////////////////////////////////////////////////////////////////

CHARACTER_GOODWILL	 RELATION_REGISTRY::GetCommunityRelation		(CHARACTER_COMMUNITY_INDEX index1, CHARACTER_COMMUNITY_INDEX index2) const
{
	return CHARACTER_COMMUNITY::relation(index1, index2);
}

CHARACTER_GOODWILL	 RELATION_REGISTRY::GetRankRelation				(CHARACTER_RANK_VALUE rank1, CHARACTER_RANK_VALUE rank2) const
{
	CHARACTER_RANK rank_from, rank_to;
	rank_from.set(rank1);
	rank_to.set(rank2);
	return CHARACTER_RANK::relation(rank_from.index(), rank_to.index());
}

CHARACTER_GOODWILL	 RELATION_REGISTRY::GetReputationRelation		(CHARACTER_REPUTATION_VALUE rep1, CHARACTER_REPUTATION_VALUE rep2) const
{
	CHARACTER_REPUTATION rep_from, rep_to;
	rep_from.set(rep1);
	rep_to.set(rep2);
	return CHARACTER_REPUTATION::relation(rep_from.index(), rep_to.index());
}

//////////////////////////////////////////////////////////////////////////
