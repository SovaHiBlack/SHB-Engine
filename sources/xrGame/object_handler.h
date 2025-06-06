////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler.h
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InventoryOwner.h"
#include "graph_engine_space.h"

namespace MonsterSpace
{
	enum EObjectAction;
}

class CStalker;
class CWeapon;
class CMissile;
class CFoodItem;
class CObjectHandlerPlanner;

class CObjectHandler : public CInventoryOwner
{
protected:
	typedef CInventoryOwner								inherited;
	typedef GraphEngineSpace::_solver_value_type		_value_type;
	typedef GraphEngineSpace::_solver_condition_type	_condition_type;

private:
	s32								m_r_hand;
	s32								m_l_finger1;
	s32								m_r_finger2;

private:
	mutable s32						m_strap_bone0;
	mutable s32						m_strap_bone1;
	mutable ALife::_OBJECT_ID		m_strap_object_id;

protected:
	bool							m_hammer_is_clutched;
	bool							m_infinite_ammo;
	CObjectHandlerPlanner* m_planner;
	mutable bool					m_inventory_actual;

public:
	bool							m_clutched_hammer_enabled;

private:
	void					actualize_strap_mode(CWeapon* weapon) const;

protected:
	IC		void					switch_torch(CInventoryItem* inventory_item, bool value);

public:
	CObjectHandler( );
	virtual							~CObjectHandler( );
	virtual	void					Load(pcstr section);
	virtual	void					reinit(CStalker* object);
	virtual	void					reload(pcstr section);
	virtual BOOL					net_Spawn(CSE_Abstract* DC);
	virtual	void					update( );
	virtual void					OnItemTake(CInventoryItem* inventory_item);
	virtual void					OnItemDrop(CInventoryItem* inventory_item);
	virtual	void					attach(CInventoryItem* inventory_item);
	virtual	void					detach(CInventoryItem* inventory_item);
	CInventoryItem* best_weapon( ) const;
	void					set_goal(MonsterSpace::EObjectAction object_action, CGameObject* game_object = 0, u32 min_queue_size = -1, u32 max_queue_size = -1, u32 min_queue_interval = 300, u32 max_queue_interval = 300);
	void					set_goal(MonsterSpace::EObjectAction object_action, CInventoryItem* inventory_item, u32 min_queue_size = -1, u32 max_queue_size = -1, u32 min_queue_interval = 300, u32 max_queue_interval = 300);
	bool					goal_reached( );
	IC		bool					hammer_is_clutched( ) const;
	IC		CObjectHandlerPlanner& planner( ) const;
	void					weapon_bones(s32& b0, s32& b1, s32& b2) const;
	bool					weapon_strapped( ) const;
	bool					weapon_strapped(CWeapon* weapon) const;
	bool					weapon_unstrapped( ) const;
	bool					weapon_unstrapped(CWeapon* weapon) const;

public:
	virtual bool					can_use_dynamic_lights( );
};

#include "object_handler_inline.h"
