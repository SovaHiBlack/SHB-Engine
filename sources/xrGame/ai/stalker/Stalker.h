////////////////////////////////////////////////////////////////////////////
//	Module 		: Stalker.h
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../CustomMonster.h"
#include "../../object_handler.h"
#include "../../AI_PhraseDialogManager.h"
#include "../../StepManager.h"
#include "../../script_export_space.h"

#ifdef DEBUG
template <typename _object_type>
class CActionBase;

template <typename _object_type>
class CPropertyEvaluator;

template <
	typename _object_type,
	bool	 _reverse_search,
	typename _world_operator,
	typename _condition_evaluator,
	typename _world_operator_ptr,
	typename _condition_evaluator_ptr
>
class CActionPlanner;

typedef CActionPlanner<
	CScriptGameObject,
	false,
	CActionBase<CScriptGameObject>,
	CPropertyEvaluator<CScriptGameObject>,
	CActionBase<CScriptGameObject>*,
	CPropertyEvaluator<CScriptGameObject>*
>								script_planner;
#endif

namespace MonsterSpace
{
	enum EMovementDirection;
};

namespace StalkerSpace
{
	enum EBodyAction;
};

enum ECriticalWoundType;

class CALifeSimulator;
class CCharacterPhysicsSupport;
class CWeapon;
class CCoverPoint;
class CCoverEvaluatorCloseToEnemy;
class CCoverEvaluatorFarFromEnemy;
class CCoverEvaluatorBest;
class CCoverEvaluatorAngle;
class CCoverEvaluatorSafe;
class CCoverEvaluatorRandomGame;
class CCoverEvaluatorAmbush;
class CCoverEvaluatorBestByTime;
class CAgentManager;
class CMotionDef;
class CStalkerAnimationManager;
class CStalkerPlanner;
class CSightManager;
class CStalkerMovementManager;
class CStalkerSoundDataVisitor;
class CWeaponShotEffector;
struct SBoneProtections;
class CDangerLocation;
class CRestrictedObject;

class CStalker : public CCustomMonster, public CObjectHandler, public CAI_PhraseDialogManager, public CStepManager
{
private:
	typedef CCustomMonster			inherited;

public:
	using inherited::useful;
	using inherited::evaluate;

private:
	CStalkerAnimationManager* m_animation_manager;
	CStalkerPlanner* m_brain;
	CSightManager* m_sight_manager;
	CStalkerMovementManager* m_movement_manager;

#ifdef DEBUG
	const script_planner* m_debug_planner;
#endif // def DEBUG

	//ALife
private:
	SBoneProtections* m_boneHitProtection;

	//weapon dispersion
private:
	f32								m_disp_walk_stand;
	f32								m_disp_walk_crouch;
	f32								m_disp_run_stand;
	f32								m_disp_run_crouch;
	f32								m_disp_stand_stand;
	f32								m_disp_stand_crouch;
	f32								m_disp_stand_stand_zoom;
	f32								m_disp_stand_crouch_zoom;

private:
	f32								m_power_fx_factor;

private:
	f32								m_fRankDisperison;
	f32								m_fRankVisibility;
	f32								m_fRankImmunity;

	//best item/ammo selection members
public:
	bool							m_item_actuality;
	CInventoryItem* m_best_item_to_kill;
	f32								m_best_item_value;
	CInventoryItem* m_best_ammo;
	const CInventoryItem* m_best_found_item_to_kill;
	const CInventoryItem* m_best_found_ammo;

	//covers being used
public:
	CCoverEvaluatorCloseToEnemy* m_ce_close;
	CCoverEvaluatorFarFromEnemy* m_ce_far;
	CCoverEvaluatorBest* m_ce_best;
	CCoverEvaluatorAngle* m_ce_angle;
	CCoverEvaluatorSafe* m_ce_safe;
	CCoverEvaluatorRandomGame* m_ce_random_game;
	CCoverEvaluatorAmbush* m_ce_ambush;
	CCoverEvaluatorBestByTime* m_ce_best_by_time;

