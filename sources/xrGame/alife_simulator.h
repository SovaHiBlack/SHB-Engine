//	Module 		: alife_simulator.h
//	Description : ALife Simulator

#pragma once

#include "alife_interaction_manager.h"
#include "AlifeUpdateManager.h"
#include "script_export_space.h"

#pragma warning(push)
#pragma warning(disable:4005)

class CALifeSimulator : 
	public CAlifeUpdateManager,
	public CALifeInteractionManager
{
protected:
	virtual void	setup_simulator		(CSE_ALifeObject *object);
	virtual void	reload				(const char* section);

public:
					CALifeSimulator		(CServer*server, CSharedString* command_line);
	virtual			~CALifeSimulator	();
	virtual void	destroy				();

#if 0//def DEBUG
			void	validate			();
#endif //DEBUG

public:
	static void script_register(lua_State*);
};

add_to_type_list(CALifeSimulator)
#undef script_type_list
#define script_type_list save_type_list(CALifeSimulator)

#pragma warning(pop)
