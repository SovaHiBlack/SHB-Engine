////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Stalker"
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "ai_stalker.h"
#include "ai_stalker_impl.h"
#include "../../script_entity_action.h"
#include "../../Inventory.h"
#include "../../ef_storage.h"
#include "../../stalker_decision_space.h"
#include "../../script_game_object.h"
#include "../../CustomZone.h"
#include "../../..\XR_3DA\skeletonanimated.h"
#include "../../agent_manager.h"
#include "../../stalker_animation_manager.h"
#include "../../stalker_planner.h"
#include "../../ef_pattern.h"
#include "../../memory_manager.h"
#include "../../hit_memory_manager.h"
#include "../../enemy_manager.h"
#include "../../item_manager.h"
#include "../../stalker_movement_manager.h"
#include "../../EntityCondition.h"
#include "../../sound_player.h"
#include "../../cover_point.h"
#include "../../agent_member_manager.h"
#include "../../agent_location_manager.h"
#include "../../danger_cover_location.h"
#include "../../object_handler_planner.h"
#include "../../object_handler_space.h"
#include "../../visual_memory_manager.h"
#include "../../Weapon.h"
#include "ai_stalker_space.h"
#include "../../effectorshot.h"
#include "../../BoneProtections.h"
#include "../../RadioactiveZone.h"
#include "../../restricted_object.h"
#include "../../ai_object_location.h"
#include "../../Missile.h"
#include "../../phworld.h"
#include "../../stalker_animation_names.h"
#include "../../agent_corpse_manager.h"
#include "../../CharacterPhysicsSupport.h"

using namespace StalkerSpace;

const f32 DANGER_DISTANCE = 3.0f;
const u32	DANGER_INTERVAL = 120000;

const f32 PRECISE_DISTANCE = 2.5f;
const f32 FLOOR_DISTANCE = 2.0f;
const f32 NEAR_DISTANCE = 2.5f;
const u32	FIRE_MAKE_SENSE_INTERVAL = 10000;

f32 CStalker::GetWeaponAccuracy( ) const
{
	f32				base = PI / 180.0f;

	//влияние ранга на меткость
	base *= m_fRankDisperison;

	if (!movement( ).path_completed( ))
	{
		if (movement( ).movement_type( ) == eMovementTypeWalk)
			if (movement( ).body_state( ) == eBodyStateStand)
				return		(base * m_disp_walk_stand);
			else
				return		(base * m_disp_walk_crouch);
		else
			if (movement( ).movement_type( ) == eMovementTypeRun)
				if (movement( ).body_state( ) == eBodyStateStand)
					return	(base * m_disp_run_stand);
				else
					return	(base * m_disp_run_crouch);
	}

	if (movement( ).body_state( ) == eBodyStateStand)
		if (zoom_state( ))
			return			(base * m_disp_stand_stand);
		else
			return			(base * m_disp_stand_stand_zoom);
	else
		if (zoom_state( ))
			return			(base * m_disp_stand_crouch);
		else
			return			(base * m_disp_stand_crouch_zoom);
}