	//physics support
public:
	CCharacterPhysicsSupport* m_pPhysics_support;

public:
	bool							m_wounded;

public:
	CStalker( );
	virtual										~CStalker( );

public:
	virtual CCharacterPhysicsSupport* character_physics_support( )
	{
		return m_pPhysics_support;
	}
	virtual CPHDestroyable* ph_destroyable( );
	virtual CAttachmentOwner* cast_attachment_owner( )
	{
		return this;
	}
	virtual CInventoryOwner* cast_inventory_owner( )
	{
		return this;
	}
	virtual CEntityAlive* cast_entity_alive( )
	{
		return this;
	}
	virtual CEntity* cast_entity( )
	{
		return this;
	}
	virtual CGameObject* cast_game_object( )
	{
		return this;
	}
	virtual CPhysicsShellHolder* cast_physics_shell_holder( )
	{
		return this;
	}
	virtual CParticlesPlayer* cast_particles_player( )
	{
		return this;
	}
	virtual Feel::Sound* dcast_FeelSound( )
	{
		return this;
	}
	virtual CStalker* cast_stalker( )
	{
		return this;
	}
	virtual CCustomMonster* cast_custom_monster( )
	{
		return this;
	}
	virtual CScriptEntity* cast_script_entity( )
	{
		return this;
	}

public:
	void										init( );
	virtual void								Load(pcstr section);
	virtual void								reinit( );
	virtual void								reload(pcstr section);
	virtual void								LoadSounds(pcstr section);

	virtual BOOL								net_Spawn(CSE_Abstract* DC);
	virtual void								net_Export(CNetPacket& P);
	virtual void								net_Import(CNetPacket& P);
	virtual void								net_Destroy( );
	virtual void								net_Save(CNetPacket& P);
	virtual BOOL								net_SaveRelevant( );
	virtual void								net_Relcase(CObject* O);

	//save/load server serialization
	virtual void								save(CNetPacket& output_packet);
	virtual void								load(IReader& input_packet);

	virtual void								UpdateCL( );
	virtual void								shedule_Update(u32 dt);
	virtual void								Think( );
	virtual void								SelectAnimation(const fVector3& _view, const fVector3& _move, f32 speed);
	virtual BOOL								UsedAI_Locations( );

	virtual void								g_WeaponBones(s32& L, s32& R1, s32& R2);
	virtual void								g_fireParams(const CHudItem* pHudItem, fVector3& P, fVector3& D);
	virtual void								HitSignal(f32 P, fVector3& vLocalDir, CObject* who, s16 element);
	virtual void								Die(CObject* who);

	virtual void								OnEvent(CNetPacket& P, u16 type);
	virtual void								feel_touch_new(CObject* O);

	virtual void								renderable_Render( );
	virtual void								Exec_Look(f32 dt);
	virtual void								Hit(SHit* pHDS);
	virtual void								PHHit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual BOOL								feel_vision_isRelevant(CObject* who);
	virtual f32									Radius( ) const;

#ifdef DEBUG
	virtual void								OnHUDDraw(CCustomHUD* hud);
	virtual void								OnRender( );
#endif // def DEBUG

	virtual bool								useful(const CItemManager* manager, const CGameObject* object) const;
	virtual f32									evaluate(const CItemManager* manager, const CGameObject* object) const;
	virtual bool								useful(const CEnemyManager* manager, const CEntityAlive* object) const;

	//Dialogs
	virtual void								UpdateAvailableDialogs(CPhraseDialogManager* partner);

	//scripts
	virtual CWeapon* GetCurrentWeapon( ) const;
	virtual u32									GetWeaponAmmo( ) const;
	virtual CInventoryItem* GetMedikit( ) const;
	virtual CInventoryItem* GetFood( ) const;
	virtual bool								bfAssignMovement(CScriptEntityAction* tpEntityAction);
	virtual bool								bfAssignWatch(CScriptEntityAction* tpEntityAction);
	virtual void								ResetScriptData(pvoid P = 0);
	virtual bool								bfAssignObject(CScriptEntityAction* tpEntityAction);
	virtual bool								bfAssignAnimation(CScriptEntityAction* tpEntityAction);

