///////////////////////////////////////////////////////////////
// BlackDropsArtefact.h
// BlackDropsArtefact - черные капли
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CBlackDropsArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CBlackDropsArtefact( );
	virtual ~CBlackDropsArtefact( );

	virtual void Load(pcstr section);
};
