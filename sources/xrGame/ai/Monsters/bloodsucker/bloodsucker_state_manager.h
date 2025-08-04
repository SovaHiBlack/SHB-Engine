#pragma once
#include "../monster_state_manager.h"

class CBloodsucker;

class CStateManagerBloodsucker : public CMonsterStateManager<CBloodsucker> {
	typedef CMonsterStateManager<CBloodsucker> inherited;

public:
					CStateManagerBloodsucker	(CBloodsucker* monster);
	virtual void	execute						();
};