void CStalker::g_fireParams(const CHudItem* pHudItem, fVector3& P, fVector3& D)
{
//.	VERIFY				(inventory().ActiveItem());
	if (!inventory( ).ActiveItem( ))
	{
#ifdef DEBUG
		Msg("! CStalker::g_fireParams() : VERIFY(inventory().ActiveItem())");
#endif // DEBUG
		P = Position( );
		D = fVector3( ).set(0.0f, 0.0f, 1.0f);
		return;
	}

	CWeapon* weapon = smart_cast<CWeapon*>(inventory( ).ActiveItem( ));
	if (!weapon)
	{
		CMissile* missile = smart_cast<CMissile*>(inventory( ).ActiveItem( ));
		if (missile)
		{
			update_throw_params( );
			P = m_throw_position;
			D = m_throw_direction;
			VERIFY(!fis_zero(D.square_magnitude( )));
			return;
		}
		P = eye_matrix.c;
		D = eye_matrix.k;
		if (weapon_shot_effector( ).IsActive( ))
			D = weapon_shot_effector_direction(D);
		VERIFY(!fis_zero(D.square_magnitude( )));
		return;
	}

	if (!g_Alive( ) || !animation( ).script_animations( ).empty( ))
	{
		P = weapon->get_LastFP( );
		D = weapon->get_LastFD( );
		VERIFY(!fis_zero(D.square_magnitude( )));
		return;
	}

	switch (movement( ).body_state( ))
	{
		case eBodyStateStand:
		{
			if (movement( ).movement_type( ) == eMovementTypeStand)
			{
				P = eye_matrix.c;
				D = eye_matrix.k;
				if (weapon_shot_effector( ).IsActive( ))
					D = weapon_shot_effector_direction(D);
				VERIFY(!fis_zero(D.square_magnitude( )));
			}
			else
			{
				D.setHP(-movement( ).m_head.current.yaw, -movement( ).m_head.current.pitch);
				if (weapon_shot_effector( ).IsActive( ))
					D = weapon_shot_effector_direction(D);
				Center(P);
				P.mad(D, .5f);
				P.y += .50f;
//				P		= weapon->get_LastFP();
//				D		= weapon->get_LastFD();
				VERIFY(!fis_zero(D.square_magnitude( )));
			}
			return;
		}
		case eBodyStateCrouch:
		{
			P = eye_matrix.c;
			D = eye_matrix.k;
			if (weapon_shot_effector( ).IsActive( ))
				D = weapon_shot_effector_direction(D);
			VERIFY(!fis_zero(D.square_magnitude( )));
			return;
		}
		default: NODEFAULT;
	}

#ifdef DEBUG
	P = weapon->get_LastFP( );
	D = weapon->get_LastFD( );
	VERIFY(!fis_zero(D.square_magnitude( )));
#endif
}

void CStalker::g_WeaponBones(s32& L, s32& R1, s32& R2)
{
	s32 r_hand;
	s32 r_finger2;
	s32 l_finger1;
	CObjectHandler::weapon_bones(r_hand, r_finger2, l_finger1);
	R1 = r_hand;
	R2 = r_finger2;
	if (!animation( ).script_animations( ).empty( ) && animation( ).script_animations( ).front( ).hand_usage( ))
	{
		L = R2;
	}
	else
	{
		L = l_finger1;
	}
}

void			CStalker::Hit(SHit* pHDS)
{
	if (invulnerable( ))
		return;

//	pHDS->power						*= .1f;

	//хит может меняться в зависимости от ранга (новички получают больше хита, чем ветераны)
	SHit							HDS = *pHDS;
	HDS.power *= m_fRankImmunity;
	if (m_boneHitProtection && HDS.hit_type == ALife::eHitTypeFireWound)
	{
		f32						BoneArmour = m_boneHitProtection->getBoneArmour(HDS.bone( ));
		f32						NewHitPower = HDS.damage( ) - BoneArmour;
		if (NewHitPower < HDS.power * m_boneHitProtection->m_fHitFrac) HDS.power = HDS.power * m_boneHitProtection->m_fHitFrac;
		else
			HDS.power = NewHitPower;

		if (wounded( ))
			HDS.power = 1000.f;
	}

	if (g_Alive( ))
	{
		bool						already_critically_wounded = critically_wounded( );

		if (!already_critically_wounded)
		{
			const CCoverPoint* cover = agent_manager( ).member( ).member(this).cover( );
			if (cover && pHDS->initiator( ) && (pHDS->initiator( )->ID( ) != ID( )) && !fis_zero(pHDS->damage( )) && brain( ).affect_cover( ))
				agent_manager( ).location( ).add(xr_new<CDangerCoverLocation>(cover, Device.dwTimeGlobal, DANGER_INTERVAL, DANGER_DISTANCE));
		}

		const CEntityAlive* entity_alive = smart_cast<const CEntityAlive*>(pHDS->initiator( ));
		if (entity_alive && !wounded( ))
		{
			if (is_relation_enemy(entity_alive))
				sound( ).play(eStalkerSoundInjuring);
//			else
//				sound().play		(eStalkerSoundInjuringByFriend);
		}

		s32							weapon_type = -1;
		if (best_weapon( ))
			weapon_type = best_weapon( )->object( ).ef_weapon_type( );

		if (
			!wounded( ) &&
			!already_critically_wounded)
		{
			bool					became_critically_wounded = update_critical_wounded(HDS.boneID, HDS.power);
			if (
				!became_critically_wounded &&
				animation( ).script_animations( ).empty( ) &&
				(pHDS->bone( ) != BI_NONE)
				)
			{
				fVector3					D;
				f32					yaw;
				f32					pitch;
				D.getHP(yaw, pitch);

#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
				f32					power_factor = m_power_fx_factor * pHDS->damage( ) / 100.f;
				clamp(power_factor, 0.f, 1.f);

				CKinematicsAnimated* tpKinematics = smart_cast<CKinematicsAnimated*>(Visual( ));
#ifdef DEBUG
				tpKinematics->LL_GetBoneInstance(pHDS->bone( ));
				if (pHDS->bone( ) >= tpKinematics->LL_BoneCount( ))
				{
					Msg("tpKinematics has no bone_id %d", pHDS->bone( ));
					pHDS->_dump( );
				}
#endif
//				s32						fx_index = iFloor(tpKinematics->LL_GetBoneInstance(pHDS->bone()).get_param(1) + (angle_difference(movement().m_body.current.yaw,-yaw) <= PI_DIV_2 ? 0 : 1));
//				if (fx_index != -1)
//					animation().play_fx	(power_factor,fx_index);
			}
			else
			{
				if (!already_critically_wounded && became_critically_wounded)
				{
					if (HDS.who)
					{
						CStalker* stalker = smart_cast<CStalker*>(HDS.who);
						if (stalker)
							stalker->on_critical_wound_initiator(this);
					}
				}
			}
		}
	}

	inherited::Hit(&HDS);
}

