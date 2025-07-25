////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script3.cpp
//	Created 	: 17.11.2004
//  Modified 	: 17.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "AISpace.h"
#include "script_engine.h"
#include "cover_evaluators.h"
#include "cover_point.h"
#include "cover_manager.h"
#include "ai/stalker/Stalker.h"
#include "stalker_animation_manager.h"
#include "stalker_planner.h"
#include "Weapon.h"
#include "Inventory.h"
#include "CustomZone.h"
#include "patrol_path_manager.h"
#include "object_handler_planner.h"
#include "object_handler_space.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "sound_memory_manager.h"
#include "hit_memory_manager.h"
#include "sight_manager.h"
#include "stalker_movement_manager.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "LevelDebug.h"
#include "ai/monsters/BaseMonster/base_monster.h"
#include "TradeParameters.h"
#include "script_ini_file.h"
#include "sound_player.h"
#include "stalker_decision_space.h"
#include "CustomOutfit.h"

namespace MemorySpace
{
	struct CVisibleObject;
	struct CSoundObject;
	struct CHitObject;
};

const CCoverPoint* CScriptGameObject::best_cover(const fVector3& position, const fVector3& enemy_position, f32 radius, f32 min_enemy_distance, f32 max_enemy_distance)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member best_cover!");
		return		(0);
	}
	stalker->m_ce_best->setup(enemy_position, min_enemy_distance, max_enemy_distance, 0.f);
	const CCoverPoint* point = ai( ).cover_manager( ).best_cover(position, radius, *stalker->m_ce_best);
	return			(point);
}

const CCoverPoint* CScriptGameObject::safe_cover(const fVector3& position, f32 radius, f32 min_distance)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member best_cover!");
		return		(0);
	}
	stalker->m_ce_safe->setup(min_distance);
	const CCoverPoint* point = ai( ).cover_manager( ).best_cover(position, radius, *stalker->m_ce_safe);
	return			(point);
}

const xr_vector<MemorySpace::CVisibleObject>& CScriptGameObject::memory_visible_objects( ) const
{
	CCustomMonster* monster = smart_cast<CCustomMonster*>(&object( ));
	if (!monster)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member memory_visible_objects!");
		NODEFAULT;
	}
	return			(monster->memory( ).visual( ).objects( ));
}

const xr_vector<MemorySpace::CSoundObject>& CScriptGameObject::memory_sound_objects( ) const
{
	CCustomMonster* monster = smart_cast<CCustomMonster*>(&object( ));
	if (!monster)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member memory_sound_objects!");
		NODEFAULT;
	}
	return			(monster->memory( ).sound( ).objects( ));
}

const xr_vector<MemorySpace::CHitObject>& CScriptGameObject::memory_hit_objects( ) const
{
	CCustomMonster* monster = smart_cast<CCustomMonster*>(&object( ));
	if (!monster)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member memory_hit_objects!");
		NODEFAULT;
	}
	return			(monster->memory( ).hit( ).objects( ));
}

void CScriptGameObject::ChangeTeam(u8 team, u8 squad, u8 group)
{
	CCustomMonster* custom_monster = smart_cast<CCustomMonster*>(&object( ));
	if (!custom_monster)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CCustomMonster: cannot access class member ChangeTeam!");
	else
		custom_monster->ChangeTeam(team, squad, group);
}

CScriptGameObject* CScriptGameObject::GetEnemy( ) const
{
	CCustomMonster* l_tpCustomMonster = smart_cast<CCustomMonster*>(&object( ));
	if (l_tpCustomMonster)
	{
		if (l_tpCustomMonster->GetCurrentEnemy( ) && !l_tpCustomMonster->GetCurrentEnemy( )->getDestroy( )) return (l_tpCustomMonster->GetCurrentEnemy( )->lua_game_object( ));
		else return (0);
	}
	else
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member GetEnemy!");
		return			(0);
	}
}

