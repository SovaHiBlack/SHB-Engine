///////////////////////////////////////////////////////////////
// Medkit.h
// Medkit - �������, ���������� ��������
///////////////////////////////////////////////////////////////

#pragma once

#include "EatableItemObject.h"

class CMedkit: public CEatableItemObject {
private:
    typedef	CEatableItemObject inherited;

public:
				 CMedkit			();
	virtual		 ~CMedkit			();
	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(pcstr section);
	virtual void net_Destroy		();
	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();
	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	(bool just_before_destroy);
};
