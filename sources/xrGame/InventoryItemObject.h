////////////////////////////////////////////////////////////////////////////
//	Module 		: InventoryItemObject.h
//	Description : Inventory item object implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "PhysicItem.h"
#include "InventoryItem.h"

class CInventoryItemObject : public CInventoryItem, public CPhysicItem
{
public:
	CInventoryItemObject( );
	virtual					~CInventoryItemObject( );
	virtual DLL_Pure* _construct( );

public:
	virtual CPhysicsShellHolder* cast_physics_shell_holder( )
	{
		return this;
	}
	virtual CInventoryItem* cast_inventory_item( )
	{
		return this;
	}
	virtual CAttachableItem* cast_attachable_item( )
	{
		return this;
	}
	virtual CWeapon* cast_weapon( )
	{
		return 0;
	}
	virtual CFoodItem* cast_food_item( )
	{
		return 0;
	}
	virtual CMissile* cast_missile( )
	{
		return 0;
	}
	virtual CHudItem* cast_hud_item( )
	{
		return 0;
	}
	virtual CWeaponAmmo* cast_weapon_ammo( )
	{
		return 0;
	}
	virtual CGameObject* cast_game_object( )
	{
		return this;
	}

public:
	virtual void	Load(pcstr section);
	virtual pcstr	Name( );
	virtual pcstr	NameShort( );
	virtual void	Hit(SHit* pHDS);

	virtual void	OnH_B_Independent(bool just_before_destroy);
	virtual void	OnH_A_Independent( );
	virtual void	OnH_B_Chield( );
	virtual void	OnH_A_Chield( );
	virtual void	UpdateCL( );
	virtual void	OnEvent(CNetPacket& P, u16 type);
	virtual BOOL	net_Spawn(CSE_Abstract* DC);
	virtual void	net_Destroy( );
	virtual void	net_Import(CNetPacket& P);					// import from server
	virtual void	net_Export(CNetPacket& P);					// export to server
	virtual void	save(CNetPacket& output_packet);
	virtual void	load(IReader& input_packet);
	virtual BOOL	net_SaveRelevant( )
	{
		return TRUE;
	}
	virtual void	renderable_Render( );
	virtual void	reload(pcstr section);
	virtual void	reinit( );
	virtual void	activate_physic_shell( );
	virtual void	on_activate_physic_shell( );
	virtual void	modify_holder_params(f32& range, f32& fov) const;

public:
	////////// network //////////////////////////////////////////////////
	virtual void	make_Interpolation( );
	virtual void	PH_B_CrPr( ); // actions & operations before physic correction-prediction steps
	virtual void	PH_I_CrPr( ); // actions & operations after correction before prediction steps

#ifdef DEBUG
	virtual void	PH_Ch_CrPr( ); // 
#endif

	virtual void	PH_A_CrPr( ); // actions & operations after phisic correction-prediction steps
	virtual bool	NeedToDestroyObject( ) const;

protected:
#ifdef DEBUG
	virtual void	OnRender( );
#endif

public:
	virtual bool	Useful( ) const;

public:
	virtual u32		ef_weapon_type( ) const;
};

#include "InventoryItem_inline.h"
