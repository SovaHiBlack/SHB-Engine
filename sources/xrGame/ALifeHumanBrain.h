//	Module 		: ALifeHumanBrain.h
//	Description : ALife human brain class

#pragma once

#include "ALifeMonsterBrain.h"

class CALifeHumanObjectHandler;
class CSE_ALifeHumanAbstract;

class CALifeHumanBrain : public CALifeMonsterBrain
{
private:
	using inherited = CALifeMonsterBrain;

public:
	using object_type = CSE_ALifeHumanAbstract;
	using object_handler_type = CALifeHumanObjectHandler;

private:
	object_type* m_object;
	object_handler_type* m_object_handler;

public:
	svector<char, 5>						m_cpEquipmentPreferences;
	svector<char, 4>						m_cpMainWeaponPreferences;

	CALifeHumanBrain(object_type* object);
	virtual								~CALifeHumanBrain( );

	void						on_state_write(CNetPacket& packet);
	void						on_state_read(CNetPacket& packet);

	inline		object_type& object( ) const;
	inline		object_handler_type& objects( ) const;

	static void script_register(lua_State*);
};

add_to_type_list(CALifeHumanBrain)
#undef script_type_list
#define script_type_list save_type_list(CALifeHumanBrain)

#include "ALifeHumanBrain_inline.h"
