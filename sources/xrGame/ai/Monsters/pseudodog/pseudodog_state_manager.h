#pragma once
#include "../monster_state_manager.h"

class CPseudoDog;

class CStateManagerPseudodog : public CMonsterStateManager<CPseudoDog> {
	typedef CMonsterStateManager<CPseudoDog> inherited;
	
public:

					CStateManagerPseudodog	(CPseudoDog* monster);
	virtual void	execute					();
};