CScriptGameObject* CScriptGameObject::GetCorpse( ) const
{
	CCustomMonster* l_tpCustomMonster = smart_cast<CCustomMonster*>(&object( ));
	if (l_tpCustomMonster)
		if (l_tpCustomMonster->GetCurrentCorpse( ) && !l_tpCustomMonster->GetCurrentCorpse( )->getDestroy( )) return (l_tpCustomMonster->GetCurrentCorpse( )->lua_game_object( ));
		else return (0);
	else
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member GetCorpse!");
		return			(0);
	}
}

bool CScriptGameObject::CheckTypeVisibility(pcstr section_name)
{
	CCustomMonster* l_tpCustomMonster = smart_cast<CCustomMonster*>(&object( ));
	if (l_tpCustomMonster)
		return			(l_tpCustomMonster->CheckTypeVisibility(section_name));
	else
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member CheckTypeVisibility!");
		return			(false);
	}
}

CScriptGameObject* CScriptGameObject::GetCurrentWeapon( ) const
{
	CStalker* l_tpStalker = smart_cast<CStalker*>(&object( ));
	if (!l_tpStalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject* current_weapon = l_tpStalker->GetCurrentWeapon( );
	return			(current_weapon ? current_weapon->lua_game_object( ) : 0);
}

CScriptGameObject* CScriptGameObject::GetCurrentOutfit( ) const
{
	CInventoryOwner* inventoryOwner = smart_cast<CInventoryOwner*>(&object( ));
	if (!inventoryOwner)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member GetCurrentOutfit!");
		return		(0);
	}
	CGameObject* current_equipment = inventoryOwner->GetCurrentOutfit( ) ? &inventoryOwner->GetCurrentOutfit( )->object( ) : 0;
	return			(current_equipment ? current_equipment->lua_game_object( ) : 0);
}

f32 CScriptGameObject::GetCurrentOutfitProtection(int hit_type)
{
	CInventoryOwner* inventoryOwner = smart_cast<CInventoryOwner*>(&object( ));
	if (!inventoryOwner)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member GetCurrentOutfitProtection!");
		return		(0);
	}
	CGameObject* current_equipment = &inventoryOwner->GetCurrentOutfit( )->object( );
	CCustomOutfit* o = smart_cast<CCustomOutfit*>(current_equipment);
	if (!o)				return 0.0f;

	return		1.0f - o->GetDefHitTypeProtection(ALife::EHitType(hit_type));
}

CScriptGameObject* CScriptGameObject::GetFood( ) const
{
	CStalker* l_tpStalker = smart_cast<CStalker*>(&object( ));
	if (!l_tpStalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member GetFood!");
		return		(0);
	}
	CGameObject* food = l_tpStalker->GetFood( ) ? &l_tpStalker->GetFood( )->object( ) : 0;
	return			(food ? food->lua_game_object( ) : 0);
}

CScriptGameObject* CScriptGameObject::GetMedikit( ) const
{
	CStalker* l_tpStalker = smart_cast<CStalker*>(&object( ));
	if (!l_tpStalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject* medikit = l_tpStalker->GetMedikit( ) ? &l_tpStalker->GetMedikit( )->object( ) : 0;
	return			(medikit ? medikit->lua_game_object( ) : 0);
}

pcstr CScriptGameObject::GetPatrolPathName( )
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		CScriptEntity* script_monster = smart_cast<CScriptEntity*>(&object( ));
		if (!script_monster)
		{
			ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member GetPatrolPathName!");
			return		("");
		}
		else
			return		(script_monster->GetPatrolPathName( ));
	}
	else
		return			(*stalker->movement( ).patrol( ).path_name( ));
}

void CScriptGameObject::add_animation(pcstr animation, bool hand_usage, bool use_movement_controller)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member add_animation!");
		return;
	}

	stalker->animation( ).add_script_animation(animation, hand_usage, use_movement_controller);
}

void CScriptGameObject::clear_animations( )
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member clear_animations!");
		return;
	}
	stalker->animation( ).clear_script_animations( );
}

int	CScriptGameObject::animation_count( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member clear_animations!");
		return			(-1);
	}
	return				((int)stalker->animation( ).script_animations( ).size( ));
}

flags32 CScriptGameObject::get_actor_relation_flags( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	THROW(stalker);

	return stalker->m_actor_relation_flags;
}

