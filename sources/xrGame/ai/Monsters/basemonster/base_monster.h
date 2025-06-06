#pragma once

#include "../../../CustomMonster.h"

#include "../monster_enemy_memory.h"
#include "../monster_corpse_memory.h"
#include "../monster_sound_memory.h"
#include "../monster_hit_memory.h"

#include "../monster_enemy_manager.h"
#include "../monster_corpse_manager.h"

#include "../../../StepManager.h"
#include "../monster_event_manager.h"
#include "../melee_checker.h"
#include "../monster_morale.h"

#include "../control_manager.h"
#include "../control_sequencer.h"

#include "../ai_monster_utils.h"

#include "../control_manager_custom.h"
#include "../ai_monster_shared_data.h"
#include "../monster_sound_defs.h"
#include "../../../InventoryOwner.h"

class CCharacterPhysicsSupport;
class CMonsterCorpseCoverEvaluator;
class CCoverEvaluatorFarFromEnemy;
class CCoverEvaluatorCloseToEnemy;
class CMonsterEventManager;
class CJumping;
class CControlledEntityBase;
class CMovementManager;
class IStateManagerBase;
class CAnomalyDetector;

class CControlAnimationBase;
class CControlMovementBase;
class CControlPathBuilderBase;
class CControlDirectionBase;
class CMonsterCoverManager;

class CMonsterHome;

class CBaseMonster : public CCustomMonster, public CStepManager, public CInventoryOwner
{
	typedef	CCustomMonster								inherited;

public:
	CBaseMonster( );
	virtual					~CBaseMonster( );

public:
	virtual	Feel::Sound* dcast_FeelSound( )
	{
		return this;
	}
	virtual	CCharacterPhysicsSupport* character_physics_support( )
	{
		return m_pPhysics_support;
	}
	virtual CPHDestroyable* ph_destroyable( );
	virtual CEntityAlive* cast_entity_alive( )
	{
		return this;
	}
	virtual CEntity* cast_entity( )
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
	virtual CCustomMonster* cast_custom_monster( )
	{
		return this;
	}
	virtual CScriptEntity* cast_script_entity( )
	{
		return this;
	}
	virtual CBaseMonster* cast_base_monster( )
	{
		return this;
	}

	virtual CInventoryOwner* cast_inventory_owner( )
	{
		return this;
	}
	virtual CGameObject* cast_game_object( )
	{
		return this;
	}

public:

	virtual	BOOL			renderable_ShadowReceive( )
	{
		return TRUE;
	}
	virtual void			Die(CObject* who);
	virtual void			HitSignal(f32 amount, fVector3& vLocalDir, CObject* who, s16 element);
	virtual	void			Hit(SHit* pHDS);
	virtual	void			PHHit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void			SelectAnimation(const fVector3& _view, const fVector3& _move, f32 speed);

	virtual void			Load(pcstr section);
	virtual DLL_Pure* _construct( );

	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Destroy( );
	virtual void			net_Save(CNetPacket& P);
	virtual	BOOL			net_SaveRelevant( );
	virtual void			net_Export(CNetPacket& P);
	virtual void			net_Import(CNetPacket& P);
	virtual void			net_Relcase(CObject* O);

	//save/load server serialization
	virtual void			save(CNetPacket& output_packet)
	{
		inherited::save(output_packet);
	}
	virtual void			load(IReader& input_packet)
	{
		inherited::load(input_packet);
	}

	virtual void			UpdateCL( );
	virtual void			shedule_Update(u32 dt);

	virtual void			InitThink( )
	{ }
	virtual void			Think( );
	virtual	void			reinit( );
	virtual void			reload(pcstr section);

	virtual void			init( )
	{ }

	virtual void			feel_sound_new(CObject* who, s32 eType, CSound_UserDataPtr user_data, const fVector3& Position, f32 power);
	virtual BOOL			feel_vision_isRelevant(CObject* O);
	virtual BOOL			feel_touch_on_contact(CObject* O);
	virtual BOOL			feel_touch_contact(CObject*);

	virtual bool			useful(const CItemManager* manager, const CGameObject* object) const;
	virtual f32			evaluate(const CItemManager* manager, const CGameObject* object) const;

