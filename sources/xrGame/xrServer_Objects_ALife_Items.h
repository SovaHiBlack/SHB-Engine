//	Module 		: xrServer_Objects_ALife.h
//	Description : Server objects items for ALife simulator

#pragma once

#include "xrServer_Objects_ALife.h"
#include "PHSynchronize.h"
#include "Inventory_space.h"

#include "CharacterInfo_defs.h"
#include "InfoPortion_defs.h"

#pragma warning(push)
#pragma warning(disable:4005)

class CSE_ALifeItemAmmo;

SERVER_ENTITY_DECLARE_BEGIN0(CSE_ALifeInventoryItem)
public:
	enum
	{
		inventory_item_state_enabled = U8(1) << 0,
		inventory_item_angular_null = U8(1) << 1,
		inventory_item_linear_null = U8(1) << 2
	};

	union mask_num_items
	{
		struct
		{
			U8	num_items : 5;
			U8	mask : 3;
		};

		U8		common;
	};

public:
	float							m_fCondition;
	float							m_fMass;
	u32								m_dwCost;
	int								m_iHealthValue;
	int								m_iFoodValue;
	float							m_fDeteriorationValue;
	CSE_ALifeObject* m_self;
	u32								m_last_update_time;

	CSE_ALifeInventoryItem(const char* caSection);
	virtual							~CSE_ALifeInventoryItem( );
	// we need this to prevent virtual inheritance :-(
	virtual CSE_Abstract* base( ) = 0;
	virtual const CSE_Abstract* base( ) const = 0;
	virtual CSE_Abstract* init( );
	virtual CSE_Abstract* cast_abstract( )
	{
		return 0;
	}
	virtual CSE_ALifeInventoryItem* cast_inventory_item( )
	{
		return this;
	}
	virtual	u32						update_rate( ) const;
	// end of the virtual inheritance dependant code

	inline		bool					attached( ) const
	{
		return						(base( )->ID_Parent < 0xffff);
	}
	virtual bool					bfUseful( );

	/////////// network ///////////////
	U8								m_u8NumItems;
	SPHNetState						State;
	///////////////////////////////////
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeInventoryItem)
#define script_type_list save_type_list(CSE_ALifeInventoryItem)

		SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeItem, CSE_ALifeDynamicObjectVisual, CSE_ALifeInventoryItem)
		bool							m_physics_disabled;

	CSE_ALifeItem(const char* caSection);
	virtual							~CSE_ALifeItem( );
	virtual CSE_Abstract* base( );
	virtual const CSE_Abstract* base( ) const;
	virtual CSE_Abstract* init( );
	virtual CSE_Abstract* cast_abstract( )
	{
		return this;
	}
	virtual CSE_ALifeInventoryItem* cast_inventory_item( )
	{
		return this;
	}
	virtual BOOL					Net_Relevant( );
	virtual void					OnEvent(CNetPacket& tNetPacket, U16 type, u32 time, ClientID sender);
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItem)
#define script_type_list save_type_list(CSE_ALifeItem)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemTorch, CSE_ALifeItem)
		//флаги
		enum EStats
	{
		eTorchActive = (1 << 0),
		eNightVisionActive = (1 << 1),
		eAttached = (1 << 2)
	};
	bool							m_active;
	bool							m_nightvision_active;
	bool							m_attached;
	CSE_ALifeItemTorch(const char* caSection);
	virtual							~CSE_ALifeItemTorch( );
	virtual BOOL					Net_Relevant( );

	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemTorch)
