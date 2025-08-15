////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object.h
//	Description : Script game object class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space_forward.h"
#include "script_bind_macroses.h"
#include "script_export_space.h"
#include "xr_time.h"
#include "CharacterInfo_defs.h"

enum EPdaMsg;
enum ESoundTypes;
enum ETaskState;

namespace ALife
{
	enum ERelationType;
};

namespace ScriptEntity
{
	enum EActionType;
};

namespace MovementManager
{
	enum EPathType;
};

namespace DetailPathManager
{
	enum EDetailPathType;
};

namespace SightManager
{
	enum ESightType;
};

class CNetPacket;
class CGameTask;

namespace PatrolPathManager
{
	enum EPatrolStartType;
	enum EPatrolRouteType;
};

namespace MemorySpace
{
	struct SMemoryInfo;
	struct CVisibleObject;
	struct CSoundObject;
	struct CHitObject;
	struct CNotYetVisibleObject;
};

namespace MonsterSpace
{
	enum EBodyState;
	enum EMovementType;
	enum EMovementDirection;
	enum EDirectionType;
	enum EPathState;
	enum EObjectAction;
	enum EMentalState;
	enum EScriptMonsterMoveAction;
	enum EScriptMonsterSpeedParam;
	enum EScriptMonsterAnimAction;
	enum EScriptMonsterGlobalAction;
	enum EScriptSoundAnim;
	enum EMonsterSounds;
	enum EMonsterHeadAnimType;
	struct SBoneRotation;
};

namespace GameObject
{
	enum ECallbackType;
};

class CGameObject;
class CScriptHit;
class CScriptEntityAction;
class CScriptTask;
class CScriptSoundInfo;
class CScriptMonsterHitInfo;
class CScriptBinderObject;
class CCoverPoint;
class CScriptIniFile;
class CPhysicsShell;
class CHelicopter;
class CHangingLamp;
class CHolderCustom;
struct ScriptCallbackInfo;
struct STasks;
class CCar;
class CDangerObject;
class CScriptGameObject;

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

class CScriptGameObject;

namespace SightManager
{
	enum ESightType;
}

struct CSightParams
{
	SightManager::ESightType	m_sight_type;
	CScriptGameObject* m_object;
	fVector3						m_vector;
};

class CScriptGameObject
{
	mutable CGameObject* m_game_object;

public:
	CScriptGameObject(CGameObject* tpGameObject);
	virtual					~CScriptGameObject( );
	operator CObject* ();

	IC		CGameObject& object( ) const;
	CScriptGameObject* Parent( ) const;
	void				Hit(CScriptHit* tLuaHit);
	s32					clsid( ) const;
	void				play_cycle(pcstr anim, bool mix_in);
	void				play_cycle(pcstr anim);
	fVector3				Center( );
	_DECLARE_FUNCTION10(Position, fVector3);
	_DECLARE_FUNCTION10(Direction, fVector3);
	_DECLARE_FUNCTION10(Mass, f32);
	_DECLARE_FUNCTION10(ID, u32);
	_DECLARE_FUNCTION10(getVisible, BOOL);
	_DECLARE_FUNCTION10(getEnabled, BOOL);
	_DECLARE_FUNCTION10(story_id, ALife::_STORY_ID);

	pcstr				Name( ) const;
	shared_str			cName( ) const;
	pcstr				Section( ) const;
// CInventoryItem
	u32					Cost( ) const;
	f32				GetCondition( ) const;
	void				SetCondition(f32 val);

// CEntity
	_DECLARE_FUNCTION10(DeathTime, u32);
	_DECLARE_FUNCTION10(MaxHealth, f32);
	_DECLARE_FUNCTION10(Accuracy, f32);
	_DECLARE_FUNCTION10(Team, s32);
	_DECLARE_FUNCTION10(Squad, s32);
	_DECLARE_FUNCTION10(Group, s32);

	void				Kill(CScriptGameObject* who);

// CEntityAlive
	_DECLARE_FUNCTION10(GetFOV, f32);
	_DECLARE_FUNCTION10(GetRange, f32);
	_DECLARE_FUNCTION10(GetHealth, f32);
	_DECLARE_FUNCTION10(GetPsyHealth, f32);
	_DECLARE_FUNCTION10(GetPower, f32);
	_DECLARE_FUNCTION10(GetRadiation, f32);
	_DECLARE_FUNCTION10(GetBleeding, f32);
	_DECLARE_FUNCTION10(GetMorale, f32);