void CScriptGameObject::set_actor_relation_flags(flags32 flags)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	THROW(stalker);
	stalker->m_actor_relation_flags = flags;
}

void CScriptGameObject::set_patrol_path(pcstr path_name, const PatrolPathManager::EPatrolStartType patrol_start_type, const PatrolPathManager::EPatrolRouteType patrol_route_type, bool random)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
		stalker->movement( ).patrol( ).set_path(path_name, patrol_start_type, patrol_route_type, random);
}

void CScriptGameObject::set_dest_level_vertex_id(u32 level_vertex_id)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member set_dest_level_vertex_id!");
	else
	{

		if (!ai( ).level_graph( ).valid_vertex_id(level_vertex_id))
		{
#ifdef DEBUG
			ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : invalid vertex id being setup by action %s!", stalker->brain( ).CStalkerPlanner::current_action( ).m_action_name);
#endif
			return;
		}
		THROW2(stalker->movement( ).restrictions( ).accessible(level_vertex_id), *stalker->cName( ));
		stalker->movement( ).set_level_dest_vertex(level_vertex_id);
	}
}

CHARACTER_RANK_VALUE CScriptGameObject::GetRank( )
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member GetRank!");
		return					(CHARACTER_RANK_VALUE(0));
	}
	else
		return					(stalker->Rank( ));
}

void CScriptGameObject::set_desired_position( )
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
		stalker->movement( ).set_desired_position(0);
}

void CScriptGameObject::set_desired_position(const fVector3* desired_position)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
	{
		THROW2(desired_position || stalker->movement( ).restrictions( ).accessible(*desired_position), *stalker->cName( ));
		stalker->movement( ).set_desired_position(desired_position);
	}
}

void  CScriptGameObject::set_desired_direction( )
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
		stalker->movement( ).set_desired_direction(0);
}

void  CScriptGameObject::set_desired_direction(const fVector3* desired_direction)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
		stalker->movement( ).set_desired_direction(desired_direction);
}

void  CScriptGameObject::set_body_state(EBodyState body_state)
{
	THROW((body_state == eBodyStateStand) || (body_state == eBodyStateCrouch));
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
		stalker->movement( ).set_body_state(body_state);
}

void  CScriptGameObject::set_movement_type(EMovementType movement_type)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
		stalker->movement( ).set_movement_type(movement_type);
}

void  CScriptGameObject::set_mental_state(EMentalState mental_state)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
	{
#ifdef DEBUG
		if (mental_state != eMentalStateDanger)
		{
			if (stalker->brain( ).current_action_id( ) == StalkerDecisionSpace::eWorldOperatorCombatPlanner)
			{
				ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : set_mental_state is used during universal combat!");
				return;
			}
		}
#endif // DEBUG
		stalker->movement( ).set_mental_state(mental_state);
	}
}

void  CScriptGameObject::set_path_type(MovementManager::EPathType path_type)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
		stalker->movement( ).set_path_type(path_type);
}

void  CScriptGameObject::set_detail_path_type(DetailPathManager::EDetailPathType detail_path_type)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement!");
	else
		stalker->movement( ).set_detail_path_type(detail_path_type);
}

MonsterSpace::EBodyState CScriptGameObject::body_state( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member body_state!");
		return		(MonsterSpace::eBodyStateStand);
	}
	return			(stalker->movement( ).body_state( ));
}

MonsterSpace::EBodyState CScriptGameObject::target_body_state( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member body_state!");
		return		(MonsterSpace::eBodyStateStand);
	}
	return			(stalker->movement( ).target_body_state( ));
}

MonsterSpace::EMovementType CScriptGameObject::movement_type( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member movement_type!");
		return		(MonsterSpace::eMovementTypeStand);
	}
	return			(stalker->movement( ).movement_type( ));
}

MonsterSpace::EMovementType CScriptGameObject::target_movement_type( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member target_movement_type!");
		return		(MonsterSpace::eMovementTypeStand);
	}
	return			(stalker->movement( ).target_movement_type( ));
}

