#pragma once

#include "GameObject.h"
#include "ParticlesPlayer.h"

class CPHDestroyable;
class CPHCollisionDamageReceiver;
class CPHSoundPlayer;
class I_DamageSource;
class CPHSkeleton;
class CCharacterPhysicsSupport;
class ICollisionDamageInfo;
class CIKLimbsController;

struct SCollisionHitCallback
{
	typedef void CollisionHitCallbackFun(CGameObject* obj, float min_cs, float max_cs, float& cs, float& hl, const ICollisionDamageInfo* di, SCollisionHitCallback* slf);
	CollisionHitCallbackFun* m_collision_hit_callback;
	void* m_data;
	SCollisionHitCallback( )
	{
		m_collision_hit_callback = nullptr;
		m_data = nullptr;
	}
	SCollisionHitCallback(CollisionHitCallbackFun* cc, void* data)
	{
		VERIFY(cc);
		m_collision_hit_callback = cc;
		m_data = data;
	}
	void call(CGameObject* obj, float min_cs, float max_cs, float& cs, float& hl, const ICollisionDamageInfo* di)
	{
		VERIFY(m_collision_hit_callback);
		m_collision_hit_callback(obj, min_cs, max_cs, cs, hl, di, this);
	}
};

class CPHShellHolder : public CGameObject, public CParticlesPlayer
{
	bool				b_sheduled;

public:
	void	SheduleRegister( )
	{
		if (!IsSheduled( )) shedule_register( ); b_sheduled = true;
	}
	void	SheduleUnregister( )
	{
		if (IsSheduled( )) shedule_unregister( ); b_sheduled = false;
	}
	inline bool	IsSheduled( )
	{
		return b_sheduled;
	}

public:
	typedef CGameObject inherited;

	CPhysicsShell* m_pPhysicsShell;

	CPHShellHolder( );

	inline CPhysicsShell*& PPhysicsShell( )
	{
		return m_pPhysicsShell;
	}

	inline CPHShellHolder* PhysicsShellHolder( )
	{
		return this;
	}
	virtual CPHDestroyable* ph_destroyable( )
	{
		return nullptr;
	}
	virtual CPHCollisionDamageReceiver* PHCollisionDamageReceiver( )
	{
		return nullptr;
	}
	virtual CPHSkeleton* PHSkeleton( )
	{
		return nullptr;
	}
	virtual CPHShellHolder* cast_physics_shell_holder( )
	{
		return this;
	}
	virtual CParticlesPlayer* cast_particles_player( )
	{
		return this;
	}
	virtual I_DamageSource* cast_IDamageSource( )
	{
		return nullptr;
	}
	virtual CPHSoundPlayer* ph_sound_player( )
	{
		return nullptr;
	}
	virtual	CCharacterPhysicsSupport* character_physics_support( )
	{
		return nullptr;
	}
	virtual	CCharacterPhysicsSupport* character_physics_support( ) const
	{
		return nullptr;
	}
	virtual	CIKLimbsController* character_ik_controller( )
	{
		return nullptr;
	}
	virtual SCollisionHitCallback* get_collision_hit_callback( )
	{
		return nullptr;
	}
	virtual bool						set_collision_hit_callback(SCollisionHitCallback* cc)
	{
		return false;
	}
	virtual void						enable_notificate( )
	{ }

public:
	virtual void			PHGetLinearVell(Fvector3& velocity);
	virtual void			PHSetLinearVell(Fvector3& velocity);
	virtual void			PHSetMaterial(const char* m);
	virtual void			PHSetMaterial(u16 m);
	void					PHSaveState(CNetPacket& P);
	void					PHLoadState(IReader& P);
	virtual float			GetMass( );
	virtual	void			PHHit(float P, Fvector3& dir, CObject* who, s16 element, Fvector3 p_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual	void			Hit(SHit* pHDS);
///////////////////////////////////////////////////////////////////////
	virtual u16				PHGetSyncItemsNumber( );
	virtual CPHSynchronize* PHGetSyncItem(u16 item);
	virtual void			PHUnFreeze( );
	virtual void			PHFreeze( );
	virtual float			EffectiveGravity( );
///////////////////////////////////////////////////////////////
	virtual void			create_physic_shell( );
	virtual void			activate_physic_shell( );
	virtual void			setup_physic_shell( );
	virtual void			deactivate_physics_shell( );

	virtual void			net_Destroy( );
	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			save(CNetPacket& output_packet);
	virtual void			load(IReader& input_packet);
	void					init( );

	virtual void			OnChangeVisual( );
	//для наследования CParticlesPlayer
	virtual void			UpdateCL( );
	void					correct_spawn_pos( );

	virtual bool			register_schedule( ) const;

	virtual	void			on_physics_disable( );
};