	//physics
	virtual u16									PHGetSyncItemsNumber( )
	{
		return inherited::PHGetSyncItemsNumber( );
	}
	virtual CPHSynchronize* PHGetSyncItem(u16 item)
	{
		return inherited::PHGetSyncItem(item);
	}
	virtual void								PHUnFreeze( )
	{
		return inherited::PHUnFreeze( );
	}
	virtual void								PHFreeze( )
	{
		return inherited::PHFreeze( );
	}

	//miscellanious functions
	void										DropItemSendMessage(CObject* O);
	bool										bfCheckForNodeVisibility(u32 dwNodeID, bool bIfRyPick = false);
	virtual ALife::ERelationType				tfGetRelationType(const CEntityAlive* tpEntityAlive) const;
	virtual const SRotation						Orientation( ) const;
	virtual const MonsterSpace::SBoneRotation& head_orientation( ) const;

	//InventoryOwner stuff
	virtual bool								CanPutInSlot(PIItem item, u32 slot)
	{
		return(slot != OUTFIT_SLOT);
	}

//////////////////////////////////////////////////////////////////////////
// action/evaluators support functions
//////////////////////////////////////////////////////////////////////////
public:
	virtual void								OnItemTake(CInventoryItem* inventory_item);
	virtual void								OnItemDrop(CInventoryItem* inventory_item);
	bool										item_to_kill( );
	bool										item_can_kill( );
	bool										remember_item_to_kill( );
	bool										remember_ammo( );
	bool										ready_to_kill( );
	bool										ready_to_detour( );
	void										update_best_item_info( );
	virtual f32									GetWeaponAccuracy( ) const;
	virtual void								spawn_supplies( );
	IC CAgentManager& agent_manager( ) const;

	virtual bool								human_being( ) const
	{
		return true;
	}

	bool										undetected_anomaly( );
	bool										inside_anomaly( );

private:
	bool							m_can_kill_member;
	bool							m_can_kill_enemy;
	f32								m_pick_distance;
	u32								m_pick_frame_id;
	collide::rq_results				rq_storage;

private:
	void										can_kill_entity(const fVector3& position, const fVector3& direction, f32 distance, collide::rq_results& rq_storage);
	void										can_kill_entity_from(const fVector3& position, fVector3 direction, f32 distance);
	void										update_can_kill_info( );

public:
	bool										can_kill_member( );
	bool										can_kill_enemy( );
	f32											pick_distance( );
	IC f32										start_pick_distance( ) const;
	bool										fire_make_sense( );

	virtual pcstr								Name( ) const;
	virtual BOOL								feel_touch_contact(CObject* O);
	virtual BOOL								feel_touch_on_contact(CObject* O);

	//�����, ����� �������� �������� ����� �� ��������� � ��������
	//(�����, �������� � �.�.)
	flags32							m_actor_relation_flags;

	// ALife
private:
	struct STradeItem
	{
		CInventoryItem* m_item;
		ALife::_OBJECT_ID				m_owner_id;
		ALife::_OBJECT_ID				m_new_owner_id;

		IC					STradeItem(CInventoryItem* item,
									   ALife::_OBJECT_ID owner_id,
									   ALife::_OBJECT_ID new_owner_id
		)
		{
			m_item = item;
			m_owner_id = owner_id;
			m_new_owner_id = new_owner_id;
		}

		IC bool			operator<	(const STradeItem& trade_item) const;
		IC bool			operator==	(u16 id) const;
	};

private:
	CGameObject* m_trader_game_object;
	CInventoryOwner* m_current_trader;
	xr_vector<STradeItem>				m_temp_items;
	u32									m_total_money;
	bool								m_sell_info_actuality;

protected:
	u32							fill_items(CInventory& inventory, CGameObject* old_owner, ALife::_OBJECT_ID new_owner_id);

	IC void						buy_item_virtual(STradeItem& item);
	void						attach_available_ammo(CWeapon* weapon);
	void						choose_food( );
	void						choose_weapon(ALife::EWeaponPriorityType weapon_priority_type);
	void						choose_medikit( );
	void						choose_detector( );
	void						choose_equipment( );

