#pragma once
#include "StateManagerPseudoDog.h"

class CStateManagerPsyDog : public CStateManagerPseudoDog
{
	typedef CStateManagerPseudoDog inherited;
public:
	CStateManagerPsyDog(CPseudoDog* monster);
	virtual void	execute( );
};
