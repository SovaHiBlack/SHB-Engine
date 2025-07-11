////////////////////////////////////////////////////////////////////////////
//	Module 		: InventoryItem.h
//	Description : Inventory item
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Inventory_space.h"
#include "HitImmunity.h"
#include "AttachableItem.h"

enum EHandDependence
{
	eHD_NONE = 0,
	eHD_1_HAND = 1,
	eHD_2_HAND = 2
};

class CSE_Abstract;
class CGameObject;
class CFoodItem;
class CMissile;
class CHudItem;
class CWeaponAmmo;
class CWeapon;
class CPhysicsShellHolder;
class CNetPacket;
class CEatableItem;
struct SPHNetState;
struct net_update_IItem;
struct net_updateData;
class CInventoryOwner;

struct SHit;

class CInventoryItem :
	public CAttachableItem,
	public CHitImmunity
#ifdef DEBUG
	, public pureRender
#endif
{
private:
	typedef CAttachableItem inherited;

protected:
	enum EInventoryItemFlags
	{
		eIIF_DROP_MANUAL = (1 << 0),
		eIIF_CAN_TAKE = (1 << 1),
		FCanTrade = (1 << 2),
		Fbelt = (1 << 3),
		Fruck = (1 << 4),
		FRuckDefault = (1 << 5),
		FUsingCondition = (1 << 6),
		FAllowSprint = (1 << 7),
		Fuseful_for_NPC = (1 << 8),
		FInInterpolation = (1 << 9),
		FInInterpolate = (1 << 10),
		FIsQuestItem = (1 << 11)
	};

	flags16						m_flags;

public:
	CInventoryItem( );
	virtual						~CInventoryItem( );

public:
	virtual void				Load(pcstr section);

	virtual pcstr				Name( );
	virtual pcstr				NameShort( );
	shared_str					ItemDescription( )
	{
		return m_Description;
	}
	virtual void				GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count)
	{ }

	virtual void				OnEvent(CNetPacket& P, u16 type);

	virtual bool				Useful( ) const;									// !!! ��������������. (��. � Inventory.cpp)
	virtual bool				Attach(PIItem pIItem, bool b_send_event)
	{
		return false;
	}
	virtual bool				Detach(PIItem pIItem)
	{
		return false;
	}
	//��� ������ ��������� ����� ���� ��� ������� �������� ������
	virtual bool				Detach(pcstr item_section_name, bool b_spawn_item);
	virtual bool				CanAttach(PIItem pIItem)
	{
		return false;
	}
	virtual bool				CanDetach(pcstr item_section_name)
	{
		return false;
	}

	virtual EHandDependence		HandDependence( ) const
	{
		return eHD_1_HAND;
	}
	virtual bool				IsSingleHanded( ) const
	{
		return true;
	}
	virtual bool				Activate( );									// !!! ��������������. (��. � Inventory.cpp)
	virtual void				Deactivate( );								// !!! ��������������. (��. � Inventory.cpp)
	virtual bool				Action(s32 cmd, u32 flags)
	{
		return false;
	}	// true ���� ��������� �������, ����� false

	virtual bool				IsHidden( )	const
	{
		return true;
	}
	virtual bool				IsHiding( )	const
	{
		return false;
	}
	virtual bool 				IsShowing( )  const
	{
		return false;
	}

	virtual void				OnH_B_Chield( );
	virtual void				OnH_A_Chield( );
	virtual void				OnH_B_Independent(bool just_before_destroy);
	virtual void				OnH_A_Independent( );

	virtual void				save(CNetPacket& output_packet);
	virtual void				load(IReader& input_packet);
	virtual BOOL				net_SaveRelevant( )
	{
		return TRUE;
	}

	virtual void				UpdateCL( );

	virtual	void				Hit(SHit* pHDS);

	BOOL				GetDropManual( ) const
	{
		return m_flags.test(eIIF_DROP_MANUAL);
	}
	void				SetDropManual(BOOL val)
	{
		m_flags.set(eIIF_DROP_MANUAL, val);
	}

	BOOL				IsInvalid( ) const;

	BOOL				IsQuestItem( ) const
	{
		return m_flags.test(FIsQuestItem);
	}
	u32					Cost( ) const
	{
		return m_cost;
	}
	virtual f32				Weight( )
	{
		return m_weight;
	}

