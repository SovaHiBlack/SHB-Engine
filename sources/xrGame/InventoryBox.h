#pragma once

#include "Inventory_space.h"
#include "GameObject.h"

class CInventoryBox : public CGameObject
{
	using inherited						= CGameObject;
	xr_vector<unsigned short>			m_items;

public:
	bool								m_in_use;
					CInventoryBox		( );
	virtual void	OnEvent				(CNetPacket& P, unsigned short type);
	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	net_Relcase			(CObject* O);
	void			AddAvailableItems	(TIItemContainer& items_container) const;
	bool			IsEmpty				( )
	{
		return m_items.empty( );
	}
};