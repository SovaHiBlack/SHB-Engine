///////////////////////////////////////////////////////////////
// MercuryBallArtefact.h
// MercuryBallArtefact - �������������� � ������������ ���
// �������������� � ����� �� �����
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CMercuryBallArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CMercuryBallArtefact( );
	virtual ~CMercuryBallArtefact( );

	virtual void Load(pcstr section);

protected:
	virtual void	UpdateCLChild( );

	//����� ���������� ���������� ��������� ����
	ALife::_TIME_ID m_timeLastUpdate;
	//����� ����� ���������
	ALife::_TIME_ID m_timeToUpdate;

	//�������� ��������� ������� ����
	f32 m_fImpulseMin;
	f32 m_fImpulseMax;
};