	_DECLARE_FUNCTION11(SetHealth, void, f32);
	_DECLARE_FUNCTION11(SetPsyHealth, void, f32);
	_DECLARE_FUNCTION11(SetPower, void, f32);
//	_DECLARE_FUNCTION11	(SetSatiety,		void, f32);
	_DECLARE_FUNCTION11(SetRadiation, void, f32);
	_DECLARE_FUNCTION11(SetCircumspection, void, f32);
	_DECLARE_FUNCTION11(SetMorale, void, f32);

	void				set_fov(f32 new_fov);
	void				set_range(f32 new_range);
	bool				Alive( ) const;
	ALife::ERelationType	GetRelationType(CScriptGameObject* who);

// CScriptEntity

	_DECLARE_FUNCTION12(SetScriptControl, void, bool, pcstr);
	_DECLARE_FUNCTION10(GetScriptControl, bool);
	_DECLARE_FUNCTION10(GetScriptControlName, pcstr);
	_DECLARE_FUNCTION10(GetEnemyStrength, s32);
	_DECLARE_FUNCTION10(can_script_capture, bool);


	CScriptEntityAction* GetCurrentAction( ) const;
	void				AddAction(const CScriptEntityAction* tpEntityAction, bool bHighPriority = false);
	void				ResetActionQueue( );
// Actor only
	void				SetActorPosition(fVector3 pos);
	void				SetActorDirection(f32 dir);
// CCustomMonster
	bool				CheckObjectVisibility(const CScriptGameObject* tpLuaGameObject);
	bool				CheckTypeVisibility(pcstr section_name);
	pcstr				WhoHitName( );
	pcstr				WhoHitSectionName( );

	void				ChangeTeam(u8 team, u8 squad, u8 group);

// CStalker
	CScriptGameObject* GetCurrentWeapon( ) const;
	CScriptGameObject* GetFood( ) const;
	CScriptGameObject* GetMedikit( ) const;

// CBloodsucker
	void				set_invisible(bool val);
	bool				get_invisible( );
	void				set_manual_invisibility(bool val);
	void				set_alien_control(bool val);

// Zombie
	bool				fake_death_fall_down( );
	void				fake_death_stand_up( );

// CBaseMonster
	void				skip_transfer_enemy(bool val);
	void				set_home(pcstr name, f32 r_min, f32 r_max, bool aggressive);
	void				remove_home( );
	void				berserk( );
	void				set_custom_panic_threshold(f32 value);
	void				set_default_panic_threshold( );

	// CTrader
	void				set_trader_global_anim(pcstr anim);
	void				set_trader_head_anim(pcstr anim);
	void				set_trader_sound(pcstr sound, pcstr anim);
	void				external_sound_start(pcstr sound);
	void				external_sound_stop( );

	template <typename T>
	IC		T* action_planner( );

	// CProjector
	fVector3				GetCurrentDirection( );
	bool				IsInvBoxEmpty( );
	//передача порции информации InventoryOwner
	bool				GiveInfoPortion(pcstr info_id);
	bool				DisableInfoPortion(pcstr info_id);
	bool				GiveGameNews(pcstr news, pcstr texture_name, fRect tex_rect, s32 delay, s32 show_time);

	void				AddIconedTalkMessage(pcstr text, pcstr texture_name, fRect tex_rect, pcstr templ_name);
	//предикаты наличия/отсутствия порции информации у персонажа
	bool				HasInfo(pcstr info_id);
	bool				DontHasInfo(pcstr info_id);
	СTime				GetInfoTime(pcstr info_id);
	//работа с заданиями
	ETaskState			GetGameTaskState(pcstr task_id, s32 objective_num);
	void				SetGameTaskState(ETaskState state, pcstr task_id, s32 objective_num);
	void				GiveTaskToActor(CGameTask* t, u32 dt, bool bCheckExisting);

	bool				IsTalking( );
	void				StopTalk( );
	void				EnableTalk( );
	void				DisableTalk( );
	bool				IsTalkEnabled( );

	void				EnableTrade( );
	void				DisableTrade( );
	bool				IsTradeEnabled( );

	void				IterateInventory(luabind::functor<void> functor, luabind::object object);
	void				MarkItemDropped(CScriptGameObject* item);
	bool				MarkedDropped(CScriptGameObject* item);
	void				UnloadMagazine( );