void CStalker::HitSignal(f32 amount, fVector3& vLocalDir, CObject* who, s16 element)
{
	if (getDestroy( ))
		return;

	if (g_Alive( ))
		memory( ).hit( ).add(amount, vLocalDir, who, element);
}

void CStalker::OnItemTake(CInventoryItem* inventory_item)
{
	CObjectHandler::OnItemTake(inventory_item);
	m_item_actuality = false;
	m_sell_info_actuality = false;
}

void CStalker::OnItemDrop(CInventoryItem* inventory_item)
{
	CObjectHandler::OnItemDrop(inventory_item);

	m_item_actuality = false;
	m_sell_info_actuality = false;

	if (!g_Alive( ))
		return;

	if (!critically_wounded( ))
		return;

//	VERIFY						(inventory().ActiveItem());

	if (inventory( ).ActiveItem( ) && (inventory( ).ActiveItem( ) != inventory_item))
		return;

	brain( ).CStalkerPlanner::m_storage.set_property(StalkerDecisionSpace::eWorldPropertyCriticallyWounded, false);
}

void CStalker::update_best_item_info( )
{
	ai( ).ef_storage( ).alife_evaluation(false);

	if (
		m_item_actuality &&
		m_best_item_to_kill &&
		m_best_item_to_kill->can_kill( )
		)
	{

		if (!memory( ).enemy( ).selected( ))
			return;

		ai( ).ef_storage( ).non_alife( ).member( ) = this;
		ai( ).ef_storage( ).non_alife( ).enemy( ) = memory( ).enemy( ).selected( ) ? memory( ).enemy( ).selected( ) : this;
		ai( ).ef_storage( ).non_alife( ).member_item( ) = &m_best_item_to_kill->object( );
		f32									value;
		value = ai( ).ef_storage( ).m_pfWeaponEffectiveness->ffGetValue( );
		if (fsimilar(value, m_best_item_value))
			return;
	}

	// initialize parameters
	m_item_actuality = true;
	ai( ).ef_storage( ).non_alife( ).member( ) = this;
	ai( ).ef_storage( ).non_alife( ).enemy( ) = memory( ).enemy( ).selected( ) ? memory( ).enemy( ).selected( ) : this;
	m_best_item_to_kill = 0;
	m_best_ammo = 0;
	m_best_found_item_to_kill = 0;
	m_best_found_ammo = 0;
	m_best_item_value = 0.f;

	// try to find the best item which can kill
	{
		TIItemContainer::iterator					I = inventory( ).m_all.begin( );
		TIItemContainer::iterator					E = inventory( ).m_all.end( );
		for (; I != E; ++I)
		{
			if ((*I)->can_kill( ))
			{
				ai( ).ef_storage( ).non_alife( ).member_item( ) = &(*I)->object( );
				f32								value;
				if (memory( ).enemy( ).selected( ))
					value = ai( ).ef_storage( ).m_pfWeaponEffectiveness->ffGetValue( );
				else
					value = (f32)(*I)->object( ).ef_weapon_type( );

				if (!fsimilar(value, m_best_item_value) && (value < m_best_item_value))
					continue;

				if (!fsimilar(value, m_best_item_value) && (value > m_best_item_value))
				{
					m_best_item_value = value;
					m_best_item_to_kill = *I;
					continue;
				}

				VERIFY(fsimilar(value, m_best_item_value));
				if (m_best_item_to_kill && ((*I)->object( ).ef_weapon_type( ) <= m_best_item_to_kill->object( ).ef_weapon_type( )))
					continue;

				m_best_item_value = value;
				m_best_item_to_kill = *I;
			}
		}
	}

	// check if we found
	if (m_best_item_to_kill)
	{
		m_best_ammo = m_best_item_to_kill;
		return;
	}

	// so we do not have such an item
	// check if we remember we saw item which can kill
	// or items which can make my item killing
	{
		xr_vector<const CGameObject*>::const_iterator	I = memory( ).item( ).objects( ).begin( );
		xr_vector<const CGameObject*>::const_iterator	E = memory( ).item( ).objects( ).end( );
		for (; I != E; ++I)
		{
			const CInventoryItem* inventory_item = smart_cast<const CInventoryItem*>(*I);
			if (!inventory_item || !memory( ).item( ).useful(&inventory_item->object( )))
				continue;
			CInventoryItem* item = inventory_item->can_kill(&inventory( ));
			if (item)
			{
				ai( ).ef_storage( ).non_alife( ).member_item( ) = &inventory_item->object( );
				f32 value = ai( ).ef_storage( ).m_pfWeaponEffectiveness->ffGetValue( );
				if (value > m_best_item_value)
				{
					m_best_item_value = value;
					m_best_found_item_to_kill = inventory_item;
					m_best_found_ammo = 0;
					m_best_ammo = item;
				}
			}
			else
			{
				item = inventory_item->can_make_killing(&inventory( ));
				if (!item)
					continue;

				ai( ).ef_storage( ).non_alife( ).member_item( ) = &item->object( );
				f32 value = ai( ).ef_storage( ).m_pfWeaponEffectiveness->ffGetValue( );
				if (value > m_best_item_value)
				{
					m_best_item_value = value;
					m_best_item_to_kill = item;
					m_best_found_item_to_kill = 0;
					m_best_found_ammo = inventory_item;
				}
			}
		}
	}

	// check if we found such an item
	if (m_best_found_item_to_kill || m_best_found_ammo)
		return;

	// check if we remember we saw item to kill
	// and item which can make this item killing
	xr_vector<const CGameObject*>::const_iterator	I = memory( ).item( ).objects( ).begin( );
	xr_vector<const CGameObject*>::const_iterator	E = memory( ).item( ).objects( ).end( );
	for (; I != E; ++I)
	{
		const CInventoryItem* inventory_item = smart_cast<const CInventoryItem*>(*I);
		if (!inventory_item || !memory( ).item( ).useful(&inventory_item->object( )))
			continue;
		const CInventoryItem* item = inventory_item->can_kill(memory( ).item( ).objects( ));
		if (item)
		{
			ai( ).ef_storage( ).non_alife( ).member_item( ) = &inventory_item->object( );
			f32 value = ai( ).ef_storage( ).m_pfWeaponEffectiveness->ffGetValue( );
			if (value > m_best_item_value)
			{
				m_best_item_value = value;
				m_best_found_item_to_kill = inventory_item;
				m_best_found_ammo = item;
			}
		}
	}
}

