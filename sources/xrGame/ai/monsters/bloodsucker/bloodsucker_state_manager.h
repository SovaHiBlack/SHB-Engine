#pragma once

#include "../monster_state_manager.h"

class CBloodsucker;

class CStateManagerBloodsucker : public CMonsterStateManager<CBloodsucker>
{
	using inherited = CMonsterStateManager<CBloodsucker>;

public:
	CStateManagerBloodsucker(CBloodsucker* monster);
	virtual void	execute						();
};
