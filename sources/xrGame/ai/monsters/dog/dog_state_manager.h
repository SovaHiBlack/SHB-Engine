#pragma once

#include "../monster_state_manager.h"

class CDog;

class CStateManagerDog : public CMonsterStateManager<CDog>
{
	using inherited = CMonsterStateManager<CDog>;

public:
					CStateManagerDog	(CDog*monster);
	virtual void	execute				();
};