	virtual void			OnEvent(CNetPacket& P, u16 type);
	virtual void			OnHUDDraw(CCustomHUD* hud)
	{
		return inherited::OnHUDDraw(hud);
	}
	virtual u16				PHGetSyncItemsNumber( )
	{
		return inherited::PHGetSyncItemsNumber( );
	}
	virtual CPHSynchronize* PHGetSyncItem(u16 item)
	{
		return inherited::PHGetSyncItem(item);
	}
	virtual void			PHUnFreeze( )
	{
		return inherited::PHUnFreeze( );
	}
	virtual void			PHFreeze( )
	{
		return inherited::PHFreeze( );
	}
	virtual BOOL			UsedAI_Locations( )
	{
		return inherited::UsedAI_Locations( );
	}

	virtual const SRotation	Orientation( ) const
	{
		return inherited::Orientation( );
	}
	virtual void			renderable_Render( )
	{
		return inherited::renderable_Render( );
	}

	virtual	void			on_restrictions_change( );

	virtual	void			SetAttackEffector( );

	virtual void			update_fsm( );

	virtual	void			post_fsm_update( );
	void			squad_notify( );

	virtual bool			IsTalkEnabled( )
	{
		return false;
	}

	virtual void			HitEntity(const CEntity* pEntity, f32 fDamage, f32 impulse, fVector3& dir);
	virtual	void			HitEntityInJump(const CEntity* pEntity)
	{ }

	virtual	void			on_before_sell(CInventoryItem* item);
	f32			GetSatiety( )
	{
		return 0.5f;
	}
	void			ChangeSatiety(f32 v)
	{ }
	// ---------------------------------------------------------------------------------
	// Process scripts
	// ---------------------------------------------------------------------------------
	virtual	bool			bfAssignMovement(CScriptEntityAction* tpEntityAction);
	virtual	bool			bfAssignObject(CScriptEntityAction* tpEntityAction);
	virtual	bool			bfAssignWatch(CScriptEntityAction* tpEntityAction);
	virtual bool			bfAssignAnimation(CScriptEntityAction* tpEntityAction);
	virtual	bool			bfAssignMonsterAction(CScriptEntityAction* tpEntityAction);
	virtual bool			bfAssignSound(CScriptEntityAction* tpEntityAction);

	virtual	void			vfFinishAction(CScriptEntityAction* tpEntityAction);

	virtual void			ProcessScripts( );

	virtual	CEntity* GetCurrentEnemy( );
	virtual	CEntity* GetCurrentCorpse( );
	virtual s32				get_enemy_strength( );

	virtual void			SetScriptControl(const bool bScriptControl, shared_str caSciptName);

	bool					m_force_real_speed;
	bool					m_script_processing_active;
	bool					m_script_state_must_execute;

	virtual void			jump(const fVector3& position, f32 factor)
	{ }

	bool					m_skip_transfer_enemy;
	IC		void			skip_transfer_enemy(bool value)
	{
		m_skip_transfer_enemy = value;
	}

	IC		s32				Rank( )
	{
		return m_rank;
	}

	//----------------------------------------------------------------------------------

	virtual void			SetTurnAnimation(bool turn_left);

	// ��������� ������������� �������� 
	virtual	void			CheckSpecParams(u32 /**spec_params/**/)
	{ }
	virtual void			ForceFinalAnimation( )
	{ }
	virtual void			LookPosition(fVector3 to_point, f32 angular_speed = PI_DIV_3);		// ������ ������ ����� ��-������� ���������� ��� ���� (e.g. �������� � ��������� ������ � �.�.)

	// Team
	virtual void			ChangeTeam(s32 team, s32 squad, s32 group);

	// ---------------------------------------------------------------------------------
	// Abilities
	// ---------------------------------------------------------------------------------
	virtual bool			ability_invisibility( )
	{
		return false;
	}
	virtual bool			ability_can_drag( )
	{
		return false;
	}
	virtual bool			ability_psi_attack( )
	{
		return false;
	}
	virtual bool			ability_earthquake( )
	{
		return false;
	}
	virtual bool			ability_can_jump( )
	{
		return false;
	}
	virtual bool			ability_distant_feel( )
	{
		return false;
	}
	virtual bool			ability_run_attack( )
	{
		return false;
	}
	virtual bool			ability_rotation_jump( )
	{
		return false;
	}
	virtual bool			ability_jump_over_physics( )
	{
		return false;
	}
	virtual bool			ability_pitch_correction( )
	{
		return true;
	}
	// ---------------------------------------------------------------------------------

