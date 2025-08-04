#pragma once
#include "../monster_state_manager.h"

class CBoar;

class CStateManagerBoar : public CMonsterStateManager<CBoar> {
	typedef CMonsterStateManager<CBoar> inherited;

public:

					CStateManagerBoar	(CBoar* monster);

	virtual void	execute				();
};