bool CStalker::item_to_kill( )
{
	update_best_item_info( );
	return					(!!m_best_item_to_kill);
}

bool CStalker::item_can_kill( )
{
	update_best_item_info( );
	return					(!!m_best_ammo);
}

bool CStalker::remember_item_to_kill( )
{
	update_best_item_info( );
	return					(!!m_best_found_item_to_kill);
}

bool CStalker::remember_ammo( )
{
	update_best_item_info( );
	return					(!!m_best_found_ammo);
}

bool CStalker::ready_to_kill( )
{
	return					(
		m_best_item_to_kill &&
		inventory( ).ActiveItem( ) &&
		(inventory( ).ActiveItem( )->object( ).ID( ) == m_best_item_to_kill->object( ).ID( )) &&
		m_best_item_to_kill->ready_to_kill( )
		);
}

bool CStalker::ready_to_detour( )
{
	if (!ready_to_kill( ))
		return			(false);

	CWeapon* weapon = smart_cast<CWeapon*>(m_best_item_to_kill);
	if (!weapon)
		return			(false);

	return				(weapon->GetAmmoElapsed( ) > weapon->GetAmmoMagSize( ) / 2);
}

class ray_query_param
{
public:
	CStalker* m_holder;
	f32					m_power;
	f32					m_power_threshold;
	bool					m_can_kill_enemy;
	bool					m_can_kill_member;
	f32					m_pick_distance;

public:
	IC				ray_query_param(const CStalker* holder, f32 power_threshold, f32 distance)
	{
		m_holder = const_cast<CStalker*>(holder);
		m_power_threshold = power_threshold;
		m_power = 1.0f;
		m_can_kill_enemy = false;
		m_can_kill_member = false;
		m_pick_distance = distance;
	}
};

