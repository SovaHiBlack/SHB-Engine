//	Module 		: ALifeHumanBrain.cpp
//	Description : ALife human brain class

#include "stdafx.h"

#include "ALifeHumanBrain.h"
#include "object_broker.h"
#include "xrServer_Objects_ALife_Monsters.h"

#include "ALifeHumanObjectHandler.h"
#include "ALifeMonsterMovementManager.h"
#include "ALifeMonsterDetailPathManager.h"
#include "ALifeMonsterPatrolPathManager.h"
#include "ai_space.h"
#include "ef_storage.h"
#include "ef_primary.h"
//#include "ALifeSimulator.h"
#include "alife_graph_registry.h"
#include "movement_manager_space.h"
#include "alife_smart_terrain_registry.h"
#include "alife_time_manager.h"
#include "date_time.h"

#ifdef DEBUG
#	include "Level.h"
#	include "MapLocation.h"
#	include "MapManager.h"
#endif

CALifeHumanBrain::CALifeHumanBrain(object_type* object) : inherited(object)
{
	VERIFY(object);
	m_object = object;

	m_object_handler = xr_new<CALifeHumanObjectHandler>(object);

	m_cpEquipmentPreferences.resize(5);
	m_cpMainWeaponPreferences.resize(4);

	m_cpEquipmentPreferences.resize(iFloor(ai( ).ef_storage( ).m_pfEquipmentType->ffGetMaxResultValue( ) + .5f));
	m_cpMainWeaponPreferences.resize(iFloor(ai( ).ef_storage( ).m_pfMainWeaponType->ffGetMaxResultValue( ) + .5f));
	R_ASSERT2((iFloor(ai( ).ef_storage( ).m_pfEquipmentType->ffGetMaxResultValue( ) + .5f) == 5) && (iFloor(ai( ).ef_storage( ).m_pfMainWeaponType->ffGetMaxResultValue( ) + .5f) == 4), "Recompile Level Editor and xrAI and rebuild file \"game.spawn\"!");

	for (int i = 0, n = m_cpEquipmentPreferences.size( ); i < n; ++i)
	{
		m_cpEquipmentPreferences[i] = U8(::Random.randI(3));
	}

	for (int i = 0, n = m_cpMainWeaponPreferences.size( ); i < n; ++i)
	{
		m_cpMainWeaponPreferences[i] = U8(::Random.randI(3));
	}
}

CALifeHumanBrain::~CALifeHumanBrain( )
{
	xr_delete(m_object_handler);
}

void CALifeHumanBrain::on_state_write(CNetPacket& packet)
{
	save_data(m_cpEquipmentPreferences, packet);
	save_data(m_cpMainWeaponPreferences, packet);
}

void CALifeHumanBrain::on_state_read(CNetPacket& packet)
{
	if (object( ).m_wVersion <= 19)
	{
		return;
	}

	if (object( ).m_wVersion < 110)
	{
		{
			DWORD_VECTOR temp;
			load_data(temp, packet);
		}
		{
			xr_vector<bool> temp;
			load_data(temp, packet);
		}
	}

	if (object( ).m_wVersion <= 35)
	{
		return;
	}

	if (object( ).m_wVersion < 110)
	{
		CSharedString temp;
		packet.r_stringZ(temp);
	}

	if (object( ).m_wVersion < 118)
	{
		ALife::OBJECT_VECTOR temp;
		load_data(temp, packet);
	}

	load_data(m_cpEquipmentPreferences, packet);
	load_data(m_cpMainWeaponPreferences, packet);
}
