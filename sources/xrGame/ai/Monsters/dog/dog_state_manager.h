#pragma once
#include "../monster_state_manager.h"

class CDog;

class CStateManagerDog : public CMonsterStateManager<CDog> {
	typedef CMonsterStateManager<CDog> inherited;

public:
					CStateManagerDog	(CDog*b monster);
	virtual void	execute				();
};
