///////////////////////////////////////////////////////////////
// ZudaArtifact.h
// ZudaArtefact - �������� ����
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CZudaArtefact : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CZudaArtefact(void);
	virtual ~CZudaArtefact(void);

	virtual void Load				(pcstr section);

protected:
};