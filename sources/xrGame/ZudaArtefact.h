// ZudaArtefact.h
// ZudaArtefact - артефакт зуда

#pragma once

#include "Artefact.h"

class CZudaArtefact : public CArtefact
{
private:
	using inherited = CArtefact;

public:
	CZudaArtefact( );
	virtual ~CZudaArtefact( );

	virtual void Load(const char* section);
};
