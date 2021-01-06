#pragma once

#include "../monster_state_manager.h"

class CFlesh;

class CStateManagerFlesh : public CMonsterStateManager<CFlesh>
{
	using inherited = CMonsterStateManager<CFlesh>;

public:
	CStateManagerFlesh(CFlesh* monster);
	virtual void	execute				();
};