	virtual void			event_on_step( )
	{ }
	virtual void			on_threaten_execute( )
	{ }
	// ---------------------------------------------------------------------------------
	// Memory
	void			UpdateMemory( );

	// Cover
	bool			GetCorpseCover(fVector3& position, u32& vertex_id);
	bool			GetCoverFromEnemy(const fVector3& enemy_pos, fVector3& position, u32& vertex_id);
	bool			GetCoverFromPoint(const fVector3& pos, fVector3& position, u32& vertex_id, f32 min_dist, f32 max_dist, f32 radius);
	bool			GetCoverCloseToPoint(const fVector3& dest_pos, f32 min_dist, f32 max_dist, f32 deviation, f32 radius, fVector3& position, u32& vertex_id);

	// Movement Manager
protected:
	CControlPathBuilder* m_movement_manager;
protected:
	virtual CMovementManager* create_movement_manager( );

	// members
public:
	// --------------------------------------------------------------------------------------
	// Monster Settings 
	ref_smem<SMonsterSettings>	m_base_settings;
	ref_smem<SMonsterSettings>	m_current_settings;

	void						settings_read(CIniFile* ini, pcstr section, SMonsterSettings& data);
	void						settings_load(pcstr section);
	void						settings_overrides( );

	SMonsterSettings& db( )
	{
		return *(*m_current_settings);
	}
	// --------------------------------------------------------------------------------------

	CCharacterPhysicsSupport* m_pPhysics_support;

	CMonsterCorpseCoverEvaluator* m_corpse_cover_evaluator;
	CCoverEvaluatorFarFromEnemy* m_enemy_cover_evaluator;
	CCoverEvaluatorCloseToEnemy* m_cover_evaluator_close_point;

	// ---------------------------------------------------------------------------------
	IStateManagerBase* StateMan;
	// ---------------------------------------------------------------------------------

	CMonsterEnemyMemory		EnemyMemory;
	CMonsterSoundMemory		SoundMemory;
	CMonsterCorpseMemory	CorpseMemory;
	CMonsterHitMemory		HitMemory;

	CMonsterEnemyManager	EnemyMan;
	CMonsterCorpseManager	CorpseMan;

	bool					hear_dangerous_sound;
	bool					hear_interesting_sound;

	// -----------------------------------------------------------------------------
	CMonsterEventManager	EventMan;
	// -----------------------------------------------------------------------------

	CMeleeChecker			MeleeChecker;
	CMonsterMorale			Morale;

	// -----------------------------------------------------------------------------

	CMonsterCoverManager* CoverMan;

	// -----------------------------------------------------------------------------

	CControlledEntityBase* m_controlled;

	// -----------------------------------------------------------------------------
	enum EMonsterType
	{
		eMonsterTypeUniversal = u32(0),
		eMonsterTypeIndoor,
		eMonsterTypeOutdoor
	} m_monster_type;

	// -----------------------------------------------------------------------------
	// Home
	CMonsterHome* Home;

	// -----------------------------------------------------------------------------
	// Anomaly Detector
private:
	CAnomalyDetector* m_anomaly_detector;

public:
	CAnomalyDetector& anomaly_detector( )
	{
		return (*m_anomaly_detector);
	}
	// -----------------------------------------------------------------------------


//	//-----------------------------------------------------------------
//	// Spawn Inventory Item
//	//-----------------------------------------------------------------
//private:
//	pcstr					m_item_section;

	//--------------------------------------------------------------------
	// Berserk
	//--------------------------------------------------------------------
public:
	u32						time_berserk_start;
	IC void					set_berserk( )
	{
		time_berserk_start = time( );
	}
	bool					berserk_always;

	//--------------------------------------------------------------------
	// Panic Threshold (extension for scripts)
	//--------------------------------------------------------------------

	f32				m_default_panic_threshold;
	IC	void				set_custom_panic_threshold(f32 value);
	IC	void				set_default_panic_threshold( );
	//--------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// -----------------------------------------------------------------------------
	// Special Services (refactoring needed)