	void						select_items( );
	void						transfer_item(CInventoryItem* item, CGameObject* old_owner, CGameObject* new_owner);

	void						update_sell_info( );
	bool						tradable_item(CInventoryItem* inventory_item, const u16& current_owner_id);
	bool						can_sell(CInventoryItem const* item);
	bool						can_take(CInventoryItem const* item);

	bool						non_conflicted(const CInventoryItem* item, const CWeapon* new_weapon) const;
	bool						enough_ammo(const CWeapon* new_weapon) const;
	bool						conflicted(const CInventoryItem* item, const CWeapon* new_weapon, bool new_wepon_enough_ammo, s32 new_weapon_rank) const;
	void						update_conflicted(CInventoryItem* item, const CWeapon* new_weapon);
	void						remove_personal_only_ammo(const CInventoryItem* item);
	void						on_after_take(const CGameObject* object);
	virtual bool						AllowItemToTrade(CInventoryItem const* item, EItemPlace place) const;

public:
	IC CStalkerAnimationManager& animation( ) const;
	IC CStalkerPlanner& brain( ) const;
	IC CSightManager& sight( ) const;

private:
	CStalkerSoundDataVisitor* m_sound_user_data_visitor;

protected:
	virtual CSound_UserDataVisitor* create_sound_visitor( );
	virtual CMemoryManager* create_memory_manager( );
	virtual CMovementManager* create_movement_manager( );

public:
	IC CStalkerMovementManager& movement( ) const;
	virtual DLL_Pure* _construct( );

private:
	IC bool						frame_check(u32& frame);
	virtual bool						natural_weapon( ) const
	{
		return false;
	}
	virtual bool						natural_detector( ) const
	{
		return false;
	}
	virtual bool						use_center_to_aim( ) const;
	void						process_enemies( );

private:
	bool								m_group_behaviour;

public:
	IC bool						group_behaviour( ) const;
	virtual void						update_range_fov(f32& new_range, f32& new_fov, f32 start_range, f32 start_fov);
	void __stdcall				update_object_handler( );
	bool						zoom_state( ) const;
	void						react_on_grenades( );
	void						react_on_member_death( );

private:
	CWeaponShotEffector* m_weapon_shot_effector;
	s32									m_weapon_shot_random_seed;

public:
	virtual void						on_weapon_shot_start(CWeapon* weapon);
	virtual void						on_weapon_shot_stop(CWeapon* weapon);
	virtual void						on_weapon_hide(CWeapon* weapon);
	IC		CWeaponShotEffector& weapon_shot_effector( ) const;
	IC		fVector3						weapon_shot_effector_direction(const fVector3& current) const;
	virtual void						UpdateCamera( );
	virtual bool						can_attach(const CInventoryItem* inventory_item) const;
	virtual bool						use_simplified_visual( ) const
	{
		return already_dead( );
	}

#ifdef DEBUG
	void						debug_planner(const script_planner* planner);
#endif

private:
	u32				m_min_queue_size_far;
	u32				m_max_queue_size_far;
	u32				m_min_queue_interval_far;
	u32				m_max_queue_interval_far;

	u32				m_min_queue_size_medium;
	u32				m_max_queue_size_medium;
	u32				m_min_queue_interval_medium;
	u32				m_max_queue_interval_medium;

	u32				m_min_queue_size_close;
	u32				m_max_queue_size_close;
	u32				m_min_queue_interval_close;
	u32				m_max_queue_interval_close;

public:
	IC u32							min_queue_size_far( ) const;
	IC u32							max_queue_size_far( ) const;
	IC u32							min_queue_interval_far( ) const;
	IC u32							max_queue_interval_far( ) const;

	IC u32							min_queue_size_medium( ) const;
	IC u32							max_queue_size_medium( ) const;
	IC u32							min_queue_interval_medium( ) const;
	IC u32							max_queue_interval_medium( ) const;

