#pragma once

#include "../monster_state_manager.h"

class CBoar;

class CBoarStateManager : public CMonsterStateManager<CBoar>
{
	using inherited = CMonsterStateManager<CBoar>;

public:
	CBoarStateManager(CBoar* monster);
	virtual void	execute( );
};