	void				on_kill_enemy(const CEntity* obj);
	void				Hit_Psy(CObject* object, f32 value);
	void				Hit_Wound(CObject* object, f32 value, const fVector3& dir, f32 impulse);
	CParticlesObject* PlayParticles(const shared_str& name, const fVector3& position, const fVector3& dir, BOOL auto_remove = TRUE, BOOL xformed = TRUE);
	void				load_effector(pcstr section, pcstr line, SAttackEffector& effector);

	// --------------------------------------------------------------------------------------
	// Kill From Here
	// --------------------------------------------------------------------------------------
	// State flags
	bool						m_bDamaged;
	bool						m_bAngry;
	bool						m_bGrowling;
	bool						m_bAggressive;
	bool						m_bSleep;
	bool						m_bRunTurnLeft;
	bool						m_bRunTurnRight;

	void						set_aggressive(bool val = true)
	{
		m_bAggressive = val;
	}

	//---------------------------------------------------------------------------------------

	u32						m_prev_sound_type;
	u32						get_attack_rebuild_time( );

	IC	virtual	EAction		CustomVelocityIndex2Action(u32 velocity_index)
	{
		return ACT_STAND_IDLE;
	}
	virtual	void		TranslateActionToPathParams( );

	bool					state_invisible;

	void					set_action(EAction action);
	void					set_state_sound(u32 type, bool once = false);
	IC	void					fall_asleep( )
	{
		m_bSleep = true;
	}
	IC	void					wake_up( )
	{
		m_bSleep = false;
	}

	// Temp
	u32						m_time_last_attack_success;
	s32						m_rank;
	f32					m_melee_rotation_factor;

private:
	bool					ignore_collision_hit;

public:
	IC	void				set_ignore_collision_hit(bool value)
	{
		ignore_collision_hit = value;
	}
	// -----------------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////

public:
	CControl_Manager& control( )
	{
		return (*m_control_manager);
	}

	CControlAnimationBase& anim( )
	{
		return (*m_anim_base);
	}
	CControlMovementBase& move( )
	{
		return (*m_move_base);
	}
	CControlPathBuilderBase& path( )
	{
		return (*m_path_base);
	}
	CControlDirectionBase& dir( )
	{
		return (*m_dir_base);
	}

	CControlManagerCustom& com_man( )
	{
		return m_com_manager;
	}

	virtual bool			check_start_conditions(ControlCom::EControlType);
	virtual void			on_activate_control(ControlCom::EControlType)
	{ }

protected:
	CControl_Manager* m_control_manager;

	CControlAnimationBase* m_anim_base;
	CControlMovementBase* m_move_base;
	CControlPathBuilderBase* m_path_base;
	CControlDirectionBase* m_dir_base;

	CControlManagerCustom	m_com_manager;

	virtual void			create_base_controls( );

	//////////////////////////////////////////////////////////////////////////
	// Critical Wounded
	//////////////////////////////////////////////////////////////////////////
	enum
	{
		critical_wound_type_head = u32(0),
		critical_wound_type_torso,
		critical_wound_type_legs
	};

	virtual void			load_critical_wound_bones( );
	virtual bool			critical_wound_external_conditions_suitable( );
	virtual void			critical_wounded_state_start( );

	void			fill_bones_body_parts(pcstr body_part, CriticalWoundType wound_type);

	pcstr					m_critical_wound_anim_head;
	pcstr					m_critical_wound_anim_torso;
	pcstr					m_critical_wound_anim_legs;

	//////////////////////////////////////////////////////////////////////////


public:


	//////////////////////////////////////////////////////////////////////////
	// DEBUG stuff
#ifdef DEBUG
public:
	struct SDebugInfo
	{
		bool	active;
		f32	x;
		f32	y;
		f32	delta_y;
		u32		color;
		u32		delimiter_color;

		SDebugInfo( ) : active(false)
		{ }
		SDebugInfo(f32 px, f32 py, f32 dy, u32 c, u32 dc) : active(true), x(px), y(py), delta_y(dy), color(c), delimiter_color(dc)
		{ }
	};

	u8						m_show_debug_info;	// 0 - none, 1 - first column, 2 - second column
	void					set_show_debug_info(u8 show = 1)
	{
		m_show_debug_info = show;
	}
	virtual	SDebugInfo		show_debug_info( );

	void					debug_fsm( );
#endif

#ifdef _DEBUG
	virtual void			debug_on_key(s32 key)
	{ }
#endif
	//////////////////////////////////////////////////////////////////////////

};

#include "base_monster_inline.h"