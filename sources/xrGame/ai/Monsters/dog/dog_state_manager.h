#pragma once
#include "../monster_state_manager.h"

class CDog;

class CStateManagerDog : public CMonsterStateManager<CDog> {
	typedef CMonsterStateManager<CDog> inherited;

public:
					CStateManagerDog	(CDog* monster);
	virtual void	execute				();
};
