///////////////////////////////////////////////////////////////
// GalantineArtefact.h
// GalantineArtefact - �������� ������� �������
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
