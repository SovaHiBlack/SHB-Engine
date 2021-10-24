//	Module 		: ALifeMonsterMovementManager.cpp
//	Description : ALife monster movement manager class

#include "stdafx.h"

#include "ALifeMonsterMovementManager.h"
#include "ALifeMonsterDetailPathManager.h"
#include "ALifeMonsterPatrolPathManager.h"
#include "object_broker.h"
#include "movement_manager_space.h"

CALifeMonsterMovementManager::CALifeMonsterMovementManager(object_type* object)
{
	VERIFY(object);
	m_object = object;
	m_detail = xr_new<detail_path_type>(object);
	m_patrol = xr_new<patrol_path_type>(object);
	m_path_type = MovementManager::ePathTypeNoPath;
}

CALifeMonsterMovementManager::~CALifeMonsterMovementManager( )
{
	delete_data(m_detail);
	delete_data(m_patrol);
}

bool CALifeMonsterMovementManager::completed( ) const
{
	return true;
}

bool CALifeMonsterMovementManager::actual( ) const
{
	return true;
}

void CALifeMonsterMovementManager::update( )
{
	switch (path_type( ))
	{
		case MovementManager::ePathTypeGamePath:
		{
			detail( ).update( );
		}
		break;
		case MovementManager::ePathTypePatrolPath:
		{
			patrol( ).update( );
			detail( ).target(patrol( ).target_game_vertex_id( ), patrol( ).target_level_vertex_id( ), patrol( ).target_position( ));
			detail( ).update( );
		}
		break;
		case MovementManager::ePathTypeNoPath:
		{
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}
}

void CALifeMonsterMovementManager::on_switch_online( )
{
	detail( ).on_switch_online( );
}

void CALifeMonsterMovementManager::on_switch_offline( )
{
	detail( ).on_switch_offline( );
}