	void				DropItem(CScriptGameObject* pItem);
	void				DropItemAndTeleport(CScriptGameObject* pItem, fVector3 position);
	void				ForEachInventoryItems(const luabind::functor<void>& functor);
	void				TransferItem(CScriptGameObject* pItem, CScriptGameObject* pForWho);
	void				TransferMoney(s32 money, CScriptGameObject* pForWho);
	void				GiveMoney(s32 money);
	u32					Money( );

	void				SetRelation(ALife::ERelationType relation, CScriptGameObject* pWhoToSet);

	s32					GetAttitude(CScriptGameObject* pToWho);

	s32					GetGoodwill(CScriptGameObject* pToWho);
	void				SetGoodwill(s32 goodwill, CScriptGameObject* pWhoToSet);
	void				ChangeGoodwill(s32 delta_goodwill, CScriptGameObject* pWhoToSet);

	void				SetStartDialog(pcstr dialog_id);
	void				GetStartDialog( );
	void				RestoreDefaultStartDialog( );

	void				SwitchToTrade( );
	void				SwitchToTalk( );
	void				RunTalkDialog(CScriptGameObject* pToWho);

	void				HideWeapon( );
	void				RestoreWeapon( );


	pcstr				ProfileName( );
	pcstr				CharacterName( );
	pcstr				CharacterCommunity( );
	s32					CharacterRank( );
	s32					CharacterReputation( );

	void SetCharacterRank(s32);
	void ChangeCharacterRank(s32);
	void ChangeCharacterReputation(s32);
	void SetCharacterCommunity(pcstr, s32, s32);

	u32					GetInventoryObjectCount( ) const;

	CScriptGameObject* GetActiveItem( );

	CScriptGameObject* GetObjectByName(pcstr caObjectName) const;
	CScriptGameObject* GetObjectByIndex(s32 iIndex) const;

// Callbacks			
	void				SetCallback(GameObject::ECallbackType type, const luabind::functor<void>& functor);
	void				SetCallback(GameObject::ECallbackType type, const luabind::functor<void>& functor, const luabind::object& object);
	void				SetCallback(GameObject::ECallbackType type);

	void				set_patrol_extrapolate_callback(const luabind::functor<bool>& functor);
	void				set_patrol_extrapolate_callback(const luabind::functor<bool>& functor, const luabind::object& object);
	void				set_patrol_extrapolate_callback( );

	void				set_enemy_callback(const luabind::functor<bool>& functor);
	void				set_enemy_callback(const luabind::functor<bool>& functor, const luabind::object& object);
	void				set_enemy_callback( );

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////use calback///////////////////////////////////////////////
	void				SetTipText(pcstr tip_text);
	void				SetTipTextDefault( );
	void				SetNonscriptUsable(bool nonscript_usable);
///////////////////////////////////////////////////////////////////////////////////////////
	void				set_fastcall(const luabind::functor<bool>& functor, const luabind::object& object);
	void				set_const_force(const fVector3& dir, f32 value, u32  time_interval);
//////////////////////////////////////////////////////////////////////////

	pcstr				GetPatrolPathName( );
	u32					GetAmmoElapsed( );
	void				SetAmmoElapsed(s32 ammo_elapsed);
	u32					GetAmmoCurrent( ) const;
	void				SetQueueSize(u32 queue_size);
	CScriptGameObject* GetBestEnemy( );
	const CDangerObject* GetBestDanger( );
	CScriptGameObject* GetBestItem( );

	_DECLARE_FUNCTION10(GetActionCount, u32);

	const				CScriptEntityAction* GetActionByIndex(u32 action_index = 0);

//////////////////////////////////////////////////////////////////////////
// Inventory Owner
//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
	flags32				get_actor_relation_flags( )			const;
	void 				set_actor_relation_flags(flags32);
	pcstr				sound_voice_prefix( )			const;

	//////////////////////////////////////////////////////////////////////////
	u32						memory_time(const CScriptGameObject& lua_game_object);
	fVector3					memory_position(const CScriptGameObject& lua_game_object);
	CScriptGameObject* best_weapon( );
	void					explode(u32 level_time);
	CScriptGameObject* GetEnemy( ) const;
	CScriptGameObject* GetCorpse( ) const;
	CScriptSoundInfo		GetSoundInfo( );
	CScriptMonsterHitInfo	GetMonsterHitInfo( );
	void					bind_object(CScriptBinderObject* object);
	CScriptGameObject* GetCurrentOutfit( ) const;
	f32					GetCurrentOutfitProtection(s32 hit_type);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
	void				set_body_state(MonsterSpace::EBodyState body_state);
	void				set_movement_type(MonsterSpace::EMovementType movement_type);
	void				set_mental_state(MonsterSpace::EMentalState mental_state);
	void				set_path_type(MovementManager::EPathType path_type);
	void				set_detail_path_type(DetailPathManager::EDetailPathType detail_path_type);

