// RustyHairArtefact.h
// RustyHairArtefact - артефакт ржавые волосы

#pragma once

#include "Artefact.h"

class CRustyHairArtefact : public CArtefact
{
private:
	using inherited = CArtefact;

public:
	CRustyHairArtefact( );
	virtual ~CRustyHairArtefact( );

	virtual void Load(const char* section);
};
