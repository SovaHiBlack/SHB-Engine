#pragma once

#include "physicsshellholder.h"
#include "damage_manager.h"
#include "EntityCondition.h"

// refs
class ENGINE_API CCameraBase;
class ENGINE_API C3DSound;
class ENGINE_API CMotionDef;
class ENGINE_API CKinematics;
class ENGINE_API CBoneInstance;
class CPHMovementControl;
class CHudItem;

class CEntity : public CPhysicsShellHolder, public CDamageManager
{
	friend class CEntityCondition;

private:
	typedef CPhysicsShellHolder		inherited;
	CEntityConditionSimple*							m_entity_condition;

protected:
	//время через которое мертвое тело убирется с уровня
	ALife::_TIME_ID									m_dwBodyRemoveTime;

protected:
	virtual CEntityConditionSimple* create_entity_condition(CEntityConditionSimple* ec);

public:
	IC f32					GetfHealth				( ) const
	{
		return m_entity_condition->GetHealth( );
	}
	IC f32					SetfHealth				(f32 value)
	{
		m_entity_condition->health( ) = value;
		return value;
	}
	f32												m_fMorale;
	// Team params
	s32												id_Team;
	s32												id_Squad;
	s32												id_Group;

	virtual void			ChangeTeam				(s32 team, s32 squad, s32 group);

	struct SEntityState
	{
		u32		bJump : 1;
		u32		bCrouch : 1;
		u32		bFall : 1;
		u32		bSprint : 1;
		f32		fVelocity;
		f32		fAVelocity;
	};

	f32												m_fFood;

	// General
							CEntity					( );
	virtual					~CEntity				( );
	virtual DLL_Pure*		_construct				( );
	virtual CEntity*		cast_entity				( )
	{
		return this;
	}

public:
	// Core events
	virtual void			Load					(pcstr section);
	virtual void			reinit					( );
	virtual void			reload					(pcstr section);
	virtual BOOL			net_Spawn				(CSE_Abstract* DC);
	virtual void			net_Destroy				( );

	virtual void			shedule_Update			(u32 dt);

	bool					IsFocused				( ) const;
	bool					IsMyCamera				( ) const;

	IC f32					GetMaxHealth			( ) const
	{
		return m_entity_condition->max_health( );
	}
	IC void					SetMaxHealth			(f32 v)
	{
		m_entity_condition->max_health( ) = v;
	}

	IC BOOL					g_Alive					( ) const
	{
		return (GetfHealth( ) > 0);
	}
	virtual BOOL			g_State					(SEntityState&) const
	{
		return FALSE;
	}

	bool					AlreadyDie				( )
	{
		return (0 != GetLevelDeathTime( ) ? true : false);
	}
	ALife::_TIME_ID			GetGameDeathTime		( ) const
	{
		return m_game_death_time;
	}
	u32						GetLevelDeathTime		( ) const
	{
		return m_level_death_time;
	}

	virtual f32				CalcCondition			(f32 hit);

	s32						g_Team					( ) const
	{
		return id_Team;
	}
	s32						g_Squad					( ) const
	{
		return id_Squad;
	}
	s32						g_Group					( ) const
	{
		return id_Group;
	}

	// Health calculations
	virtual void			Hit						(SHit* pHDS);
	virtual void			HitSignal				(f32 P, fVector3& local_dir, CObject* who, s16 element) = 0;
	virtual void			HitImpulse				(f32 P, fVector3& vWorldDir, fVector3& vLocalDir) = 0;

	virtual void			Die						(CObject* who);
	void					KillEntity				(u16 whoID);

	// Events
	virtual void			OnEvent					(CNetPacket& P, u16 type);

	virtual BOOL			IsVisibleForHUD			( )
	{
		return g_Alive( );
	}
	virtual void			g_fireParams			(const CHudItem*, fVector3&, fVector3&)
	{ }

	//time of entity death
	u32												m_level_death_time;
	ALife::_TIME_ID									m_game_death_time;

	void					set_death_time			( );
	virtual void			set_ready_to_save		( );

private:
	ALife::_OBJECT_ID								m_killer_id;

public:
	IC u16					killer_id				( ) const
	{
		return m_killer_id;
	}
	virtual bool			use_simplified_visual	( ) const
	{
		return false;
	}

public:
	virtual void			on_before_change_team	( );
	virtual void			on_after_change_team	( );

private:
	bool											m_registered_member;
};