	MonsterSpace::EBodyState			body_state( ) const;
	MonsterSpace::EBodyState			target_body_state( ) const;
	MonsterSpace::EMovementType			movement_type( ) const;
	MonsterSpace::EMovementType			target_movement_type( ) const;
	MonsterSpace::EMentalState			mental_state( ) const;
	MonsterSpace::EMentalState			target_mental_state( ) const;
	MovementManager::EPathType			path_type( ) const;
	DetailPathManager::EDetailPathType	detail_path_type( ) const;

	u32					add_sound(pcstr prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, pcstr bone_name);
	u32					add_sound(pcstr prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type);
	u32					add_sound(pcstr prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, pcstr bone_name, pcstr head_anim);
	void				remove_sound(u32 internal_type);
	void				set_sound_mask(u32 sound_mask);
	void				set_sight(SightManager::ESightType sight_type, const fVector3* vector3d, u32 dwLookOverDelay);
	void				set_sight(SightManager::ESightType sight_type, bool torso_look, bool path);
	void				set_sight(SightManager::ESightType sight_type, const fVector3& vector3d, bool torso_look);
	void 				set_sight(SightManager::ESightType sight_type, const fVector3* vector3d);
	void 				set_sight(CScriptGameObject* object_to_look);
	void 				set_sight(CScriptGameObject* object_to_look, bool torso_look);
	void 				set_sight(CScriptGameObject* object_to_look, bool torso_look, bool fire_object);
	void 				set_sight(CScriptGameObject* object_to_look, bool torso_look, bool fire_object, bool no_pitch);
	void 				set_sight(const MemorySpace::SMemoryInfo* memory_object, bool	torso_look);
	CHARACTER_RANK_VALUE GetRank( );
	void				play_sound(u32 internal_type);
	void				play_sound(u32 internal_type, u32 max_start_time);
	void				play_sound(u32 internal_type, u32 max_start_time, u32 min_start_time);
	void				play_sound(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time);
	void				play_sound(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time);
	void				play_sound(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id);

	void				set_item(MonsterSpace::EObjectAction object_action);
	void				set_item(MonsterSpace::EObjectAction object_action, CScriptGameObject* game_object);
	void				set_item(MonsterSpace::EObjectAction object_action, CScriptGameObject* game_object, u32 queue_size);
	void				set_item(MonsterSpace::EObjectAction object_action, CScriptGameObject* game_object, u32 queue_size, u32 queue_interval);
	void				set_desired_position( );
	void				set_desired_position(const fVector3* desired_position);
	void				set_desired_direction( );
	void				set_desired_direction(const fVector3* desired_direction);
	void				set_patrol_path(pcstr path_name, const PatrolPathManager::EPatrolStartType patrol_start_type, const PatrolPathManager::EPatrolRouteType patrol_route_type, bool random);
	void				set_dest_level_vertex_id(u32 level_vertex_id);
	u32					level_vertex_id( ) const;
	f32				level_vertex_light(const u32& level_vertex_id) const;
	u32					game_vertex_id( ) const;
	void				add_animation(pcstr animation, bool hand_usage, bool use_movement_controller);
	void				clear_animations( );
	s32					animation_count( ) const;
	s32					animation_slot( ) const;
	CScriptBinderObject* binded_object( );
	void				set_previous_point(s32 point_index);
	void				set_start_point(s32 point_index);
	u32					get_current_patrol_point_index( );
	bool				path_completed( ) const;
	void				patrol_path_make_inactual( );
	void				extrapolate_length(f32 extrapolate_length);
	f32				extrapolate_length( ) const;
	void				enable_memory_object(CScriptGameObject* object, bool enable);
	s32					active_sound_count( );
	s32					active_sound_count(bool only_playing);
	const CCoverPoint* best_cover(const fVector3& position, const fVector3& enemy_position, f32 radius, f32 min_enemy_distance, f32 max_enemy_distance);
	const CCoverPoint* safe_cover(const fVector3& position, f32 radius, f32 min_distance);
	CScriptIniFile* spawn_ini( ) const;
	bool				active_zone_contact(u16 id);