IC BOOL ray_query_callback(collide::rq_result& result, pvoid params)
{
	ray_query_param* param = (ray_query_param*)params;
	f32								power = param->m_holder->feel_vision_mtl_transp(result.O, result.element);
	param->m_power *= power;

//	if (power >= .05f) {
//		param->m_pick_distance			= result.range;
//		return							(true);
//	}

	if (!result.O)
	{
		if (param->m_power > param->m_power_threshold)
			return						(true);

		param->m_pick_distance = result.range;
		return							(false);
	}

	CEntityAlive* entity_alive = smart_cast<CEntityAlive*>(result.O);
	if (!entity_alive)
	{
		if (param->m_power > param->m_power_threshold)
			return						(true);

		param->m_pick_distance = result.range;
		return							(false);
	}

	if (param->m_holder->is_relation_enemy(entity_alive))
		param->m_can_kill_enemy = true;
	else
		param->m_can_kill_member = true;

	param->m_pick_distance = result.range;
	return								(false);
}

void CStalker::can_kill_entity(const fVector3& position, const fVector3& direction, f32 distance, collide::rq_results& rq_storage)
{
	VERIFY(!fis_zero(direction.square_magnitude( )));

	collide::ray_defs				ray_defs(position, direction, distance, CDB::OPT_CULL, collide::rqtBoth);
	VERIFY(!fis_zero(ray_defs.dir.square_magnitude( )));

	ray_query_param					params(this, memory( ).visual( ).transparency_threshold( ), distance);

	Level( ).ObjectSpace.RayQuery(rq_storage, ray_defs, ray_query_callback, &params, NULL, this);
	m_can_kill_enemy = m_can_kill_enemy || params.m_can_kill_enemy;
	m_can_kill_member = m_can_kill_member || params.m_can_kill_member;
	m_pick_distance = _max(m_pick_distance, params.m_pick_distance);
}

void CStalker::can_kill_entity_from(const fVector3& position, fVector3 direction, f32 distance)
{
	m_pick_distance = 0.f;
	rq_storage.r_clear( );
	can_kill_entity(position, direction, distance, rq_storage);
	if (m_can_kill_member && m_can_kill_enemy)
		return;

	f32					yaw;
	f32					pitch;
	f32					safety_fire_angle = 1.f * PI_DIV_8 * .5f;
	direction.getHP(yaw, pitch);

	direction.setHP(yaw - safety_fire_angle, pitch);
	can_kill_entity(position, direction, distance, rq_storage);
	if (m_can_kill_member && m_can_kill_enemy)
		return;

	direction.setHP(yaw + safety_fire_angle, pitch);
	can_kill_entity(position, direction, distance, rq_storage);
	if (m_can_kill_member && m_can_kill_enemy)
		return;

	direction.setHP(yaw, pitch - safety_fire_angle);
	can_kill_entity(position, direction, distance, rq_storage);
	if (m_can_kill_member && m_can_kill_enemy)
		return;

	direction.setHP(yaw, pitch + safety_fire_angle);
	can_kill_entity(position, direction, distance, rq_storage);
}

IC	f32 CStalker::start_pick_distance( ) const
{
	f32					result = 50.f;
	if (!memory( ).enemy( ).selected( ))
		return				(result);

	return					(
		_max(
		result,
		memory( ).enemy( ).selected( )->Position( ).distance_to(Position( )) + 1.f
		)
		);
}

