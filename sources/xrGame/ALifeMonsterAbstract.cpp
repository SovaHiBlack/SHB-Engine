//	Module 		: ALifeMonsterAbstract.cpp
//	Description : ALife mnster abstract class

#include "stdafx.h"

#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "ALifeSimulator.h"
#include "alife_time_manager.h"
#include "alife_graph_registry.h"
#include "game_graph.h"
#include "alife_object_registry.h"
#include "ef_storage.h"
#include "ef_pattern.h"
#include "ALifeMonsterBrain.h"
#include "ALifeMonsterMovementManager.h"
#include "ALifeMonsterDetailPathManager.h"

void CSE_ALifeMonsterAbstract::add_online(const bool& update_registries)
{
	inherited1::add_online(update_registries);
	brain( ).on_switch_online( );
}

void CSE_ALifeMonsterAbstract::add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries)
{
	inherited1::add_offline(saved_children, update_registries);
	brain( ).on_switch_offline( );
}

void CSE_ALifeMonsterAbstract::update( )
{
	if (!bfActive( ))
	{
		return;
	}

	brain( ).update( );
}

CSE_ALifeItemWeapon* CSE_ALifeMonsterAbstract::tpfGetBestWeapon(ALife::EHitType& tHitType, float& fHitPower)
{
	m_tpCurrentBestWeapon = nullptr;
	fHitPower = m_fHitPower;
	tHitType = m_tHitType;
	return m_tpCurrentBestWeapon;
}

ALife::EMeetActionType CSE_ALifeMonsterAbstract::tfGetActionType(CSE_ALifeSchedulable* tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
	return ALife::eMeetActionTypeIgnore;
}

bool CSE_ALifeMonsterAbstract::bfActive( )
{
	CSE_ALifeGroupAbstract* l_tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(this);
	return (interactive( ) && ((l_tpALifeGroupAbstract && (l_tpALifeGroupAbstract->m_wCount > 0)) || (!l_tpALifeGroupAbstract && (fHealth > EPS_L))));
}

CSE_ALifeDynamicObject* CSE_ALifeMonsterAbstract::tpfGetBestDetector( )
{
	CSE_ALifeGroupAbstract* l_tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(this);
	if (!l_tpALifeGroupAbstract)
	{
		return this;
	}
	else
	{
		if (!l_tpALifeGroupAbstract->m_wCount)
		{
			return nullptr;
		}
		else
		{
			return ai( ).alife( ).objects( ).object(l_tpALifeGroupAbstract->m_tpMembers[0]);
		}
	}
}

void CSE_ALifeMonsterAbstract::vfCheckForPopulationChanges( )
{
	CSE_ALifeGroupAbstract* l_tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(this);
	if (!l_tpALifeGroupAbstract || !bfActive( ) || m_bOnline)
	{
		return;
	}

	ai( ).ef_storage( ).alife_evaluation(true);
	ALife::_TIME_ID l_tTimeID = ai( ).alife( ).time_manager( ).game_time( );
	if (l_tTimeID >= l_tpALifeGroupAbstract->m_tNextBirthTime)
	{
		ai( ).ef_storage( ).alife( ).member( ) = this;
		l_tpALifeGroupAbstract->m_tNextBirthTime = l_tTimeID + ALife::_TIME_ID(ai( ).ef_storage( ).m_pfBirthSpeed->ffGetValue( ) * 24 * 60 * 60 * 1000);
		if (randF(100.0f) < ai( ).ef_storage( ).m_pfBirthProbability->ffGetValue( ))
		{
			unsigned int l_dwBornCount = iFloor(float(l_tpALifeGroupAbstract->m_wCount) * randF(0.5f, 1.5f) * ai( ).ef_storage( ).m_pfBirthPercentage->ffGetValue( ) / 100.0f + 0.5f);
			if (l_dwBornCount)
			{
				l_tpALifeGroupAbstract->m_tpMembers.resize(l_tpALifeGroupAbstract->m_wCount + l_dwBornCount);
				ALife::OBJECT_IT I = l_tpALifeGroupAbstract->m_tpMembers.begin( ) + l_tpALifeGroupAbstract->m_wCount;
				ALife::OBJECT_IT E = l_tpALifeGroupAbstract->m_tpMembers.end( );
				for (; I != E; ++I)
				{
					CSE_Abstract* l_tpAbstract = alife( ).create(l_tpALifeGroupAbstract, this);
					*I = l_tpAbstract->ID;
				}

				l_tpALifeGroupAbstract->m_wCount = l_tpALifeGroupAbstract->m_wCount + unsigned short(l_dwBornCount);
			}
		}
	}
}

Fvector3 CSE_ALifeMonsterAbstract::draw_level_position( ) const
{

#if 0
	brain( ).update( );
#endif

	return brain( ).movement( ).detail( ).draw_level_position( );
}

bool CSE_ALifeMonsterAbstract::redundant( ) const
{
	if (g_Alive( ))
	{
		return false;
	}

	if (m_bOnline)
	{
		return false;
	}

	if (m_story_id != INVALID_STORY_ID)
	{
		return false;
	}

	if (!m_game_death_time)
	{
		return false;
	}

	ALife::_TIME_ID current_time = alife( ).time_manager( ).game_time( );
	VERIFY(m_game_death_time <= current_time);
	if ((m_game_death_time + m_stay_after_death_time_interval) > current_time)
	{
		return false;
	}

	return true;
}
