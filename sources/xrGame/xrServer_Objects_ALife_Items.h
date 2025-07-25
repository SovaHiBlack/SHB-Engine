////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects items for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_ItemsH
#define xrServer_Objects_ALife_ItemsH

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
	enum {
		inventory_item_state_enabled	= u8(1) << 0,
		inventory_item_angular_null		= u8(1) << 1,
		inventory_item_linear_null		= u8(1) << 2,
	};

	union mask_num_items {
		struct {
			u8	num_items : 5;
			u8	mask      : 3;
		};
		u8		common;
	};

public:
	f32							m_fCondition;
	f32							m_fMass;
	u32								m_dwCost;
	s32								m_iHealthValue;
	s32								m_iFoodValue;
	f32							m_fDeteriorationValue;
	CSE_ALifeObject					*m_self;
	u32								m_last_update_time;

									CSE_ALifeInventoryItem	(pcstr caSection);
	virtual							~CSE_ALifeInventoryItem	();
	// we need this to prevent virtual inheritance :-(
	virtual CSE_Abstract			*base					() = 0;
	virtual const CSE_Abstract		*base					() const = 0;
	virtual CSE_Abstract			*init					();
	virtual CSE_Abstract			*cast_abstract			() {return 0;};
	virtual CSE_ALifeInventoryItem	*cast_inventory_item	() {return this;};
	virtual	u32						update_rate				() const;
	// end of the virtual inheritance dependant code

	IC		bool					attached	() const
	{
		return						(base()->ID_Parent < 0xffff);
	}
	virtual bool					bfUseful();

	/////////// network ///////////////
	u8								m_u8NumItems;
	SPHNetState						State;
	///////////////////////////////////
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeInventoryItem)
#define script_type_list save_type_list(CSE_ALifeInventoryItem)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeItem,CSE_ALifeDynamicObjectVisual,CSE_ALifeInventoryItem)
	bool							m_physics_disabled;

									CSE_ALifeItem	(pcstr caSection);
	virtual							~CSE_ALifeItem	();
	virtual CSE_Abstract			*base			();
	virtual const CSE_Abstract		*base			() const;
	virtual CSE_Abstract			*init					();
	virtual CSE_Abstract			*cast_abstract			() {return this;};
	virtual CSE_ALifeInventoryItem	*cast_inventory_item	() {return this;};
	virtual BOOL					Net_Relevant			();
	virtual void					OnEvent					(CNetPacket& tNetPacket, u16 type, u32 time, CClientID sender );
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItem)
#define script_type_list save_type_list(CSE_ALifeItem)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemTorch,CSE_ALifeItem)
//�����
	enum EStats{
		eTorchActive				= (1<<0),
		eNightVisionActive			= (1<<1),
		eAttached					= (1<<2)
	};
	bool							m_active;
	bool							m_nightvision_active;
	bool							m_attached;
									CSE_ALifeItemTorch	(pcstr caSection);
    virtual							~CSE_ALifeItemTorch	();
	virtual BOOL					Net_Relevant			();

SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemTorch)
#define script_type_list save_type_list(CSE_ALifeItemTorch)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemAmmo,CSE_ALifeItem)
	u16								a_elapsed;
	u16								m_boxSize;

									CSE_ALifeItemAmmo	(pcstr caSection);
	virtual							~CSE_ALifeItemAmmo	();
	virtual CSE_ALifeItemAmmo		*cast_item_ammo		()  {return this;};
	virtual bool					can_switch_online	() const;
	virtual bool					can_switch_offline	() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemAmmo)
#define script_type_list save_type_list(CSE_ALifeItemAmmo)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeapon,CSE_ALifeItem)

	//����������� ����������� �������
	enum EWeaponAddonStatus{
		eAddonDisabled				= 0,	//������ ������������
		eAddonPermanent				= 1,	//��������� ���������� �� ���������
		eAddonAttachable			= 2		//����� ������������
	};

	//������� ��������� �������
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
	u8								wpn_flags;
	u8								wpn_state;
	u8								ammo_type;
	u16								a_current;
	u16								a_elapsed;
	f32							m_fHitPower;
	ALife::EHitType					m_tHitType;
	pcstr							m_caAmmoSections;
	u32								m_dwAmmoAvailable;
	flags8							m_addon_flags;
	u8								m_bZoom;
	u32								m_ef_main_weapon_type;
	u32								m_ef_weapon_type;

									CSE_ALifeItemWeapon	(pcstr caSection);
	virtual							~CSE_ALifeItemWeapon();
	virtual void					OnEvent				(CNetPacket& P, u16 type, u32 time, CClientID sender );
	virtual u32						ef_main_weapon_type	() const;
	virtual u32						ef_weapon_type		() const;
	u8								get_slot			();
	u16								get_ammo_total		();
	u16								get_ammo_elapsed	();
	u16								get_ammo_magsize	();

	virtual BOOL					Net_Relevant		();

	virtual CSE_ALifeItemWeapon		*cast_item_weapon	() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemWeapon)
