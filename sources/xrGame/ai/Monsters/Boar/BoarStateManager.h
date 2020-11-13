#pragma once

#include "../monster_state_manager.h"

class CBoar;

class CBoarStateManager : public CMonsterStateManager<CBoar>
{
	typedef CMonsterStateManager<CBoar> inherited;

public:

	CBoarStateManager(CBoar* monster);

	virtual void	execute( );
};