MonsterSpace::EMentalState CScriptGameObject::mental_state( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member mental_state!");
		return		(MonsterSpace::eMentalStateDanger);
	}
	return			(stalker->movement( ).mental_state( ));
}

MonsterSpace::EMentalState CScriptGameObject::target_mental_state( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member mental_state!");
		return		(MonsterSpace::eMentalStateDanger);
	}
	return			(stalker->movement( ).target_mental_state( ));
}

MovementManager::EPathType CScriptGameObject::path_type( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member path_type!");
		return		(MovementManager::ePathTypeNoPath);
	}
	return			(stalker->movement( ).path_type( ));
}

DetailPathManager::EDetailPathType CScriptGameObject::detail_path_type( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member detail_path_type!");
		return		(DetailPathManager::eDetailPathTypeSmooth);
	}
	return			(DetailPathManager::eDetailPathTypeSmooth);
}

void CScriptGameObject::set_sight(SightManager::ESightType sight_type, const fVector3* vector3d, u32 dwLookOverDelay)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSightManager : cannot access class member set_sight!");
	else
		stalker->sight( ).setup(sight_type, vector3d);
}

void CScriptGameObject::set_sight(SightManager::ESightType sight_type, bool torso_look, bool path)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSightManager : cannot access class member set_sight!");
	else
		stalker->sight( ).setup(sight_type, torso_look, path);
}

void CScriptGameObject::set_sight(SightManager::ESightType sight_type, const fVector3& vector3d, bool torso_look = false)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSightManager : cannot access class member set_sight!");
	else
		stalker->sight( ).setup(sight_type, vector3d, torso_look);
}

void CScriptGameObject::set_sight(SightManager::ESightType sight_type, const fVector3* vector3d)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSightManager : cannot access class member set_sight!");
	else
		stalker->sight( ).setup(sight_type, vector3d);
}

void CScriptGameObject::set_sight(CScriptGameObject* object_to_look)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSightManager : cannot access class member set_sight!");
	else
		stalker->sight( ).setup(&object_to_look->object( ));
}

void CScriptGameObject::set_sight(CScriptGameObject* object_to_look, bool torso_look)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSightManager : cannot access class member set_sight!");
	else
		stalker->sight( ).setup(&object_to_look->object( ), torso_look);
}

void CScriptGameObject::set_sight(CScriptGameObject* object_to_look, bool torso_look, bool fire_object)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSightManager : cannot access class member set_sight!");
	else
		stalker->sight( ).setup(&object_to_look->object( ), torso_look, fire_object);
}

void CScriptGameObject::set_sight(CScriptGameObject* object_to_look, bool torso_look, bool fire_object, bool no_pitch)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSightManager : cannot access class member set_sight!");
	else
		stalker->sight( ).setup(CSightAction(&object_to_look->object( ), torso_look, fire_object, no_pitch));
}

void CScriptGameObject::set_sight(const SMemoryInfo* memory_object, bool	torso_look)
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSightManager : cannot access class member set_sight!");
	else
		stalker->sight( ).setup(memory_object, torso_look);
}

// CStalker
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

u32	CScriptGameObject::GetInventoryObjectCount( ) const
{
	CInventoryOwner* l_tpInventoryOwner = smart_cast<CInventoryOwner*>(&object( ));
	if (l_tpInventoryOwner)
		return			(l_tpInventoryOwner->inventory( ).dwfGetObjectCount( ));
	else
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member obj_count!");
		return			(0);
	}
}

CScriptGameObject* CScriptGameObject::GetActiveItem( )
{
	CInventoryOwner* l_tpInventoryOwner = smart_cast<CInventoryOwner*>(&object( ));
	if (l_tpInventoryOwner)
		if (l_tpInventoryOwner->inventory( ).ActiveItem( ))
			return		(l_tpInventoryOwner->inventory( ).ActiveItem( )->object( ).lua_game_object( ));
		else
			return		(0);
	else
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member activge_item!");
		return			(0);
	}
}

