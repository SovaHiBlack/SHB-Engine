///////////////////////////////////////////////////////////////
// ZudaArtefact.h
// ZudaArtefact - �������� "����"
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CZudaArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CZudaArtefact( );
	virtual ~CZudaArtefact( );

	virtual void Load(pcstr section);
};
