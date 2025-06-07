/////////////////////////////////////////////////////
// PDA_msgPDA_msg.h - ��������� ���������� ��� ������ PDA

#pragma once
#include "alife_space.h"
#include "PDA_space.h"

//��������� ��� �������� ��������� PDA,
//������������ ��� ������� �����
typedef struct tagSPdaMessage
{
	EPdaMsg			msg;

	//true ���� �� �������� ��������� 
	//� false ���� �� ��� ��������
	bool			receive;

	//true, ���� ��������� - ������
	//� false, ���� �����
	bool			question;

	// ���������
	shared_str		info_id;

	//����� ���������/�������� ���������
	ALife::_TIME_ID	time;

} SPdaMessage;

//���������� � ��������� ���������� �� PDA � �� ����� �������
struct STalkContactData
{
	STalkContactData( ) : id(u16(-1)), time(0)
	{ }
	STalkContactData(u16 contact_id, ALife::_TIME_ID contact_time) : id(contact_id), time(contact_time)
	{ }
	//����� ��������
	ALife::_TIME_ID	time;
	//id ��������� � ������� ��������
	u16				id;
};

DEFINE_VECTOR(STalkContactData, TALK_CONTACT_VECTOR, TALK_CONTACT_VECTOR_IT);
