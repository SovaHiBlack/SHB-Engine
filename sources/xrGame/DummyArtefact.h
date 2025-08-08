///////////////////////////////////////////////////////////////
// DummyArtefact.h
// DummyArtefact - артефакт пустышка
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CDummyArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CDummyArtefact( );
	virtual ~CDummyArtefact( );

	virtual void Load(pcstr section);
};
