#pragma once

#include "alife_space.h"
#include "PHSkeleton.h"
#include "EntityAlive.h"
#include "PHSoundPlayer.h"
#include "Phdestroyable.h"
#include "CharacterHitAnimationController.h"

class CPhysicsShell;
class CPHMovementControl;
class CIKLimbsController;
class CInteractiveMotion;

class CCharacterPhysicsSupport : public CPHSkeleton, public CPHDestroyable
{
public:
	enum EType
	{
		etActor,
		etStalker,
		etBitting
	};

	enum EState
	{
		esDead,
		esAlive,
		esRemoved
	};

private:
	EType								m_eType;
	EState								m_eState;
	flags8								m_flags;
	enum Fags
	{
		fl_death_anim_on = 1 << 0,
		fl_skeleton_in_shell = 1 << 1,
		fl_specific_bonce_demager = 1 << 2,
		fl_block_hit = 1 << 3
	};

	struct	animation_movement_state
	{
		bool		active;
		bool		character_exist;
		void		init( )
		{
			active = false;
			character_exist = false;
		}
		animation_movement_state( )
		{
			init( );
		}
	}									anim_mov_state;

	CEntityAlive& m_EntityAlife;
	fMatrix4x4& mXFORM;
	CPhysicsShell*& m_pPhysicsShell;
	CPhysicsShell* m_physics_skeleton;
	CPHMovementControl* m_PhysicMovementControl;
	CPHSoundPlayer						m_ph_sound_player;
	CIKLimbsController* m_ik_controller;
	SCollisionHitCallback* m_collision_hit_callback;
	CCharacterHitAnimationController	m_hit_animations;

	CInteractiveMotion* m_interactive_motion;
	//skeleton modell(!share?)
	f32									skel_airr_lin_factor;
	f32									skel_airr_ang_factor;
	f32									hinge_force_factor1;
	f32									skel_fatal_impulse_factor;
	f32									skel_ddelay;
	f32									skel_remain_time;

///////////////////////////////////////////////////////////////////////////
	f32									m_shot_up_factor;
	f32									m_after_death_velocity_factor;
	f32									m_BonceDamageFactor;
	//���������� ��� ��������� ������������� ������ � ���������� �� ����� ������
	f32									skeleton_skin_ddelay;
	f32									skeleton_skin_remain_time;
	f32									skeleton_skin_friction_start;
	f32									skeleton_skin_friction_end;
	f32									skeleton_skin_ddelay_after_wound;
	f32									skeleton_skin_remain_time_after_wound;
	bool								m_was_wounded;
	f32									m_Pred_Time;//��� ���������� ������ ������� ����� ����������� ������������� � �������� � ������������ NPC
	f32									m_time_delta;
	f32									pelvis_factor_low_pose_detect;
	BOOL								character_have_wounded_state;

public:
	f32									m_curr_skin_friction_in_death;
	EType Type( )
	{
		return m_eType;
	}
	EState STate( )
	{
		return m_eState;
	}
	void	SetState(EState astate)
	{
		m_eState = astate;
	}
	IC	bool isDead( )
	{
		return m_eState == esDead;
	}
	IC	bool isAlive( )
	{
		return !m_pPhysicsShell;
	}

protected:
	virtual void							SpawnInitPhysics(CSE_Abstract* D);
	virtual CPhysicsShellHolder* PPhysicsShellHolder( )
	{
		return m_EntityAlife.PhysicsShellHolder( );
	}

	virtual bool							CanRemoveObject( );

public:
	IC		CPHMovementControl* movement( )
	{
		return m_PhysicMovementControl;
	}
	IC		CPHSoundPlayer* ph_sound_player( )
	{
		return &m_ph_sound_player;
	}
	IC		CIKLimbsController* ik_controller( )
	{
		return	m_ik_controller;
	}
	void							SetRemoved( );
	bool							IsRemoved( )
	{
		return m_eState == esRemoved;
	}
	bool							IsSpecificDamager( )
	{
		return !!m_flags.test(fl_specific_bonce_demager);
	}
	f32								BonceDamageFactor( )
	{
		return m_BonceDamageFactor;
	}
	void							set_movement_position(const fVector3& pos);
//////////////////base hierarchi methods///////////////////////////////////////////////////
	void							CreateCharacter( );
	void 							in_UpdateCL( );
	void 							in_shedule_Update(u32 DT);
	void 							in_NetSpawn(CSE_Abstract* e);
	void 							in_NetDestroy( );
	void							in_NetRelcase(CObject* O);
	void 							in_Init( );
	void 							in_Load(pcstr section);
	void 							in_Hit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type, bool is_killing = false);
	void							in_NetSave(CNetPacket& P);
	void							in_ChangeVisual( );
	void							on_create_anim_mov_ctrl( );
	void							on_destroy_anim_mov_ctrl( );
	void							PHGetLinearVell(fVector3& velocity);
	SCollisionHitCallback* get_collision_hit_callback( );
	bool							set_collision_hit_callback(SCollisionHitCallback* cc);
/////////////////////////////////////////////////////////////////
	CCharacterPhysicsSupport& operator = (CCharacterPhysicsSupport& /**asup/**/)
	{
		R_ASSERT2(false, "Can not assign it");
	}
	CCharacterPhysicsSupport(EType atype, CEntityAlive* aentity);
	virtual							~CCharacterPhysicsSupport( );

private:
	void 							CreateSkeleton(CPhysicsShell*& pShell);
	void 							CreateSkeleton( );
	void 							ActivateShell(CObject* who);
	void							KillHit(CObject* who, ALife::EHitType hit_type, f32& impulse);
	void							CreateIKController( );
	void							DestroyIKController( );
	void							CollisionCorrectObjPos(const fVector3& start_from, bool character_create = false);
	void							FlyTo(const	fVector3& disp);
	void							TestForWounded( );
	IC		void							UpdateFrictionAndJointResistanse( );
	IC		void							UpdateDeathAnims( );
	IC		void							CalculateTimeDelta( );
	IC		bool							DoCharacterShellCollide( );
};