#define script_type_list save_type_list(CSE_ALifeItemWeapon)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeaponMagazined,CSE_ALifeItemWeapon)
u8			m_u8CurFireMode;
CSE_ALifeItemWeaponMagazined(pcstr caSection);
virtual							~CSE_ALifeItemWeaponMagazined();

virtual CSE_ALifeItemWeapon		*cast_item_weapon	() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemWeaponMagazined)
#define script_type_list save_type_list(CSE_ALifeItemWeaponMagazined)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeaponMagazinedWGL, CSE_ALifeItemWeaponMagazined)
bool			m_bGrenadeMode;
CSE_ALifeItemWeaponMagazinedWGL(pcstr caSection);
virtual							~CSE_ALifeItemWeaponMagazinedWGL();

virtual CSE_ALifeItemWeapon		*cast_item_weapon	() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemWeaponMagazinedWGL)
#define script_type_list save_type_list(CSE_ALifeItemWeaponMagazinedWGL)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeaponShotGun,CSE_ALifeItemWeaponMagazined)
	xr_vector<u8>				m_AmmoIDs;
								CSE_ALifeItemWeaponShotGun(pcstr caSection);
virtual							~CSE_ALifeItemWeaponShotGun();

virtual CSE_ALifeItemWeapon		*cast_item_weapon	() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemWeaponShotGun)
#define script_type_list save_type_list(CSE_ALifeItemWeaponShotGun)


SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemDetector,CSE_ALifeItem)
	u32								m_ef_detector_type;
									CSE_ALifeItemDetector(pcstr caSection);
	virtual							~CSE_ALifeItemDetector();
	virtual u32						ef_detector_type() const;
	virtual CSE_ALifeItemDetector	*cast_item_detector		() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemDetector)
#define script_type_list save_type_list(CSE_ALifeItemDetector)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemArtefact,CSE_ALifeItem)
f32							m_fAnomalyValue;
									CSE_ALifeItemArtefact	(pcstr caSection);
	virtual							~CSE_ALifeItemArtefact	();
	virtual BOOL					Net_Relevant			();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemArtefact)
#define script_type_list save_type_list(CSE_ALifeItemArtefact)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemPDA,CSE_ALifeItem)
	u16								m_original_owner;
	shared_str						m_specific_character;
	shared_str						m_info_portion;

									CSE_ALifeItemPDA(pcstr caSection);
	virtual							~CSE_ALifeItemPDA();
	virtual CSE_ALifeItemPDA		*cast_item_pda				() {return this;};
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemPDA)
#define script_type_list save_type_list(CSE_ALifeItemPDA)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemDocument,CSE_ALifeItem)
	shared_str							m_wDoc;
									CSE_ALifeItemDocument(pcstr caSection);
	virtual							~CSE_ALifeItemDocument();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemDocument)
#define script_type_list save_type_list(CSE_ALifeItemDocument)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemGrenade,CSE_ALifeItem)
	u32								m_ef_weapon_type;
									CSE_ALifeItemGrenade	(pcstr caSection);
	virtual							~CSE_ALifeItemGrenade	();
	virtual u32						ef_weapon_type			() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemGrenade)
#define script_type_list save_type_list(CSE_ALifeItemGrenade)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemExplosive,CSE_ALifeItem)
									CSE_ALifeItemExplosive(pcstr caSection);
	virtual							~CSE_ALifeItemExplosive();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemExplosive)
#define script_type_list save_type_list(CSE_ALifeItemExplosive)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemBolt,CSE_ALifeItem)
	u32								m_ef_weapon_type;
									CSE_ALifeItemBolt	(pcstr caSection);
	virtual							~CSE_ALifeItemBolt	();
	virtual bool					can_save			() const;
	virtual bool					used_ai_locations	() const;
	virtual u32						ef_weapon_type		() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemBolt)
#define script_type_list save_type_list(CSE_ALifeItemBolt)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemCustomOutfit,CSE_ALifeItem)
	u32								m_ef_equipment_type;
									CSE_ALifeItemCustomOutfit	(pcstr caSection);
	virtual							~CSE_ALifeItemCustomOutfit	();
	virtual u32						ef_equipment_type			() const;
	virtual BOOL					Net_Relevant				();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemCustomOutfit)
#define script_type_list save_type_list(CSE_ALifeItemCustomOutfit)

#pragma warning(pop)

#endif