	IC u32							min_queue_size_close( ) const;
	IC u32							max_queue_size_close( ) const;
	IC u32							min_queue_interval_close( ) const;
	IC u32							max_queue_interval_close( ) const;

public:
	typedef fastdelegate::FastDelegate<void(const CCoverPoint*, const CCoverPoint*)>	on_best_cover_changed_delegate;

private:
	typedef xr_vector<on_best_cover_changed_delegate>	cover_delegates;

private:
	cover_delegates						m_cover_delegates;
	const CCoverPoint* m_best_cover;
	f32									m_best_cover_value;
	bool								m_best_cover_actual;
	bool								m_best_cover_can_try_advance;
	const CCoverPoint* m_best_cover_advance_cover;

private:
	f32						best_cover_value(const fVector3& position_to_cover_from);
	const CCoverPoint* find_best_cover(const fVector3& position_to_cover_from);
	void						update_best_cover_actuality(const fVector3& position_to_cover_from);
	void						on_best_cover_changed(const CCoverPoint* new_cover, const CCoverPoint* old_cover);

public:
	void						best_cover_can_try_advance( );
	const CCoverPoint* best_cover(const fVector3& position_to_cover_from);

public:
	void						subscribe_on_best_cover_changed(const on_best_cover_changed_delegate& delegate);
	void						unsubscribe_on_best_cover_changed(const on_best_cover_changed_delegate& delegate);

public:
	virtual void						on_enemy_change(const CEntityAlive* enemy);
	virtual void						on_restrictions_change( );
	void						on_cover_blocked(const CCoverPoint* cover);
	void						on_danger_location_add(const CDangerLocation& location);
	void						on_danger_location_remove(const CDangerLocation& location);

public:
	void						wounded(bool value);
	bool						wounded(const CRestrictedObject* object) const;
	IC bool						wounded( ) const;

// throwing grenades
private:
	// actuality parameters
	bool								m_throw_actual;
	fVector3								m_computed_object_position;
	fVector3								m_computed_object_direction;
	// target parameters
	fVector3								m_throw_target;
	// computed
	f32								m_throw_force;
	fVector3								m_throw_position;
	fVector3								m_throw_direction;

public:
	virtual bool						use_default_throw_force( );
	virtual f32						missile_throw_force( );
	virtual bool						use_throw_randomness( );
	void						throw_target(const fVector3& position);
	void						update_throw_params( );

#ifdef DEBUG
public:
	void						dbg_draw_vision( );
	void						dbg_draw_visibility_rays( );
#endif

	//////////////////////////////////////////////////////////////////////////
	// Critical Wounds
	//////////////////////////////////////////////////////////////////////////	
private:
	virtual void						load_critical_wound_bones( );
	virtual bool						critical_wound_external_conditions_suitable( );
	virtual void						critical_wounded_state_start( );

	void						fill_bones_body_parts(pcstr bone_id, const ECriticalWoundType& wound_type);

public:
	typedef xr_vector<f32>			CRITICAL_WOUND_WEIGHTS;

private:
	CRITICAL_WOUND_WEIGHTS				m_critical_wound_weights;

public:
	bool						critically_wounded( );
	IC const CRITICAL_WOUND_WEIGHTS& critical_wound_weights( ) const;

private:
	bool						can_cry_enemy_is_wounded( ) const;
	void						on_critical_wound_initiator(const CStalker* critically_wounded);
	void						on_enemy_wounded_or_killed(const CStalker* wounded_or_killed);
	void						notify_on_wounded_or_killed(CObject* object);
	void						notify_on_wounded_or_killed( );
	void xr_stdcall			remove_critical_hit( );
//////////////////////////////////////////////////////////////////////////
private:
	bool	m_registered_in_combat_on_migration;

public:
	virtual void						on_before_change_team( );
	virtual void						on_after_change_team( );

private:
	bool	m_sight_enabled_before_animation_controller;

public:
	virtual void						create_anim_mov_ctrl(CBlend* b);
	virtual void						destroy_anim_mov_ctrl( );

private:
	bool	m_can_select_items;

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CStalker)
#undef script_type_list
#define script_type_list save_type_list(CStalker)

#include "ai_stalker_inline.h"
