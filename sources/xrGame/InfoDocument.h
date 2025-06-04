///////////////////////////////////////////////////////////////
// InfoDocument.h
// InfoDocument - ��������, ���������� �������� ����������
///////////////////////////////////////////////////////////////

#pragma once

#include "InventoryItemObject.h"
#include "InfoPortion_defs.h"

class CInfoDocument: public CInventoryItemObject {
private:
    typedef	CInventoryItemObject inherited;
public:
	CInfoDocument(void);
	virtual ~CInfoDocument(void);

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(pcstr section);
	virtual void net_Destroy		();
	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	(bool just_before_destroy);

protected:
	//������ ����������, ������������ � ���������
	shared_str m_Info;
};