public:
	CInventory* m_pCurrentInventory;

	shared_str					m_name;
	shared_str					m_nameShort;
	shared_str					m_nameComplex;

	EItemPlace					m_eItemPlace;

	virtual void				OnMoveToSlot( )
	{ }
	virtual void				OnMoveToBelt( )
	{ }
	virtual void				OnMoveToRuck( )
	{ }

	s32					GetGridWidth( ) const;
	s32					GetGridHeight( ) const;
	const shared_str& GetIconName( ) const
	{
		return m_icon_name;
	}
	s32					GetXPos( ) const;
	s32					GetYPos( ) const;
	//---------------------------------------------------------------------
	f32				GetKillMsgXPos( ) const;
	f32				GetKillMsgYPos( ) const;
	f32				GetKillMsgWidth( ) const;
	f32				GetKillMsgHeight( ) const;
	//---------------------------------------------------------------------
	f32				GetCondition( ) const
	{
		return m_fCondition;
	}
	virtual	f32				GetConditionToShow( ) const
	{
		return GetCondition( );
	}
	void				ChangeCondition(f32 fDeltaCondition);

	virtual u32					GetSlot( ) const
	{
		return m_slot;
	}

	bool				Belt( )
	{
		return !!m_flags.test(Fbelt);
	}
	void				Belt(bool on_belt)
	{
		m_flags.set(Fbelt, on_belt);
	}
	bool				Ruck( )
	{
		return !!m_flags.test(Fruck);
	}
	void				Ruck(bool on_ruck)
	{
		m_flags.set(Fruck, on_ruck);
	}
	bool				RuckDefault( )
	{
		return !!m_flags.test(FRuckDefault);
	}

	virtual bool				CanTake( ) const
	{
		return !!m_flags.test(eIIF_CAN_TAKE);
	}
	bool				CanTrade( ) const;
	virtual bool 				IsNecessaryItem(CInventoryItem* item);
	virtual bool				IsNecessaryItem(const shared_str& item_sect)
	{
		return false;
	}

protected:
	u32							m_slot;
	u32							m_cost;
	f32						m_weight;
	f32						m_fCondition;
	shared_str					m_Description;

	ALife::_TIME_ID				m_dwItemRemoveTime;
	ALife::_TIME_ID				m_dwItemIndependencyTime;

	f32						m_fControlInertionFactor;
	shared_str					m_icon_name;

	////////// network //////////////////////////////////////////////////
public:
	virtual void				make_Interpolation( );
	virtual void				PH_B_CrPr( ); // actions & operations before physic correction-prediction steps
	virtual void				PH_I_CrPr( ); // actions & operations after correction before prediction steps

#ifdef DEBUG
	virtual void				PH_Ch_CrPr( ); // 
#endif
	virtual void				PH_A_CrPr( ); // actions & operations after phisic correction-prediction steps

	virtual void				net_Import(CNetPacket& P);					// import from server
	virtual void				net_Export(CNetPacket& P);					// export to server

public:
	virtual void				activate_physic_shell( );
	virtual u16					bone_count_to_synchronize( ) const;

	virtual bool				NeedToDestroyObject( ) const;
	virtual ALife::_TIME_ID		TimePassedAfterIndependant( ) const;

	virtual bool				IsSprintAllowed( ) const
	{
		return !!m_flags.test(FAllowSprint);
	}

	virtual	f32				GetControlInertionFactor( ) const
	{
		return m_fControlInertionFactor;
	}

protected:
	virtual void				UpdateXForm( );

protected:
	net_updateData* m_net_updateData;
	net_updateData* NetSync( );
	void						CalculateInterpolationParams( );

public:
	virtual BOOL				net_Spawn(CSE_Abstract* DC);
	virtual void				net_Destroy( );
	virtual void				reload(pcstr section);
	virtual void				reinit( );
	virtual bool				can_kill( ) const;
	virtual CInventoryItem* can_kill(CInventory* inventory) const;
	virtual const CInventoryItem* can_kill(const xr_vector<const CGameObject*>& items) const;
	virtual CInventoryItem* can_make_killing(const CInventory* inventory) const;
	virtual bool				ready_to_kill( ) const;
	IC		bool				useful_for_NPC( ) const;

#ifdef DEBUG
	virtual void				OnRender( );
#endif

public:
	virtual DLL_Pure* _construct( );
	IC	CPhysicsShellHolder& object( ) const
	{
		VERIFY(m_object);
		return		(*m_object);
	}
	virtual void				on_activate_physic_shell( )
	{
		R_ASSERT(0);
	} //sea

protected:
	f32						m_holder_range_modifier;
	f32						m_holder_fov_modifier;

public:
	virtual	void				modify_holder_params(f32& range, f32& fov) const;

protected:
	IC	CInventoryOwner& inventory_owner( ) const;

private:
	CPhysicsShellHolder* m_object;

public:
	virtual CInventoryItem* cast_inventory_item( )
	{
		return this;
	}
	virtual CAttachableItem* cast_attachable_item( )
	{
		return this;
	}
	virtual CPhysicsShellHolder* cast_physics_shell_holder( )
	{
		return 0;
	}
	virtual CEatableItem* cast_eatable_item( )
	{
		return 0;
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
		return 0;
	}
};

#include "InventoryItem_inline.h"
