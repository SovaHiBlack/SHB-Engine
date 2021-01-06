#pragma once
#include "../monster_state_manager.h"

class CBurer;

class CStateManagerBurer : public CMonsterStateManager<CBurer> 
 {
	using inherited = CMonsterStateManager<CBurer>;

public:
					CStateManagerBurer		(CBurer *monster); 
	virtual void	execute					();
	virtual void	setup_substates			();
};
