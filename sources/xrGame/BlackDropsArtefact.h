// BlackDropsArtefact.h
// BlackDropsArtefact - черные капли

#pragma once

#include "Artefact.h"

class CBlackDropsArtefact : public CArtefact
{
private:
	using inherited = CArtefact;

public:
	CBlackDropsArtefact( );
	virtual ~CBlackDropsArtefact( );

	virtual void Load(const char* section);
};
