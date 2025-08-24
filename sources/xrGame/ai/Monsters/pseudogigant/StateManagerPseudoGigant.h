#pragma once
#include "../monster_state_manager.h"

class CPseudoGigant;

class CStateManagerPseudoGigant : public CMonsterStateManager<CPseudoGigant>
{
	typedef CMonsterStateManager<CPseudoGigant> inherited;
public:

	CStateManagerPseudoGigant(CPseudoGigant* monster);
	virtual void	execute( );
};
