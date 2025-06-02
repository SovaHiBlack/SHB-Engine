#pragma once

#include "InventoryItem.h"

class CPhysicItem;
class CEntityAlive;

class CEatableItem : public CInventoryItem {
private:
	typedef CInventoryItem	inherited;

private:
	CPhysicItem		*m_physic_item;

public:
							CEatableItem				();
	virtual					~CEatableItem				();
	virtual	DLL_Pure*		_construct					();
	virtual CEatableItem	*cast_eatable_item			()	{return this;}

	virtual void			Load						(pcstr section);
	virtual bool			Useful						() const;

	virtual BOOL			net_Spawn					(CSE_Abstract* DC);

	virtual void			OnH_B_Independent			(bool just_before_destroy);
	virtual	void			UseBy						(CEntityAlive* npc);
			bool			Empty						() const				{return m_iPortionsNum==0;};

protected:
	//������� ��� �������� ���� �� ��������� ������
	f32					m_fHealthInfluence;
	f32					m_fPowerInfluence;
	f32					m_fSatietyInfluence;
	f32					m_fRadiationInfluence;
	f32					m_fMaxPowerUpInfluence;
	//���������� ��� �� ���-�� ���������
	f32					m_fWoundsHealPerc;

	//���������� ������ ���, 
	//-1 - ������ ���� � ������ �� ������ (���� �� �������� ������� � ����)
	s32						m_iPortionsNum;
	s32						m_iStartPortionsNum;
};
