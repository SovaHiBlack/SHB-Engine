///////////////////////////////////////////////////////////////
// MercuryBall.h
// MercuryBall - �������������� � ������������ ���
// �������������� � ����� �� �����
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CMercuryBall : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CMercuryBall( );
	virtual ~CMercuryBall( );

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
