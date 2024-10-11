///////////////////////////////////////////////////////////////
// GraviArtifact.h
// GraviArtefact - �������������� ��������, ������� �� �����
// � ����� ��� ������
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CGraviArtefact : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CGraviArtefact(void);
	virtual ~CGraviArtefact(void);

	virtual void Load				(pcstr section);

protected:
	virtual void	UpdateCLChild	();
	//��������� ���������
	f32 m_fJumpHeight;
	f32 m_fEnergy;
};
