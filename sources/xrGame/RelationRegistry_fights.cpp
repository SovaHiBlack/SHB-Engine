// реестр для хранения данных об отношении персонажа к другим персонажам

#include "stdafx.h"

#include "RelationRegistry.h"

SRelationRegistry::SFightData::SFightData( )
{
	attacker = defender = 0xffff;
	total_hit = 0.0f;
	time = 0;
	time_old = 0;
	attack_time = 0;
	defender_to_attacker = ALife::eRelationTypeDummy;
}

void SRelationRegistry::FightRegister(unsigned short attacker, unsigned short defender, ALife::ERelationType defender_to_attacker, float hit_amount)
{
	UpdateFightRegister( );

	FightDataVec& fights = fight_registry( );
	for (FightDataVec_it it = fights.begin( ); it != fights.end( ); it++)
	{
		SFightData& fight_data = *it;
		if (attacker == fight_data.attacker && defender == fight_data.defender)
		{
			fight_data.time_old = fight_data.time;
			fight_data.time = Device.dwTimeGlobal;
			fight_data.total_hit += hit_amount;
			break;
		}
	}

	if (it == fights.end( ))
	{
		SFightData fight_data;
		fight_data.attacker = attacker;
		fight_data.defender = defender;
		fight_data.total_hit = hit_amount;
		fight_data.time = Device.dwTimeGlobal;
		fight_data.defender_to_attacker = defender_to_attacker;
		fights.push_back(fight_data);
	}
}

SRelationRegistry::SFightData* SRelationRegistry::FindFight(unsigned short object_id, bool by_attacker)
{
	FightDataVec& fights = fight_registry( );
	for (FightDataVec_it it = fights.begin( ); it != fights.end( ); it++)
	{
		SFightData& fight_data = *it;
		unsigned short id_to_find = by_attacker ? fight_data.attacker : fight_data.defender;
		if (object_id == id_to_find)
		{
			return &fight_data;
		}
	}

	return nullptr;
}

bool fight_time_pred(SRelationRegistry::SFightData& fight_data)
{
	//(c) время которое про драку помнит реестр (иначе считать неактуальным)
	static u32 fight_remember_time = u32(1000.0f * pSettings->r_float(ACTIONS_POINTS_SECT, "fight_remember_time"));

	u32 time_delta = Device.dwTimeGlobal - fight_data.time;
	if (time_delta > fight_remember_time)
	{
		return true;
	}

	return false;
}

void SRelationRegistry::UpdateFightRegister( )
{
	FightDataVec& fights = fight_registry( );
	FightDataVec_it it = std::remove_if(fights.begin( ), fights.end( ), fight_time_pred);
	fights.erase(it, fights.end( ));
}