f32 CStalker::pick_distance( )
{
	if (!inventory( ).ActiveItem( ))
		return				(start_pick_distance( ));

	update_can_kill_info( );
	return					(m_pick_distance);
}

void CStalker::update_can_kill_info( )
{
	if (m_pick_frame_id == Device.dwFrame)
		return;

	m_pick_frame_id = Device.dwFrame;
	m_can_kill_member = false;
	m_can_kill_enemy = false;

	fVector3					position;
	fVector3				direction;
	VERIFY(inventory( ).ActiveItem( ));
	g_fireParams(0, position, direction);
	can_kill_entity_from(position, direction, start_pick_distance( ));
}

bool CStalker::undetected_anomaly( )
{
	return					(inside_anomaly( ) || brain( ).CStalkerPlanner::m_storage.property(StalkerDecisionSpace::eWorldPropertyAnomaly));
}

bool CStalker::inside_anomaly( )
{
	xr_vector<CObject*>::const_iterator	I = feel_touch.begin( );
	xr_vector<CObject*>::const_iterator	E = feel_touch.end( );
	for (; I != E; ++I)
	{
		CCustomZone* zone = smart_cast<CCustomZone*>(*I);
		if (zone)
		{
			if (smart_cast<CRadioactiveZone*>(zone))
				continue;

			return			(true);
		}
	}
	return					(false);
}

bool CStalker::zoom_state( ) const
{
	if (!inventory( ).ActiveItem( ))
		return				(false);

	if ((movement( ).movement_type( ) != eMovementTypeStand) && (movement( ).body_state( ) != eBodyStateCrouch) && !movement( ).path_completed( ))
		return				(false);

	switch (CObjectHandler::planner( ).current_action_state_id( ))
	{
		case ObjectHandlerSpace::eWorldOperatorAim1:
		case ObjectHandlerSpace::eWorldOperatorAim2:
		case ObjectHandlerSpace::eWorldOperatorAimingReady1:
		case ObjectHandlerSpace::eWorldOperatorAimingReady2:
		case ObjectHandlerSpace::eWorldOperatorQueueWait1:
		case ObjectHandlerSpace::eWorldOperatorQueueWait2:
		case ObjectHandlerSpace::eWorldOperatorFire1:
		// we need this 2 operators to prevent fov/range switching
		case ObjectHandlerSpace::eWorldOperatorReload1:
		case ObjectHandlerSpace::eWorldOperatorReload2:
		case ObjectHandlerSpace::eWorldOperatorForceReload1:
		case ObjectHandlerSpace::eWorldOperatorForceReload2:
			return			(true);
	}

	return					(false);
}

void CStalker::update_range_fov(f32& new_range, f32& new_fov, f32 start_range, f32 start_fov)
{
	f32					range = start_range;
	f32					fov = start_fov;

	if (zoom_state( ))
		inventory( ).ActiveItem( )->modify_holder_params(range, fov);

	return					(inherited::update_range_fov(new_range, new_fov, range, fov));
}

bool CStalker::fire_make_sense( )
{
	// if we do not have an enemy
	const CEntityAlive* enemy = memory( ).enemy( ).selected( );
	if (!enemy)
		return				(false);

	if ((pick_distance( ) + PRECISE_DISTANCE) < Position( ).distance_to(enemy->Position( )))
		return				(false);

	if (_abs(Position( ).y - enemy->Position( ).y) > FLOOR_DISTANCE)
		return				(false);

	if (pick_distance( ) < NEAR_DISTANCE)
		return				(false);

	if (memory( ).visual( ).visible_right_now(enemy))
		return				(true);

	u32						last_time_seen = memory( ).visual( ).visible_object_time_last_seen(enemy);
	if (last_time_seen == u32(-1))
		return				(false);

	if (Device.dwTimeGlobal > last_time_seen + FIRE_MAKE_SENSE_INTERVAL)
		return				(false);

	// if we do not have a weapon
	if (!best_weapon( ))
		return				(false);

	// if we do not have automatic weapon
	if (best_weapon( )->object( ).ef_weapon_type( ) != 6)
		return				(false);

	return					(true);
}

