///////////////////////////////////////////////////////////////
// ElectricBall.h
// ElectricBall - �������� ������������� ���
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CElectricBall : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CElectricBall(void);
	virtual ~CElectricBall(void);

	virtual void Load				(pcstr section);

protected:
	virtual void	UpdateCLChild	();

};