///////////////////////////////////////////////////////////////
// RustyHairArtifact.h
// RustyHairArtefact - �������� ������ ������
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CRustyHairArtefact : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CRustyHairArtefact(void);
	virtual ~CRustyHairArtefact(void);

	virtual void Load				(pcstr section);

protected:
};