CScriptGameObject* CScriptGameObject::GetObjectByName(pcstr caObjectName) const
{
	CInventoryOwner* l_tpInventoryOwner = smart_cast<CInventoryOwner*>(&object( ));
	if (l_tpInventoryOwner)
	{
		CInventoryItem* l_tpInventoryItem = l_tpInventoryOwner->inventory( ).GetItemFromInventory(caObjectName);
		CGameObject* l_tpGameObject = smart_cast<CGameObject*>(l_tpInventoryItem);
		if (!l_tpGameObject)
			return		(0);
		else
			return		(l_tpGameObject->lua_game_object( ));
	}
	else
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member object!");
		return			(0);
	}
}

CScriptGameObject* CScriptGameObject::GetObjectByIndex(int iIndex) const
{
	CInventoryOwner* l_tpInventoryOwner = smart_cast<CInventoryOwner*>(&object( ));
	if (l_tpInventoryOwner)
	{
		CInventoryItem* l_tpInventoryItem = l_tpInventoryOwner->inventory( ).tpfGetObjectByIndex(iIndex);
		CGameObject* l_tpGameObject = smart_cast<CGameObject*>(l_tpInventoryItem);
		if (!l_tpGameObject)
			return		(0);
		else
			return		(l_tpGameObject->lua_game_object( ));
	}
	else
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member object!");
		return			(0);
	}
}

void CScriptGameObject::EnableAnomaly( )
{
	CCustomZone* zone = smart_cast<CCustomZone*>(&object( )); THROW(zone);
	zone->ZoneEnable( );
}

void CScriptGameObject::DisableAnomaly( )
{
	CCustomZone* zone = smart_cast<CCustomZone*>(&object( )); THROW(zone);
	zone->ZoneDisable( );
}

f32 CScriptGameObject::GetAnomalyPower( )
{
	CCustomZone* zone = smart_cast<CCustomZone*>(&object( )); THROW(zone);
	return zone->GetMaxPower( );
}

void CScriptGameObject::SetAnomalyPower(f32 p)
{
	CCustomZone* zone = smart_cast<CCustomZone*>(&object( )); THROW(zone);
	zone->SetMaxPower(p);
}

bool CScriptGameObject::weapon_strapped( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member weapon_strapped!");
		return		(false);
	}
	return			(stalker->weapon_strapped( ));
}

bool CScriptGameObject::weapon_unstrapped( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member weapon_unstrapped!");
		return		(false);
	}
	return			(stalker->weapon_unstrapped( ));
}

bool CScriptGameObject::path_completed( ) const
{
	CCustomMonster* monster = smart_cast<CCustomMonster*>(&object( ));
	if (!monster)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member path_completed!");
		return		(false);
	}
	return			(monster->movement( ).path_completed( ));
}

void CScriptGameObject::patrol_path_make_inactual( )
{
	CCustomMonster* monster = smart_cast<CCustomMonster*>(&object( ));
	if (!monster)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member patrol_path_make_inactual!");
		return;
	}
	monster->movement( ).patrol( ).make_inactual( );
}

fVector3	CScriptGameObject::head_orientation( ) const
{
	CStalker* stalker = smart_cast<CStalker*>(&object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member head_orientation!");
		return		(fVector3( ).set(flt_max, flt_max, flt_max));
	}
	const SRotation& r = stalker->movement( ).head_orientation( ).current;
	return			(fVector3( ).setHP(-r.yaw, -r.pitch));
}

void CScriptGameObject::info_add(pcstr text)
{
#ifdef DEBUG
	DBG( ).object_info(&object( ), this).add_item(text, D3DCOLOR_XRGB(255, 0, 0), 0);
#endif
}

void CScriptGameObject::info_clear( )
{
#ifdef DEBUG
	DBG( ).object_info(&object( ), this).clear( );
#endif
}

void CScriptGameObject::jump(const fVector3& position, f32 factor)
{
	CBaseMonster* monster = smart_cast<CBaseMonster*>(&object( ));
	if (!monster)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot process jump for not a monster!");
		return;
	}

	monster->jump(position, factor);
}

