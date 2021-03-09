// DummyArtefact.h
// DummyArtefact - артефакт пустышка

#pragma once

#include "Artefact.h"

class CDummyArtefact : public CArtefact
{
private:
	using inherited = CArtefact;

public:
	CDummyArtefact( );
	virtual ~CDummyArtefact( );

	virtual void Load(const char* section);
};