#define script_type_list save_type_list(CSE_ALifeItemTorch)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemAmmo, CSE_ALifeItem)
		U16								a_elapsed;
	U16								m_boxSize;

	CSE_ALifeItemAmmo(const char* caSection);
	virtual							~CSE_ALifeItemAmmo( );
	virtual CSE_ALifeItemAmmo* cast_item_ammo( )
	{
		return this;
	}
	virtual bool					can_switch_online( ) const;
	virtual bool					can_switch_offline( ) const;
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemAmmo)
#define script_type_list save_type_list(CSE_ALifeItemAmmo)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeapon, CSE_ALifeItem)

			//возможность подключения аддонов
		enum EWeaponAddonStatus
	{
		eAddonDisabled = 0,	//нельзя присоеденить
		eAddonPermanent = 1,	//постоянно подключено по умолчанию
		eAddonAttachable = 2		//можно присоединять
	};

	//текущее состояние аддонов
	enum EWeaponAddonState
	{
		eWeaponAddonScope = 0x01,
		eWeaponAddonGrenadeLauncher = 0x02,
		eWeaponAddonSilencer = 0x04
	};

	EWeaponAddonStatus				m_scope_status;
	EWeaponAddonStatus				m_silencer_status;
	EWeaponAddonStatus				m_grenade_launcher_status;

	u32								timestamp;
	U8								wpn_flags;
	U8								wpn_state;
	U8								ammo_type;
	U16								a_current;
	U16								a_elapsed;
	float							m_fHitPower;
	ALife::EHitType					m_tHitType;
	const char* m_caAmmoSections;
	u32								m_dwAmmoAvailable;
	Flags8							m_addon_flags;
	U8								m_bZoom;
	u32								m_ef_main_weapon_type;
	u32								m_ef_weapon_type;

	CSE_ALifeItemWeapon(const char* caSection);
	virtual							~CSE_ALifeItemWeapon( );
	virtual void					OnEvent(CNetPacket& P, U16 type, u32 time, ClientID sender);
	virtual u32						ef_main_weapon_type( ) const;
	virtual u32						ef_weapon_type( ) const;
	U8								get_slot( );
	U16								get_ammo_total( );
	U16								get_ammo_elapsed( );
	U16								get_ammo_magsize( );

	virtual BOOL					Net_Relevant( );

	virtual CSE_ALifeItemWeapon* cast_item_weapon( )
	{
		return this;
	}
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemWeapon)
#define script_type_list save_type_list(CSE_ALifeItemWeapon)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeaponMagazined, CSE_ALifeItemWeapon)
		U8			m_u8CurFireMode;
	CSE_ALifeItemWeaponMagazined(const char* caSection);
	virtual							~CSE_ALifeItemWeaponMagazined( );

	virtual CSE_ALifeItemWeapon* cast_item_weapon( )
	{
		return this;
	}
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemWeaponMagazined)
#define script_type_list save_type_list(CSE_ALifeItemWeaponMagazined)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeaponMagazinedWGL, CSE_ALifeItemWeaponMagazined)
		bool			m_bGrenadeMode;
	CSE_ALifeItemWeaponMagazinedWGL(const char* caSection);
	virtual							~CSE_ALifeItemWeaponMagazinedWGL( );

	virtual CSE_ALifeItemWeapon* cast_item_weapon( )
	{
		return this;
	}
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemWeaponMagazinedWGL)
#define script_type_list save_type_list(CSE_ALifeItemWeaponMagazinedWGL)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeaponShotGun, CSE_ALifeItemWeaponMagazined)
		xr_vector<U8>				m_AmmoIDs;
	CSE_ALifeItemWeaponShotGun(const char* caSection);
	virtual							~CSE_ALifeItemWeaponShotGun( );

	virtual CSE_ALifeItemWeapon* cast_item_weapon( )
	{
		return this;
	}
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemWeaponShotGun)
#define script_type_list save_type_list(CSE_ALifeItemWeaponShotGun)


		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemDetector, CSE_ALifeItem)
		u32								m_ef_detector_type;
	CSE_ALifeItemDetector(const char* caSection);
	virtual							~CSE_ALifeItemDetector( );
	virtual u32						ef_detector_type( ) const;
	virtual CSE_ALifeItemDetector* cast_item_detector( )
	{
		return this;
	}
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemDetector)
#define script_type_list save_type_list(CSE_ALifeItemDetector)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemArtefact, CSE_ALifeItem)
		float							m_fAnomalyValue;
	CSE_ALifeItemArtefact(const char* caSection);
	virtual							~CSE_ALifeItemArtefact( );
	virtual BOOL					Net_Relevant( );
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemArtefact)
#define script_type_list save_type_list(CSE_ALifeItemArtefact)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemPDA, CSE_ALifeItem)
		U16								m_original_owner;
	CSharedString						m_specific_character;
	CSharedString						m_info_portion;

	CSE_ALifeItemPDA(const char* caSection);
	virtual							~CSE_ALifeItemPDA( );
	virtual CSE_ALifeItemPDA* cast_item_pda( )
	{
		return this;
	}
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemPDA)
#define script_type_list save_type_list(CSE_ALifeItemPDA)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemDocument, CSE_ALifeItem)
		CSharedString							m_wDoc;
	CSE_ALifeItemDocument(const char* caSection);
	virtual							~CSE_ALifeItemDocument( );
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemDocument)
#define script_type_list save_type_list(CSE_ALifeItemDocument)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemGrenade, CSE_ALifeItem)
		u32								m_ef_weapon_type;
	CSE_ALifeItemGrenade(const char* caSection);
	virtual							~CSE_ALifeItemGrenade( );
	virtual u32						ef_weapon_type( ) const;
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemGrenade)
#define script_type_list save_type_list(CSE_ALifeItemGrenade)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemExplosive, CSE_ALifeItem)
		CSE_ALifeItemExplosive(const char* caSection);
	virtual							~CSE_ALifeItemExplosive( );
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemExplosive)
#define script_type_list save_type_list(CSE_ALifeItemExplosive)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemBolt, CSE_ALifeItem)
		u32								m_ef_weapon_type;
	CSE_ALifeItemBolt(const char* caSection);
	virtual							~CSE_ALifeItemBolt( );
	virtual bool					can_save( ) const;
	virtual bool					used_ai_locations( ) const;
	virtual u32						ef_weapon_type( ) const;
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemBolt)
#define script_type_list save_type_list(CSE_ALifeItemBolt)

		SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemCustomOutfit, CSE_ALifeItem)
		u32								m_ef_equipment_type;
	CSE_ALifeItemCustomOutfit(const char* caSection);
	virtual							~CSE_ALifeItemCustomOutfit( );
	virtual u32						ef_equipment_type( ) const;
	virtual BOOL					Net_Relevant( );
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_ALifeItemCustomOutfit)
#define script_type_list save_type_list(CSE_ALifeItemCustomOutfit)

#pragma warning(pop)
