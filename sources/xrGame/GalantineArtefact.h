// GalantineArtefact.h
// GalantineArtefact - артефакт ведьмин студень

#pragma once

#include "Artefact.h"

class CGalantineArtefact : public CArtefact 
{
private:
	using inherited = CArtefact;

public:
	CGalantineArtefact( );
	virtual ~CGalantineArtefact( );

	virtual void Load				(const char* section);
};
