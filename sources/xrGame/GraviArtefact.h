///////////////////////////////////////////////////////////////
// GraviArtefact.h
// GraviArtefact - �������������� ��������, ������� �� �����
// � ����� ��� ������
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
	//��������� ���������
	f32 m_fJumpHeight;
	f32 m_fEnergy;
};
