#pragma once
#include "../monster_state_manager.h"

class CPseudoDog;

class CStateManagerPseudoDog : public CMonsterStateManager<CPseudoDog> {
	typedef CMonsterStateManager<CPseudoDog> inherited;
	
public:

	CStateManagerPseudoDog(CPseudoDog* monster);
	virtual void	execute					();
};
