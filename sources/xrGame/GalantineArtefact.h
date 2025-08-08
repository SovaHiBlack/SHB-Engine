///////////////////////////////////////////////////////////////
// GalantineArtefact.h
// GalantineArtefact - артефакт ведбмин студень
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CGalantineArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CGalantineArtefact( );
	virtual ~CGalantineArtefact( );

	virtual void Load(pcstr section);
};
