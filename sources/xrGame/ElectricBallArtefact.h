// ElectricBallArtefact.h
// ElectricBallArtefact - артефакт электрический шар

#pragma once

#include "Artefact.h"

class CElectricBallArtefact : public CArtefact
{
private:
	using inherited = CArtefact;

public:
	CElectricBallArtefact( );
	virtual ~CElectricBallArtefact( );

	virtual void Load(const char* section);

protected:
	virtual void	UpdateCLChild( );
};
