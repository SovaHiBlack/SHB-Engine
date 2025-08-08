///////////////////////////////////////////////////////////////
// GraviArtefact.h
// GraviArtefact - гравитационный артефакт, прыгает на месте
// и парит над землей
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CGraviArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CGraviArtefact( );
	virtual ~CGraviArtefact( );

	virtual void Load(pcstr section);

protected:
	virtual void	UpdateCLChild( );
	//параметры артефакта
	f32 m_fJumpHeight;
	f32 m_fEnergy;
};
