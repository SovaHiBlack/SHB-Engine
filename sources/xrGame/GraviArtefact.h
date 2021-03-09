// GraviArtefact.h
// GraviArtefact - гравитационный артефакт, прыгает на месте и парит над землей

#pragma once

#include "Artefact.h"

class CGraviArtefact : public CArtefact
{
private:
	using inherited = CArtefact;

public:
	CGraviArtefact( );
	virtual ~CGraviArtefact( );

	virtual void Load(const char* section);

protected:
	virtual void	UpdateCLChild( );
	//параметры артефакта
	float m_fJumpHeight;
	float m_fEnergy;
};
