////////////////////////////////////////////////////////////////////////////
//	Module 		: HudItemObject.h
//	Description : HUD item
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InventoryItemObject.h"
#include "HudItem.h"

class CHudItemObject : public CInventoryItemObject, public CHudItem
{
protected: //���� ������ ���� ������� �� ������
	CHudItemObject( );
	virtual				~CHudItemObject( );

public:
	virtual	DLL_Pure* _construct( );

public:
	virtual CHudItem* cast_hud_item( )
	{
		return this;
	}

public:
	virtual void		Load(pcstr section);
	virtual bool		Action(s32 cmd, u32 flags);
	virtual void		SwitchState(u32 S);
	virtual void		OnStateSwitch(u32 S);
	virtual void		OnEvent(CNetPacket& P, u16 type);
	virtual void		OnH_A_Chield( );
	virtual void		OnH_B_Chield( );
	virtual void		OnH_B_Independent(bool just_before_destroy);
	virtual void		OnH_A_Independent( );
	virtual	BOOL		net_Spawn(CSE_Abstract* DC);
	virtual void		net_Destroy( );
	virtual bool		Activate( );
	virtual void		Deactivate( );
	virtual void		UpdateCL( );
	virtual void		renderable_Render( );
	virtual void		on_renderable_Render( );

	virtual bool			use_parent_ai_locations( ) const
	{
		return				(Device.dwFrame != dwXF_Frame);
	}
};