void CScriptGameObject::make_object_visible_somewhen(CScriptGameObject* object)
{
	CStalker* stalker = smart_cast<CStalker*>(&this->object( ));
	if (!stalker)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CStalker : cannot access class member make_object_visible_somewhen!");
		return;
	}

	CEntityAlive* entity_alive = smart_cast<CEntityAlive*>(object);
	if (!entity_alive)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CEntityAlive : cannot access class member make_object_visible_somewhen!");
		return;
	}

	stalker->memory( ).make_object_visible_somewhen(entity_alive);
}

void CScriptGameObject::sell_condition(CScriptIniFile* ini_file, pcstr section)
{
	CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(&object( ));
	if (!inventory_owner)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member sell_condition!");
		return;
	}

	inventory_owner->trade_parameters( ).process(CTradeParameters::action_sell(0), *ini_file, section);
}

void CScriptGameObject::sell_condition(f32 friend_factor, f32 enemy_factor)
{
	CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(&object( ));
	if (!inventory_owner)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member sell_condition!");
		return;
	}

	inventory_owner->trade_parameters( ).default_factors(
		CTradeParameters::action_sell(0),
		CTradeFactors(
		friend_factor,
		enemy_factor
	)
	);
}

void CScriptGameObject::buy_condition(CScriptIniFile* ini_file, pcstr section)
{
	CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(&object( ));
	if (!inventory_owner)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member buy_condition!");
		return;
	}

	inventory_owner->trade_parameters( ).process(CTradeParameters::action_buy(0), *ini_file, section);
}

void CScriptGameObject::buy_condition(f32 friend_factor, f32 enemy_factor)
{
	CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(&object( ));
	if (!inventory_owner)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member buy_condition!");
		return;
	}

	inventory_owner->trade_parameters( ).default_factors(
		CTradeParameters::action_buy(0),
		CTradeFactors(
		friend_factor,
		enemy_factor
	)
	);
}

void CScriptGameObject::show_condition(CScriptIniFile* ini_file, pcstr section)
{
	CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(&object( ));
	if (!inventory_owner)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member show_condition!");
		return;
	}

	inventory_owner->trade_parameters( ).process(
		CTradeParameters::action_show(0),
		*ini_file,
		section
	);
}

void CScriptGameObject::buy_supplies(CScriptIniFile* ini_file, pcstr section)
{
	CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(&object( ));
	if (!inventory_owner)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member buy_condition!");
		return;
	}

	inventory_owner->buy_supplies(
		*ini_file,
		section
	);
}

void sell_condition(CScriptIniFile* ini_file, pcstr section)
{
	default_trade_parameters( ).process(CTradeParameters::action_sell(0), *ini_file, section);
}

void sell_condition(f32 friend_factor, f32 enemy_factor)
{
	default_trade_parameters( ).default_factors(
		CTradeParameters::action_sell(0),
		CTradeFactors(
		friend_factor,
		enemy_factor
	)
	);
}

void buy_condition(CScriptIniFile* ini_file, pcstr section)
{
	default_trade_parameters( ).process(CTradeParameters::action_buy(0), *ini_file, section);
}

void buy_condition(f32 friend_factor, f32 enemy_factor)
{
	default_trade_parameters( ).default_factors(
		CTradeParameters::action_buy(0),
		CTradeFactors(
		friend_factor,
		enemy_factor
	)
	);
}

void show_condition(CScriptIniFile* ini_file, pcstr section)
{
	default_trade_parameters( ).process(CTradeParameters::action_show(0), *ini_file, section);
}

pcstr CScriptGameObject::sound_prefix( ) const
{
	CCustomMonster* custom_monster = smart_cast<CCustomMonster*>(&object( ));
	if (!custom_monster)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CCustomMonster : cannot access class member sound_prefix!");
		return								(0);
	}

	return									(*custom_monster->sound( ).sound_prefix( ));
}

void CScriptGameObject::sound_prefix(pcstr sound_prefix)
{
	CCustomMonster* custom_monster = smart_cast<CCustomMonster*>(&object( ));
	if (!custom_monster)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CCustomMonster : cannot access class member sound_prefix!");
		return;
	}

	custom_monster->sound( ).sound_prefix(sound_prefix);
}
