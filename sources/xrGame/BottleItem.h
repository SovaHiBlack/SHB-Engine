// BottleItem - бутылка с напитком, которую можно разбить

#pragma once

#include "FoodItem.h"

class CBottleItem : public CFoodItem
{
private:
	using inherited					= CFoodItem;

public:
					CBottleItem		( );
	virtual			~CBottleItem	( );
	virtual void	Load			(const char* section);
	void			OnEvent			(NET_Packet& P, unsigned short type);
	virtual void	Hit				(SHit* pHDS);
	void			BreakToPieces	( );
	virtual void	UseBy			(CEntityAlive* entity_alive);

protected:
	float							m_alcohol;

	// партиклы и звук разбивания бутылки
	shared_str						m_sBreakParticles;
	ref_sound						sndBreaking;
};
