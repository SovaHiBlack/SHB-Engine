///////////////////////////////////////////////////////////////
// RustyHairArtefact.h
// RustyHairArtefact - �������� ������ ������
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CRustyHairArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CRustyHairArtefact( );
	virtual ~CRustyHairArtefact( );

	virtual void Load(pcstr section);
};
