///////////////////////////////////////////////////////////////
// MercuryBall.h
// MercuryBall - �������������� � ������������ ���
// �������������� � ����� �� �����
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CMercuryBall : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CMercuryBall(void);
	virtual ~CMercuryBall(void);

	virtual void Load				(pcstr section);
protected:
	virtual void	UpdateCLChild	();

	//����� ���������� ���������� ��������� ����
	ALife::_TIME_ID m_timeLastUpdate;
	//����� ����� ���������
	ALife::_TIME_ID m_timeToUpdate;

	//�������� ��������� ������� ����
	f32 m_fImpulseMin;
	f32 m_fImpulseMax;
};

/*

#pragma once
#include "gameobject.h"
#include "PhysicsShell.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// ������� ���
// ���������� ����� �������, �������� �������, ����� ���� ����������.
// ����:  �� 50 �� 200 ������, � ����������� �� ������� 
// ���������: ������� ���������� �����������, ������� ������ � ���������� ����������,
// �������� � ������� R1.
class CMercuryBall : public CGameObject {
typedef	CGameObject	inherited;
public:
	CMercuryBall(void);
	virtual ~CMercuryBall(void);

	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent(bool just_before_destroy);

	
	virtual BOOL			net_Spawn			(CSE_Abstract* DC);
};
*/