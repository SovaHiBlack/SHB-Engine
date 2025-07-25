////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_misc.cpp
//	Created 	: 27.02.2003
//  Modified 	: 27.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Miscellaneous functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "Stalker.h"
#include "ai_stalker_space.h"
#include "../../Bolt.h"
#include "../../Inventory.h"
#include "../../CharacterInfo.h"
#include "../../RelationRegistry.h"
#include "../../memory_manager.h"
#include "../../item_manager.h"
#include "../../stalker_movement_manager.h"
#include "../../Explosive.h"
#include "../../agent_manager.h"
#include "../../agent_member_manager.h"
#include "../../agent_explosive_manager.h"
#include "../../agent_location_manager.h"
#include "../../danger_object_location.h"
#include "../../member_order.h"
#include "../../level.h"
#include "../../sound_player.h"
#include "../../enemy_manager.h"
#include "../../danger_manager.h"
#include "../../visual_memory_manager.h"
#include "../../agent_enemy_manager.h"

const u32 TOLLS_INTERVAL					= 2000;
const u32 GRENADE_INTERVAL					= 0*1000;
const f32 FRIENDLY_GRENADE_ALARM_DIST		= 5.0f;
const u32 DANGER_INFINITE_INTERVAL			= 60000000;
const f32 DANGER_EXPLOSIVE_DISTANCE			= 10.0f;

bool CStalker::useful		(const CItemManager *manager, const CGameObject *object) const
{
	const CExplosive	*explosive = smart_cast<const CExplosive*>(object);

	if (explosive && smart_cast<const CInventoryItem*>(object))
		agent_manager().location().add	(xr_new<CDangerObjectLocation>(object,Device.dwTimeGlobal,DANGER_INFINITE_INTERVAL,DANGER_EXPLOSIVE_DISTANCE));

	if (explosive && (explosive->CurrentParentID() != 0xffff)) {
		agent_manager().explosive().register_explosive(explosive,object);
		CEntityAlive			*entity_alive = smart_cast<CEntityAlive*>(Level().Objects.net_Find(explosive->CurrentParentID()));
		if (entity_alive)
			memory().danger().add(CDangerObject(entity_alive,object->Position(),Device.dwTimeGlobal,CDangerObject::eDangerTypeGrenade,CDangerObject::eDangerPerceiveTypeVisual,object));
	}

	if (!memory().item().useful(object))
		return			(false);

	const CInventoryItem *inventory_item = smart_cast<const CInventoryItem*>(object);
	if (!inventory_item || !inventory_item->useful_for_NPC())
		return			(false);

	if (!const_cast<CStalker*>(this)->can_take(inventory_item))
		return			(false);

	const CBolt			*bolt = smart_cast<const CBolt*>(object);
	if (bolt)
		return			(false);

	CInventory			*inventory_non_const = const_cast<CInventory*>(&inventory());
	CInventoryItem		*inventory_item_non_const = const_cast<CInventoryItem*>(inventory_item);
	if (!inventory_non_const->CanTakeItem(inventory_item_non_const))
		return			(false);

	return				(true);
}

f32 CStalker::evaluate		(const CItemManager *manager, const CGameObject *object) const
{
	f32				distance = Position().distance_to_sqr(object->Position());
	distance			= !fis_zero(distance) ? distance : EPSILON_3;
	return				(distance);
}

bool CStalker::useful		(const CEnemyManager *manager, const CEntityAlive *object) const
{
	if (!agent_manager().enemy().useful_enemy(object,this))
		return			(false);

	return				(memory().enemy().useful(object));
}

ALife::ERelationType CStalker::tfGetRelationType	(const CEntityAlive *tpEntityAlive) const
{
	const CInventoryOwner* pOtherIO = smart_cast<const CInventoryOwner*>(tpEntityAlive);
	
	ALife::ERelationType relation = ALife::eRelationTypeDummy;
		
	if(pOtherIO && !(const_cast<CEntityAlive*>(tpEntityAlive)->cast_base_monster()))
		relation = SRelationRegistry().GetRelationType(static_cast<const CInventoryOwner*>(this), pOtherIO);
	
	if(ALife::eRelationTypeDummy != relation)
		return relation;
	else
		return inherited::tfGetRelationType(tpEntityAlive);
}

void CStalker::react_on_grenades		()
{
	CMemberOrder::CGrenadeReaction	&reaction = agent_manager().member().member(this).grenade_reaction();
	if (!reaction.m_processing)
		return;

	if (Device.dwTimeGlobal < reaction.m_time + GRENADE_INTERVAL)
		return;

//	u32							interval = AFTER_GRENADE_DESTROYED_INTERVAL;
	const CMissile				*missile = smart_cast<const CMissile*>(reaction.m_grenade);
//	if (missile && (missile->destroy_time() > Device.dwTimeGlobal))
//		interval				= missile->destroy_time() - Device.dwTimeGlobal + AFTER_GRENADE_DESTROYED_INTERVAL;
//	m_object->agent_manager().add_danger_location(reaction.m_game_object->Position(),Device.dwTimeGlobal,interval,GRENADE_RADIUS);

	if (missile && agent_manager().member().group_behaviour()) {
//		Msg						("%6d : Stalker %s : grenade reaction",Device.dwTimeGlobal,*m_object->cName());
		CEntityAlive			*initiator = smart_cast<CEntityAlive*>(Level().Objects.net_Find(reaction.m_grenade->CurrentParentID()));
		if (is_relation_enemy(initiator))
			sound().play		(StalkerSpace::eStalkerSoundGrenadeAlarm);
		else
			if (missile->Position().distance_to(Position()) < FRIENDLY_GRENADE_ALARM_DIST)
				sound().play	(StalkerSpace::eStalkerSoundFriendlyGrenadeAlarm);
	}

	reaction.clear				();
}

void CStalker::react_on_member_death	()
{
	CMemberOrder::CMemberDeathReaction	&reaction = agent_manager().member().member(this).member_death_reaction();
	if (!reaction.m_processing)
		return;

	if (Device.dwTimeGlobal < reaction.m_time + TOLLS_INTERVAL)
		return;

	if (agent_manager().member().group_behaviour())
		sound().play			(StalkerSpace::eStalkerSoundTolls);

	reaction.clear				();
}

void CStalker::process_enemies		()
{
	if (memory().enemy().selected())
		return;

	typedef MemorySpace::squad_mask_type	squad_mask_type;
	typedef CVisualMemoryManager::VISIBLES	VISIBLES;

	bool						found = false;
	squad_mask_type				mask = memory().visual().mask();
	VISIBLES::const_iterator	I = memory().visual().objects().begin();
	VISIBLES::const_iterator	E = memory().visual().objects().end();
	for ( ; I != E; ++I) {
		if (!(*I).visible(mask))
			continue;

		const CStalker* member = smart_cast<const CStalker*>((*I).m_object);
		if (!member)
			continue;

		if (is_relation_enemy(member))
			continue;

		if (!member->g_Alive())
			continue;

		if (!member->memory().enemy().selected()) {
			if (!memory().danger().selected() && member->memory().danger().selected())
				memory().danger().add(*member->memory().danger().selected());
			continue;
		}

		memory().make_object_visible_somewhen	(member->memory().enemy().selected());
		found					= true;
		break;
	}
}