// shot effector stuff
void CStalker::on_weapon_shot_start(CWeapon* weapon)
{
	weapon_shot_effector( ).SetRndSeed(m_weapon_shot_random_seed);
	weapon_shot_effector( ).Shot(weapon->camDispersion + weapon->camDispersionInc * f32(weapon->ShotsFired( )));
}

void CStalker::on_weapon_shot_stop(CWeapon* weapon)
{ }

void CStalker::on_weapon_hide(CWeapon* weapon)
{ }

void CStalker::notify_on_wounded_or_killed(CObject* object)
{
	CStalker* stalker = smart_cast<CStalker*>(object);
	if (!stalker)
		return;

	stalker->on_enemy_wounded_or_killed(this);

	typedef CAgentCorpseManager::MEMBER_CORPSES	MEMBER_CORPSES;

	const MEMBER_CORPSES& corpses = agent_manager( ).corpse( ).corpses( );
	if (std::find(corpses.begin( ), corpses.end( ), this) != corpses.end( ))
		return;

	agent_manager( ).corpse( ).register_corpse(this);
}

void CStalker::notify_on_wounded_or_killed( )
{
	ALife::_OBJECT_ID					last_hit_object_id = memory( ).hit( ).last_hit_object_id( );
	if (last_hit_object_id == ALife::_OBJECT_ID(-1))
		return;

	CObject* object = Level( ).Objects.net_Find(last_hit_object_id);
	if (!object)
		return;

	notify_on_wounded_or_killed(object);
}

void CStalker::wounded(bool value)
{
	if (m_wounded == value)
		return;

	if (value)
		notify_on_wounded_or_killed( );

	m_wounded = value;

	if (!m_wounded && g_Alive( ))
		character_physics_support( )->CreateCharacter( );

	if (!m_wounded)
		return;

	character_physics_support( )->movement( )->DestroyCharacter( );

	if (!agent_manager( ).member( ).registered_in_combat(this))
		return;

	agent_manager( ).member( ).unregister_in_combat(this);
}

bool CStalker::wounded(const CRestrictedObject* object) const
{
	if (!wounded( ))
		return				(false);

	VERIFY(object);
	if (!object->accessible(Position( )))
		return				(false);

	if (!object->accessible(ai_location( ).level_vertex_id( )))
		return				(false);

	return					(true);
}

bool CStalker::use_default_throw_force( )
{
	return					(false);
}

bool CStalker::use_throw_randomness( )
{
	return					(false);
}

f32 CStalker::missile_throw_force( )
{
	update_throw_params( );
	VERIFY(_valid(m_throw_force));
	return					(m_throw_force);
}

void CStalker::throw_target(const fVector3& position)
{
	f32					distance_to_sqr = position.distance_to_sqr(m_throw_target);
	m_throw_actual = m_throw_actual && (distance_to_sqr < _sqr(.1f));
	m_throw_target = position;
}

void CStalker::update_throw_params( )
{
	if (m_throw_actual)
	{
		if (m_computed_object_position.similar(Position( )))
		{
			if (m_computed_object_direction.similar(Direction( )))
			{
				VERIFY(_valid(m_throw_force));
				return;
			}
		}
	}

	m_throw_actual = true;
	m_computed_object_position = Position( );
	m_computed_object_direction = Direction( );

	m_throw_position = eye_matrix.c;

	// computing velocity with minimum magnitude
	fVector3					velocity;
	velocity.sub(m_throw_target, m_throw_position);
	f32					time = ThrowMinVelTime(velocity, ph_world->Gravity( ));
	TransferenceToThrowVel(velocity, time, ph_world->Gravity( ));
	m_throw_force = velocity.magnitude( );
	m_throw_direction = velocity.normalize( );
	VERIFY(_valid(m_throw_force));
}

bool CStalker::critically_wounded( )
{
	if (critical_wound_type( ) == critical_wound_type_dummy)
		return					(false);

	if (!brain( ).CStalkerPlanner::m_storage.property(StalkerDecisionSpace::eWorldPropertyCriticallyWounded))
	{
		critical_wounded_state_stop( );
		return						(false);
	}

	return						(true);
}

