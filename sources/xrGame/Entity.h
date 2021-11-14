#pragma once

#include "PHShellHolder.h"
#include "DamageManager.h"
#include "EntityCondition.h"

class ENGINE_API CCameraBase;
class ENGINE_API C3DSound;
class ENGINE_API CMotionDef;
class ENGINE_API CKinematics;
class ENGINE_API CBoneInstance;
class CPHMovementControl;
class CHudItem;

class CEntity : public CPHShellHolder, public C_DamageManager
{
	friend class CEntityCondition;

private:
	using inherited = CPHShellHolder;
	CEntityConditionSimple* m_entity_condition;

protected:
	//время через которое мертвое тело убирется с уровня
	ALife::_TIME_ID					m_dwBodyRemoveTime;
	virtual CEntityConditionSimple* create_entity_condition(CEntityConditionSimple* ec);

public:
	/*virtual*/inline float			GetfHealth( ) const
	{
		return m_entity_condition->GetHealth( );
	}
	/*virtual*/inline float			SetfHealth(float value)
	{
		m_entity_condition->health( ) = value; return value;
	}
	float						m_fMorale;
	// Team params
	int							id_Team;
	int							id_Squad;
	int							id_Group;

	virtual void		ChangeTeam(int team, int squad, int group);

	struct SEntityState
	{
		u32		bJump : 1;
		u32		bCrouch : 1;
		u32		bFall : 1;
		u32		bSprint : 1;
		float	fVelocity;
		float	fAVelocity;
	};

	float					m_fFood;

	// General
	CEntity( );
	virtual ~CEntity( );
	virtual DLL_Pure* _construct( );
	virtual CEntity* cast_entity( )
	{
		return this;
	}

	// Core events
	virtual void			Load(const char* section);
	virtual void			reinit( );
	virtual void			reload(const char* section);
	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Destroy( );

	virtual void			shedule_Update(u32 dt);

	bool					IsFocused( )const;
	bool					IsMyCamera( )const;

//	virtual float			g_Health			()const	{ return GetfHealth();}
	/*	virtual*/ inline float			GetMaxHealth( )const
	{
		return m_entity_condition->max_health( );
	}
	/*	virtual*/ inline void			SetMaxHealth(float v)
	{
		m_entity_condition->max_health( ) = v;
	}

	/*virtual*/ inline BOOL		g_Alive( )const
	{
		return GetfHealth( ) > 0;
	}
	virtual BOOL			g_State(SEntityState&) const
	{
		return FALSE;
	}

	bool			AlreadyDie( )
	{
		return  0 != GetLevelDeathTime( ) ? true : false;
	}
	ALife::_TIME_ID	GetGameDeathTime( )const
	{
		return m_game_death_time;
	}
	u32				GetLevelDeathTime( )const
	{
		return m_level_death_time;
	}

	virtual float			CalcCondition(float hit);

	int						g_Team( )const
	{
		return id_Team;
	}
	int						g_Squad( )const
	{
		return id_Squad;
	}
	int						g_Group( )const
	{
		return id_Group;
	}

	// Health calculations
	virtual	void			Hit(SHit* pHDS);
	virtual void			HitSignal(float P, Fvector3& local_dir, CObject* who, signed short element) = 0;
	virtual void			HitImpulse(float P, Fvector3& vWorldDir, Fvector3& vLocalDir) = 0;

	virtual void			Die(CObject* who);
//			void			KillEntity			(CObject* who);
	void			KillEntity(U16 whoID);

	// Events
	virtual void			OnEvent(CNetPacket& P, U16 type);

	virtual BOOL			IsVisibleForHUD( )
	{
		return g_Alive( );
	}
	virtual void			g_fireParams(const CHudItem*, Fvector3&, Fvector3&)
	{ }

	// time of entity death
	u32						m_level_death_time;
	ALife::_TIME_ID			m_game_death_time;

	void			set_death_time( );
	virtual	void			set_ready_to_save( );

private:
	ALife::_OBJECT_ID		m_killer_id;

public:
	inline		U16				killer_id( ) const
	{
		return m_killer_id;
	}
	virtual bool			use_simplified_visual( ) const
	{
		return false;
	}

	virtual void			on_before_change_team( );
	virtual void			on_after_change_team( );

private:
	bool					m_registered_member;
};
