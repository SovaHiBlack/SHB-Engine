#pragma once

#include "InventoryItem.h"

class CPHItem;
class CEntityAlive;

class CEatableItem : public CInventoryItem
{
private:
	using inherited								= CInventoryItem;

private:
	CPHItem*									m_physic_item;

public:
							CEatableItem		( );
	virtual					~CEatableItem		( );
	virtual DLL_Pure*		_construct			( );
	virtual CEatableItem*	cast_eatable_item	( )
	{
		return this;
	}

	virtual void			Load				(const char* section);
	virtual bool			Useful				( ) const;

	virtual BOOL			net_Spawn			(CSE_Abstract* DC);

	virtual void			OnH_B_Independent	(bool just_before_destroy);
	virtual void			UseBy				(CEntityAlive* npc);
	bool					Empty				( ) const
	{
		return m_iPortionsNum == 0;
	}

protected:
	//влияние при поедании вещи на параметры игрока
	float										m_fHealthInfluence;
	float										m_fPowerInfluence;
	float										m_fSatietyInfluence;
	float										m_fRadiationInfluence;
	float										m_fMaxPowerUpInfluence;
	//заживление ран на кол-во процентов
	float										m_fWoundsHealPerc;

	//количество порций еды,
	//-1 - порция одна и больше не бывает (чтоб не выводить надпись в меню)
	int											m_iPortionsNum;
	int											m_iStartPortionsNum;
};
