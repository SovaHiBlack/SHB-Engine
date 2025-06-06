#pragma once

#include "CustomZone.h"
#include "script_export_space.h"

class CMosquitoBald : public CCustomZone
{
private:
	typedef	CCustomZone	inherited;

public:
	CMosquitoBald( );
	virtual ~CMosquitoBald( );

	virtual void Load(pcstr section);
	virtual void Postprocess(f32 /*val*/);
	virtual bool EnableEffector( )
	{
		return true;
	}

	virtual void Affect(SZoneObjectInfo* O);

protected:
	virtual bool BlowoutState( );
	//��� ���� ����� blowout ��������� ���� ���
	//����� ���� ��� ���� ������������ � ������ ���������
	bool m_bLastBlowoutUpdate;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CMosquitoBald)
#undef script_type_list
#define script_type_list save_type_list(CMosquitoBald)