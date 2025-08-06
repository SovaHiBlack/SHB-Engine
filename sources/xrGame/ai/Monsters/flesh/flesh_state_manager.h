#pragma once
#include "../monster_state_manager.h"

class CFlesh;

class CStateManagerFlesh : public CMonsterStateManager<CFlesh> {
	typedef CMonsterStateManager<CFlesh> inherited;

public:

					CStateManagerFlesh	(CFlesh* monster);
	virtual void	execute				();
};