bool CStalker::critical_wound_external_conditions_suitable( )
{
	if (movement( ).body_state( ) != eBodyStateStand)
		return						(false);

	if (animation( ).non_script_need_update( ))
		return						(false);

	CWeapon* active_weapon = smart_cast<CWeapon*>(inventory( ).ActiveItem( ));
	if (!active_weapon)
		return						(false);

	switch (active_weapon->animation_slot( ))
	{
		case 1: // pistols
		case 2: // automatic weapon
		case 3: // rifles
			break;
		default:
			return					(false);
	}

	if (!agent_manager( ).member( ).registered_in_combat(this))
		return						(false);

//	Msg								("%6d executing critical hit",Device.dwTimeGlobal);
	animation( ).global( ).make_inactual( );
	return							(true);
}

void CStalker::remove_critical_hit( )
{
	brain( ).CStalkerPlanner::m_storage.set_property(
		StalkerDecisionSpace::eWorldPropertyCriticallyWounded,
		false
	);

	animation( ).global( ).remove_callback(
		CStalkerAnimationPair::CALLBACK_ID(
		this,
		&CStalker::remove_critical_hit
	)
	);
}

void CStalker::critical_wounded_state_start( )
{
	brain( ).CStalkerPlanner::m_storage.set_property(
		StalkerDecisionSpace::eWorldPropertyCriticallyWounded,
		true
	);

	animation( ).global( ).add_callback(
		CStalkerAnimationPair::CALLBACK_ID(
		this,
		&CStalker::remove_critical_hit
	)
	);
}

bool CStalker::can_cry_enemy_is_wounded( ) const
{
	if (!brain( ).initialized( ))
		return						(false);

	if (brain( ).current_action_id( ) != StalkerDecisionSpace::eWorldOperatorCombatPlanner)
		return						(false);

	typedef CActionPlannerActionScript<CStalker>	planner_type;
	planner_type* planner = smart_cast<planner_type*>(&brain( ).current_action( ));
	VERIFY(planner);

	switch (planner->current_action_id( ))
	{
		case StalkerDecisionSpace::eWorldOperatorGetReadyToKill:
		case StalkerDecisionSpace::eWorldOperatorGetReadyToDetour:
		case StalkerDecisionSpace::eWorldOperatorKillEnemy:
		case StalkerDecisionSpace::eWorldOperatorTakeCover:
		case StalkerDecisionSpace::eWorldOperatorLookOut:
		case StalkerDecisionSpace::eWorldOperatorHoldPosition:
		case StalkerDecisionSpace::eWorldOperatorDetourEnemy:
		case StalkerDecisionSpace::eWorldOperatorSearchEnemy:
		case StalkerDecisionSpace::eWorldOperatorKillEnemyIfNotVisible:
		case StalkerDecisionSpace::eWorldOperatorKillEnemyIfCriticallyWounded:
			return					(true);
		case StalkerDecisionSpace::eWorldOperatorGetItemToKill:
		case StalkerDecisionSpace::eWorldOperatorRetreatFromEnemy:
		case StalkerDecisionSpace::eWorldOperatorPostCombatWait:
		case StalkerDecisionSpace::eWorldOperatorHideFromGrenade:
		case StalkerDecisionSpace::eWorldOperatorSuddenAttack:
		case StalkerDecisionSpace::eWorldOperatorKillEnemyIfPlayerOnThePath:
		case StalkerDecisionSpace::eWorldOperatorKillWoundedEnemy:
		case StalkerDecisionSpace::eWorldOperatorCriticallyWounded:
			return					(false);
		default: NODEFAULT;
	}
#ifdef DEBUG
	return							(false);
#endif // DEBUG
}

void CStalker::on_critical_wound_initiator(const CStalker* critically_wounded)
{
	if (!can_cry_enemy_is_wounded( ))
		return;

	sound( ).play(eStalkerSoundEnemyCriticallyWounded);
}

void CStalker::on_enemy_wounded_or_killed(const CStalker* wounded_or_killed)
{
	if (!can_cry_enemy_is_wounded( ))
		return;

	sound( ).play(eStalkerSoundEnemyKilledOrWounded);
}

bool CStalker::can_kill_member( )
{
	if (!animation( ).script_animations( ).empty( ))
		return				(false);

	update_can_kill_info( );
	return					(m_can_kill_member);
}

bool CStalker::can_kill_enemy( )
{
	VERIFY(inventory( ).ActiveItem( ));
	update_can_kill_info( );
	return					(m_can_kill_enemy);
}
