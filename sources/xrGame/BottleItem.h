///////////////////////////////////////////////////////////////
// BottleItem.h
// BottleItem - ������� � ��������, ������� ����� �������
///////////////////////////////////////////////////////////////

#pragma once

#include "FoodItem.h"

class CBottleItem : public CFoodItem
{
private:
	typedef	CFoodItem inherited;

public:
	CBottleItem( );
	virtual ~CBottleItem( );


	virtual void Load(pcstr section);


	void	OnEvent(CNetPacket& P, u16 type);


	virtual	void	Hit(SHit* pHDS);


	void					BreakToPieces( );
	virtual void					UseBy(CEntityAlive* entity_alive);

protected:
	f32		m_alcohol;
	//�������� ���������� �������
	shared_str m_sBreakParticles;
	ref_sound sndBreaking;
};
