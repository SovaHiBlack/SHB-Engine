///////////////////////////////////////////////////////////////
// ElectricBallArtefact.h
// ElectricBallArtefact - артефакт электрический шар
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CElectricBallArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CElectricBallArtefact( );
	virtual ~CElectricBallArtefact( );

	virtual void Load(pcstr section);

protected:
	virtual void	UpdateCLChild( );
};
