///////////////////////////////////////////////////////////////
// ThornArtefact.h
// ThornArtefact - �������� �������
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CThornArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CThornArtefact( );
	virtual ~CThornArtefact( );

	virtual void Load(pcstr section);
};
