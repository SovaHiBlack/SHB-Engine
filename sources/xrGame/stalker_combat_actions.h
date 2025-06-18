////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_action_base.h"
#include "Random32.h"

class CCoverPoint;

namespace MonsterSpace
{
	enum EBodyState;
	enum EMovementType;
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetItemToKill : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionGetItemToKill(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionMakeItemKilling
//////////////////////////////////////////////////////////////////////////

class CStalkerActionMakeItemKilling : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionMakeItemKilling(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionRetreatFromEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionRetreatFromEnemy : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionRetreatFromEnemy(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
	virtual _edge_value_type	weight(const CSConditionState& condition0, const CSConditionState& condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetReadyToKill : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

private:
	MonsterSpace::EBodyState		m_body_state;
	MonsterSpace::EMovementType		m_movement_type;
	bool							m_affect_properties;
	bool							m_enable_enemy_change;

public:
	CStalkerActionGetReadyToKill(bool affect_properties, CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemy : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionKillEnemy(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionTakeCover
//////////////////////////////////////////////////////////////////////////

class CStalkerActionTakeCover : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

private:
	MonsterSpace::EBodyState		m_body_state;
	MonsterSpace::EMovementType		m_movement_type;

public:
	CStalkerActionTakeCover(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionLookOut
//////////////////////////////////////////////////////////////////////////

class CStalkerActionLookOut : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

private:
	u32					m_last_change_time;
	CRandom32			m_crouch_look_out_random;

public:
	CStalkerActionLookOut(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionHoldPosition
//////////////////////////////////////////////////////////////////////////

class CStalkerActionHoldPosition : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionHoldPosition(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetDistance
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetDistance : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionGetDistance(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDetourEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDetourEnemy : public CStalkerActionCombatBase
{
private:
	const CCoverPoint* m_enemy_cover;

protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionDetourEnemy(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSearchEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionSearchEnemy : public CStalkerActionCombatBase
{
private:
	const CCoverPoint* m_enemy_cover;

protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionSearchEnemy(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};


//////////////////////////////////////////////////////////////////////////
// CStalkerActionPostCombatWait
//////////////////////////////////////////////////////////////////////////

class CStalkerActionPostCombatWait : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionPostCombatWait(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionHideFromGrenade
//////////////////////////////////////////////////////////////////////////

class CStalkerActionHideFromGrenade : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionHideFromGrenade(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSuddenAttack
//////////////////////////////////////////////////////////////////////////

class CStalkerActionSuddenAttack : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionSuddenAttack(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyIfPlayerOnThePath
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemyIfPlayerOnThePath : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionKillEnemyIfPlayerOnThePath(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionCriticalHit
//////////////////////////////////////////////////////////////////////////

class CStalkerActionCriticalHit : public CStalkerActionCombatBase
{
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionCriticalHit(CStalker* object, pcstr action_name = "");
	virtual void		initialize( );
	virtual void		execute( );
	virtual void		finalize( );
};
