//	Module 		: ALifeHumanObjectHandler.h
//	Description : ALife human object handler class

#pragma once

#include "ALife_space.h"

class CSE_ALifeItemWeapon;
class CSE_ALifeInventoryItem;
class CSE_ALifeGroupAbstract;
class CSE_ALifeHumanAbstract;

class CALifeHumanObjectHandler
{
public:
	using object_type = CSE_ALifeHumanAbstract;

private:
	object_type* m_object;

public:
	inline								CALifeHumanObjectHandler(object_type* object);
	inline object_type& object( ) const;

	U16						get_available_ammo_count(const CSE_ALifeItemWeapon* weapon, ALife::OBJECT_VECTOR& objects);
	U16						get_available_ammo_count(const CSE_ALifeItemWeapon* weapon, ALife::ITEM_P_VECTOR& items, ALife::OBJECT_VECTOR* objects = 0);
	void					attach_available_ammo(CSE_ALifeItemWeapon* weapon, ALife::ITEM_P_VECTOR& items, ALife::OBJECT_VECTOR* objects = 0);
	bool					can_take_item(CSE_ALifeInventoryItem* inventory_item);
	void					collect_ammo_boxes( );

	void					detach_all(bool fictitious);
	void					update_weapon_ammo( );
	void					process_items( );
	CSE_ALifeDynamicObject* best_detector( );
	CSE_ALifeItemWeapon* best_weapon( );

	int						choose_equipment(ALife::OBJECT_VECTOR* objects = 0);
	int						choose_weapon(const ALife::EWeaponPriorityType& weapon_priority_type, ALife::OBJECT_VECTOR* objects = 0);
	int						choose_food(ALife::OBJECT_VECTOR* objects = 0);
	int						choose_medikit(ALife::OBJECT_VECTOR* objects = 0);
	int						choose_detector(ALife::OBJECT_VECTOR* objects = 0);
	int						choose_valuables( );
	bool					choose_fast( );
	void					choose_group(CSE_ALifeGroupAbstract* group_abstract);
	void					attach_items( );
};

#include "ALifeHumanObjectHandler_inline.h"
