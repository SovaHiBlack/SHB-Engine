///////////////////////////////////////////////////////////////
// Silencer.h
// Silencer - апгрейд оружия глушитель 
///////////////////////////////////////////////////////////////

#pragma once
#include "InventoryItemObject.h"

class CSilencer : public CInventoryItemObject
{
private:
	typedef CInventoryItemObject inherited;

public:
	CSilencer( );
	virtual ~CSilencer( );

	virtual BOOL net_Spawn(CSE_Abstract* DC);
	virtual void Load(pcstr section);
	virtual void net_Destroy( );

	virtual void OnH_A_Chield( );
	virtual void OnH_B_Independent(bool just_before_destroy);

	virtual void UpdateCL( );
	virtual void renderable_Render( );
};