	///
	void				add_restrictions(pcstr out, pcstr in);
	void				remove_restrictions(pcstr out, pcstr in);
	void				remove_all_restrictions( );
	pcstr				in_restrictions( );
	pcstr				out_restrictions( );
	pcstr				base_in_restrictions( );
	pcstr				base_out_restrictions( );
	bool				accessible_position(const fVector3& position);
	bool				accessible_vertex_id(u32 level_vertex_id);
	u32					accessible_nearest(const fVector3& position, fVector3& result);

	const xr_vector<MemorySpace::CVisibleObject>& memory_visible_objects( ) const;
	const xr_vector<MemorySpace::CSoundObject>& memory_sound_objects( ) const;
	const xr_vector<MemorySpace::CHitObject>& memory_hit_objects( ) const;
	const xr_vector<MemorySpace::CNotYetVisibleObject>& not_yet_visible_objects( ) const;
	f32				visibility_threshold( ) const;
	void				enable_vision(bool value);
	bool				vision_enabled( ) const;
	void				set_sound_threshold(f32 value);
	void				restore_sound_threshold( );
	//////////////////////////////////////////////////////////////////////////
	void				enable_attachable_item(bool value);
	bool				attachable_item_enabled( ) const;
	// CustomZone
	void				EnableAnomaly( );
	void				DisableAnomaly( );
	f32				GetAnomalyPower( );
	void				SetAnomalyPower(f32 p);

	// HELICOPTER
	CHelicopter* get_helicopter( );
	//CAR
	CCar* get_car( );
	//LAMP
	CHangingLamp* get_hanging_lamp( );
	CHolderCustom* get_custom_holder( );
	CHolderCustom* get_current_holder( ); //actor only

	fVector3				bone_position(pcstr bone_name) const;
	bool				is_body_turning( ) const;
	CPhysicsShell* get_physics_shell( ) const;
	bool				weapon_strapped( ) const;
	bool				weapon_unstrapped( ) const;
	void				eat(CScriptGameObject* item);
	bool				inside(const fVector3& position, f32 epsilon) const;
	bool				inside(const fVector3& position) const;

	fVector3				head_orientation( ) const;
	u32					vertex_in_direction(u32 level_vertex_id, fVector3 direction, f32 max_distance) const;

	void				info_add(pcstr text);
	void				info_clear( );

	// Monster Jumper
	void				jump(const fVector3& position, f32 factor);

	void				set_ignore_monster_threshold(f32 ignore_monster_threshold);
	void				restore_ignore_monster_threshold( );
	f32				ignore_monster_threshold( ) const;
	void				set_max_ignore_monster_distance(const f32& max_ignore_monster_distance);
	void				restore_max_ignore_monster_distance( );
	f32				max_ignore_monster_distance( ) const;

	void				make_object_visible_somewhen(CScriptGameObject* object);

	CScriptGameObject* item_in_slot(u32 slot_id) const;
	u32					active_slot( );
	void				activate_slot(u32 slot_id);

#ifdef DEBUG
	void				debug_planner(const script_planner* planner);
#endif

	void				sell_condition(CScriptIniFile* ini_file, pcstr section);
	void				sell_condition(f32 friend_factor, f32 enemy_factor);
	void				buy_condition(CScriptIniFile* ini_file, pcstr section);
	void				buy_condition(f32 friend_factor, f32 enemy_factor);
	void				show_condition(CScriptIniFile* ini_file, pcstr section);
	void				buy_supplies(CScriptIniFile* ini_file, pcstr section);

	pcstr				sound_prefix( ) const;
	void				sound_prefix(pcstr sound_prefix);

	u32					location_on_path(f32 distance, fVector3* location);

	bool				wounded( ) const;
	void				wounded(bool value);

	CSightParams		sight_params( );

	void				enable_movement(bool enable);
	bool				movement_enabled( );

	bool				critically_wounded( );

	bool				invulnerable( ) const;
	void				invulnerable(bool invulnerable);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptGameObject)
#undef script_type_list
#define script_type_list save_type_list(CScriptGameObject)

extern void sell_condition(CScriptIniFile* ini_file, pcstr section);
extern void sell_condition(f32 friend_factor, f32 enemy_factor);
extern void buy_condition(CScriptIniFile* ini_file, pcstr section);
extern void buy_condition(f32 friend_factor, f32 enemy_factor);
extern void show_condition(CScriptIniFile* ini_file, pcstr section);