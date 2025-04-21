#pragma once

#include "GameObject.h"
#include "ParticlesPlayer.h"

class CPHDestroyable;
class CPHCollisionDamageReceiver;
class CPHSoundPlayer;
class IDamageSource;
class CPHSkeleton;
class CCharacterPhysicsSupport;
class ICollisionDamageInfo;
class CIKLimbsController;

struct SCollisionHitCallback
{
	typedef								void					CollisionHitCallbackFun(CGameObject* obj, f32 min_cs, f32 max_cs, f32& cs, f32& hl, const ICollisionDamageInfo* di, SCollisionHitCallback* slf);
	CollisionHitCallbackFun* m_collision_hit_callback;
	void* m_data;
	SCollisionHitCallback( )
	{
		m_collision_hit_callback = NULL;
		m_data = NULL;
	}
	SCollisionHitCallback(CollisionHitCallbackFun* cc, void* data)
	{
		VERIFY(cc);
		m_collision_hit_callback = cc;
		m_data = data;
	}
	void call(CGameObject* obj, f32 min_cs, f32 max_cs, f32& cs, f32& hl, const ICollisionDamageInfo* di)
	{
		VERIFY(m_collision_hit_callback);
		m_collision_hit_callback(obj, min_cs, max_cs, cs, hl, di, this);
	}
};
class CPhysicsShellHolder : public CGameObject,
	public CParticlesPlayer

{
	bool				b_sheduled;

public:
	void	SheduleRegister( )
	{
		if (!IsSheduled( ))shedule_register( ); b_sheduled = true;
	}
	void	SheduleUnregister( )
	{
		if (IsSheduled( ))shedule_unregister( ); b_sheduled = false;
	}
	IC	bool	IsSheduled( )
	{
		return b_sheduled;
	}

public:
	typedef CGameObject inherited;

	CPhysicsShell* m_pPhysicsShell;

	CPhysicsShellHolder( );

	IC CPhysicsShell*& PPhysicsShell( )
	{
		return m_pPhysicsShell;
	}

	IC CPhysicsShellHolder* PhysicsShellHolder( )
	{
		return this;
	}
	virtual CPHDestroyable* ph_destroyable( )
	{
		return NULL;
	}
	virtual CPHCollisionDamageReceiver* PHCollisionDamageReceiver( )
	{
		return NULL;
	}
	virtual CPHSkeleton* PHSkeleton( )
	{
		return NULL;
	}
	virtual CPhysicsShellHolder* cast_physics_shell_holder( )
	{
		return this;
	}
	virtual CParticlesPlayer* cast_particles_player( )
	{
		return this;
	}
	virtual IDamageSource* cast_IDamageSource( )
	{
		return NULL;
	}
	virtual CPHSoundPlayer* ph_sound_player( )
	{
		return NULL;
	}
	virtual	CCharacterPhysicsSupport* character_physics_support( )
	{
		return NULL;
	}
	virtual	CCharacterPhysicsSupport* character_physics_support( ) const
	{
		return NULL;
	}
	virtual	CIKLimbsController* character_ik_controller( )
	{
		return NULL;
	}
	virtual SCollisionHitCallback* get_collision_hit_callback( )
	{
		return NULL;
	}
	virtual bool						set_collision_hit_callback(SCollisionHitCallback* cc)
	{
		return false;
	}
	virtual void						enable_notificate( )
	{
		;
	}

public:
	virtual void			PHGetLinearVell(fVector3& velocity);
	virtual void			PHSetLinearVell(fVector3& velocity);
	virtual void			PHSetMaterial(pcstr m);
	virtual void			PHSetMaterial(u16 m);
	void			PHSaveState(CNetPacket& P);
	void			PHLoadState(IReader& P);
	virtual f32				GetMass( );
	virtual	void			PHHit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual	void			Hit(SHit* pHDS);
///////////////////////////////////////////////////////////////////////
	virtual u16				PHGetSyncItemsNumber( );
	virtual CPHSynchronize* PHGetSyncItem(u16 item);
	virtual void			PHUnFreeze( );
	virtual void			PHFreeze( );
	virtual f32			EffectiveGravity( );
///////////////////////////////////////////////////////////////
	virtual void			create_physic_shell( );
	virtual void			activate_physic_shell( );
	virtual void			setup_physic_shell( );
	virtual void			deactivate_physics_shell( );

	virtual void			net_Destroy( );
	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			save(CNetPacket& output_packet);
	virtual void			load(IReader& input_packet);
	void			init( );

	virtual void			OnChangeVisual( );
	//��� ������������ CParticlesPlayer
	virtual void			UpdateCL( );
	void			correct_spawn_pos( );

public:
	virtual bool			register_schedule( ) const;

public:
	virtual	void			on_physics_disable( );
};
