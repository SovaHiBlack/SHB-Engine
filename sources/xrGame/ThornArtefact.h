// ThornArtefact.h
// ThornArtefact - артефакт колючка

#pragma once

#include "Artefact.h"

class CThornArtefact : public CArtefact
{
private:
	using inherited = CArtefact;

public:
	CThornArtefact( );
	virtual ~CThornArtefact( );

	virtual void Load(const char* section);
};
