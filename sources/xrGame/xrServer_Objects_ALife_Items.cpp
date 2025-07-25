////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects items for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrMessages.h"
#include "../xrNetServer/net_utils.h"
#include "clsid_game.h"
#include "xrServer_Objects_ALife_Items.h"
#include "clsid_game.h"

#ifndef XRGAME_EXPORTS
#	include "Bone.h"
#else
#	include "..\XR_3DA\Bone.h"
#	ifdef DEBUG
#		define PHPH_DEBUG
#	endif
#endif
#ifdef PHPH_DEBUG
#include "PHDebug.h"
#endif
////////////////////////////////////////////////////////////////////////////
// CSE_ALifeInventoryItem
////////////////////////////////////////////////////////////////////////////
CSE_ALifeInventoryItem::CSE_ALifeInventoryItem(pcstr caSection)
{
	//������� ��������� ����
	m_fCondition = 1.0f;

	m_fMass = pSettings->r_float(caSection, "inv_weight");
	m_dwCost = pSettings->r_u32(caSection, "cost");

	if (pSettings->line_exist(caSection, "condition"))
		m_fCondition = pSettings->r_float(caSection, "condition");

	if (pSettings->line_exist(caSection, "health_value"))
		m_iHealthValue = pSettings->r_s32(caSection, "health_value");
	else
		m_iHealthValue = 0;

	if (pSettings->line_exist(caSection, "food_value"))
		m_iFoodValue = pSettings->r_s32(caSection, "food_value");
	else
		m_iFoodValue = 0;

	m_fDeteriorationValue = 0;

	m_last_update_time = 0;

	State.quaternion.x = 0.f;
	State.quaternion.y = 0.f;
	State.quaternion.z = 1.f;
	State.quaternion.w = 0.f;

	State.angular_vel.set(0.f, 0.f, 0.f);
	State.linear_vel.set(0.f, 0.f, 0.f);
}

CSE_Abstract* CSE_ALifeInventoryItem::init( )
{
	m_self = smart_cast<CSE_ALifeObject*>(this);
	R_ASSERT(m_self);
//	m_self->m_flags.set			(CSE_ALifeObject::flSwitchOffline,TRUE);
	return						(base( ));
}

CSE_ALifeInventoryItem::~CSE_ALifeInventoryItem( )
{ }

void CSE_ALifeInventoryItem::STATE_Write(CNetPacket& tNetPacket)
{
	tNetPacket.w_float(m_fCondition);
	State.position = base( )->o_Position;
}

void CSE_ALifeInventoryItem::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	u16 m_wVersion = base( )->m_wVersion;
	if (m_wVersion > 52)
		tNetPacket.r_float(m_fCondition);

	State.position = base( )->o_Position;
}

static inline bool check(const u8& mask, const u8& test)
{
	return							(!!(mask & test));
}

void CSE_ALifeInventoryItem::UPDATE_Write(CNetPacket& tNetPacket)
{
	if (!m_u8NumItems)
	{
		tNetPacket.w_u8(0);
		return;
	}

	mask_num_items					num_items;
	num_items.mask = 0;
	num_items.num_items = m_u8NumItems;

	R_ASSERT2(
		num_items.num_items < (u8(1) << 5),
		make_string("%d", num_items.num_items)
	);

	if (State.enabled)									num_items.mask |= inventory_item_state_enabled;
	if (fis_zero(State.angular_vel.square_magnitude( )))	num_items.mask |= inventory_item_angular_null;
	if (fis_zero(State.linear_vel.square_magnitude( )))	num_items.mask |= inventory_item_linear_null;

	tNetPacket.w_u8(num_items.common);

	tNetPacket.w_vec3(State.position);

	tNetPacket.w_float_q8(State.quaternion.x, 0.f, 1.f);
	tNetPacket.w_float_q8(State.quaternion.y, 0.f, 1.f);
	tNetPacket.w_float_q8(State.quaternion.z, 0.f, 1.f);
	tNetPacket.w_float_q8(State.quaternion.w, 0.f, 1.f);

	if (!check(num_items.mask, inventory_item_angular_null))
	{
		tNetPacket.w_float_q8(State.angular_vel.x, 0.f, 10 * PI_MUL_2);
		tNetPacket.w_float_q8(State.angular_vel.y, 0.f, 10 * PI_MUL_2);
		tNetPacket.w_float_q8(State.angular_vel.z, 0.f, 10 * PI_MUL_2);
	}

	if (!check(num_items.mask, inventory_item_linear_null))
	{
		tNetPacket.w_float_q8(State.linear_vel.x, -32.f, 32.f);
		tNetPacket.w_float_q8(State.linear_vel.y, -32.f, 32.f);
		tNetPacket.w_float_q8(State.linear_vel.z, -32.f, 32.f);
	}
};

void CSE_ALifeInventoryItem::UPDATE_Read(CNetPacket& tNetPacket)
{
	tNetPacket.r_u8(m_u8NumItems);
	if (!m_u8NumItems)
	{
		return;
	}

	mask_num_items					num_items;
	num_items.common = m_u8NumItems;
	m_u8NumItems = num_items.num_items;

	R_ASSERT2(
		m_u8NumItems < (u8(1) << 5),
		make_string("%d", m_u8NumItems)
	);

	tNetPacket.r_vec3(State.position);

	tNetPacket.r_float_q8(State.quaternion.x, 0.f, 1.f);
	tNetPacket.r_float_q8(State.quaternion.y, 0.f, 1.f);
	tNetPacket.r_float_q8(State.quaternion.z, 0.f, 1.f);
	tNetPacket.r_float_q8(State.quaternion.w, 0.f, 1.f);

	State.enabled = check(num_items.mask, inventory_item_state_enabled);

	if (!check(num_items.mask, inventory_item_angular_null))
	{
		tNetPacket.r_float_q8(State.angular_vel.x, 0.f, 10 * PI_MUL_2);
		tNetPacket.r_float_q8(State.angular_vel.y, 0.f, 10 * PI_MUL_2);
		tNetPacket.r_float_q8(State.angular_vel.z, 0.f, 10 * PI_MUL_2);
	}
	else
		State.angular_vel.set(0.f, 0.f, 0.f);

	if (!check(num_items.mask, inventory_item_linear_null))
	{
		tNetPacket.r_float_q8(State.linear_vel.x, -32.0f, 32.0f);
		tNetPacket.r_float_q8(State.linear_vel.y, -32.0f, 32.0f);
		tNetPacket.r_float_q8(State.linear_vel.z, -32.0f, 32.0f);
	}
	else
		State.linear_vel.set(0.0f, 0.0f, 0.0f);
};

void CSE_ALifeInventoryItem::FillProps(pcstr pref, PropItemVec& values)
{
	PHelper( ).CreateFloat(values, PrepareKey(pref, *base( )->s_name, "Item condition"), &m_fCondition, 0.f, 1.f);
	CSE_ALifeObject* alife_object = smart_cast<CSE_ALifeObject*>(base( ));
	R_ASSERT(alife_object);
	PHelper( ).CreateFlag32(values, PrepareKey(pref, *base( )->s_name, "ALife\\Useful for AI"), &alife_object->m_flags, CSE_ALifeObject::flUsefulForAI);
	PHelper( ).CreateFlag32(values, PrepareKey(pref, *base( )->s_name, "ALife\\Visible for AI"), &alife_object->m_flags, CSE_ALifeObject::flVisibleForAI);
}

bool CSE_ALifeInventoryItem::bfUseful( )
{
	return						(true);
}

u32 CSE_ALifeInventoryItem::update_rate( ) const
{
	return						(1000);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItem
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItem::CSE_ALifeItem(pcstr caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_ALifeInventoryItem(caSection)
{
	m_physics_disabled = false;
}

CSE_ALifeItem::~CSE_ALifeItem( )
{ }

CSE_Abstract* CSE_ALifeItem::init( )
{
	inherited1::init( );
	inherited2::init( );
	return						(base( ));
}

CSE_Abstract* CSE_ALifeItem::base( )
{
	return						(inherited1::base( ));
}

const CSE_Abstract* CSE_ALifeItem::base( ) const
{
	return						(inherited1::base( ));
}

void CSE_ALifeItem::STATE_Write(CNetPacket& tNetPacket)
{
	inherited1::STATE_Write(tNetPacket);
	inherited2::STATE_Write(tNetPacket);
}

void CSE_ALifeItem::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited1::STATE_Read(tNetPacket, size);
	if ((m_tClassID == CLSID_OBJECT_W_BINOCULAR) && (m_wVersion < 37))
	{
		tNetPacket.r_u16( );
		tNetPacket.r_u16( );
		tNetPacket.r_u8( );
	}
	inherited2::STATE_Read(tNetPacket, size);
}

void CSE_ALifeItem::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited1::UPDATE_Write(tNetPacket);
	inherited2::UPDATE_Write(tNetPacket);

#ifdef XRGAME_EXPORTS
	m_last_update_time = Device.dwTimeGlobal;
#endif // XRGAME_EXPORTS

}

void CSE_ALifeItem::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited1::UPDATE_Read(tNetPacket);
	inherited2::UPDATE_Read(tNetPacket);

	m_physics_disabled = false;
}

void CSE_ALifeItem::FillProps(pcstr pref, PropItemVec& values)
{
	inherited1::FillProps(pref, values);
	inherited2::FillProps(pref, values);
}

BOOL CSE_ALifeItem::Net_Relevant( )
{
	if (attached( ))
		return					(false);

	if (!m_physics_disabled && !fis_zero(State.linear_vel.square_magnitude( ), EPSILON_3))
		return					(true);

#ifdef XRGAME_EXPORTS
	if (Device.dwTimeGlobal < (m_last_update_time + update_rate( )))
		return					(false);
#endif // XRGAME_EXPORTS

	return						(true);
}

void CSE_ALifeItem::OnEvent(CNetPacket& tNetPacket, u16 type, u32 time, CClientID sender)
{
	inherited1::OnEvent(tNetPacket, type, time, sender);

	if (type != GE_FREEZE_OBJECT)
		return;

//	R_ASSERT					(!m_physics_disabled);
	m_physics_disabled = true;
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemTorch
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemTorch::CSE_ALifeItemTorch(pcstr caSection) : CSE_ALifeItem(caSection)
{
	m_active = false;
	m_nightvision_active = false;
	m_attached = false;
}

CSE_ALifeItemTorch::~CSE_ALifeItemTorch( )
{ }

BOOL	CSE_ALifeItemTorch::Net_Relevant( )
{
	if (m_attached) return true;
	return inherited::Net_Relevant( );
}


void CSE_ALifeItemTorch::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read(tNetPacket, size);

}

void CSE_ALifeItemTorch::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
}

void CSE_ALifeItemTorch::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);

	BYTE F = tNetPacket.r_u8( );
	m_active = !!(F & eTorchActive);
	m_nightvision_active = !!(F & eNightVisionActive);
	m_attached = !!(F & eAttached);
}

void CSE_ALifeItemTorch::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);

	BYTE F = 0;
	F |= (m_active ? eTorchActive : 0);
	F |= (m_nightvision_active ? eNightVisionActive : 0);
	F |= (m_attached ? eAttached : 0);
	tNetPacket.w_u8(F);
}

void CSE_ALifeItemTorch::FillProps(pcstr pref, PropItemVec& values)
{
	inherited::FillProps(pref, values);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemWeapon
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemWeapon::CSE_ALifeItemWeapon(pcstr caSection) : CSE_ALifeItem(caSection)
{
	a_current = 90;
	a_elapsed = 0;
	wpn_flags = 0;
	wpn_state = 0;
	ammo_type = 0;

	m_fHitPower = pSettings->r_float(caSection, "hit_power");
	m_tHitType = ALife::g_tfString2HitType(pSettings->r_string(caSection, "hit_type"));
	m_caAmmoSections = pSettings->r_string(caSection, "ammo_class");
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection, "visual"))
	{
		set_visual(pSettings->r_string(caSection, "visual"));
	}

	m_addon_flags.zero( );

	m_scope_status = (EWeaponAddonStatus)pSettings->r_s32(s_name, "scope_status");
	m_silencer_status = (EWeaponAddonStatus)pSettings->r_s32(s_name, "silencer_status");
	m_grenade_launcher_status = (EWeaponAddonStatus)pSettings->r_s32(s_name, "grenade_launcher_status");
	m_ef_main_weapon_type = READ_IF_EXISTS(pSettings, r_u32, caSection, "ef_main_weapon_type", u32(-1));
	m_ef_weapon_type = READ_IF_EXISTS(pSettings, r_u32, caSection, "ef_weapon_type", u32(-1));
}

CSE_ALifeItemWeapon::~CSE_ALifeItemWeapon( )
{ }

u32	CSE_ALifeItemWeapon::ef_main_weapon_type( ) const
{
	VERIFY(m_ef_main_weapon_type != u32(-1));
	return	(m_ef_main_weapon_type);
}

u32	CSE_ALifeItemWeapon::ef_weapon_type( ) const
{
	VERIFY(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

void CSE_ALifeItemWeapon::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);

	tNetPacket.r_float_q8(m_fCondition, 0.0f, 1.0f);
	tNetPacket.r_u8(wpn_flags);
	tNetPacket.r_u16(a_elapsed);
	tNetPacket.r_u8(m_addon_flags.flags);
	tNetPacket.r_u8(ammo_type);
	tNetPacket.r_u8(wpn_state);
	tNetPacket.r_u8(m_bZoom);
}

void CSE_ALifeItemWeapon::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);

	tNetPacket.w_float_q8(m_fCondition, 0.0f, 1.0f);
	tNetPacket.w_u8(wpn_flags);
	tNetPacket.w_u16(a_elapsed);
	tNetPacket.w_u8(m_addon_flags.get( ));
	tNetPacket.w_u8(ammo_type);
	tNetPacket.w_u8(wpn_state);
	tNetPacket.w_u8(m_bZoom);
}

void CSE_ALifeItemWeapon::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited::STATE_Read(tNetPacket, size);
	tNetPacket.r_u16(a_current);
	tNetPacket.r_u16(a_elapsed);
	tNetPacket.r_u8(wpn_state);

	if (m_wVersion > 40)
		tNetPacket.r_u8(m_addon_flags.flags);

	if (m_wVersion > 46)
		tNetPacket.r_u8(ammo_type);
}

void CSE_ALifeItemWeapon::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
	tNetPacket.w_u16(a_current);
	tNetPacket.w_u16(a_elapsed);
	tNetPacket.w_u8(wpn_state);
	tNetPacket.w_u8(m_addon_flags.get( ));
	tNetPacket.w_u8(ammo_type);
}

void CSE_ALifeItemWeapon::OnEvent(CNetPacket& tNetPacket, u16 type, u32 time, CClientID sender)
{
	inherited::OnEvent(tNetPacket, type, time, sender);
	switch (type)
	{
		case GE_WPN_STATE_CHANGE:
		{
			tNetPacket.r_u8(wpn_state);
//				u8 sub_state = 
			tNetPacket.r_u8( );
//				u8 NewAmmoType = 
			tNetPacket.r_u8( );
//				u8 AmmoElapsed = 
			tNetPacket.r_u8( );
		}
		break;
	}
}

u8 CSE_ALifeItemWeapon::get_slot( )
{
	return ((u8)pSettings->r_u8(s_name, "slot"));
}

u16 CSE_ALifeItemWeapon::get_ammo_total( )
{
	return (u16)a_current;
}

u16 CSE_ALifeItemWeapon::get_ammo_elapsed( )
{
	return (u16)a_elapsed;
}

u16 CSE_ALifeItemWeapon::get_ammo_magsize( )
{
	if (pSettings->line_exist(s_name, "ammo_mag_size"))
		return					(pSettings->r_u16(s_name, "ammo_mag_size"));
	else
		return					0;
}

BOOL CSE_ALifeItemWeapon::Net_Relevant( )
{
	return (wpn_flags == 1);
}

void CSE_ALifeItemWeapon::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
	PHelper( ).CreateU8(items, PrepareKey(pref, *s_name, "Ammo type:"), &ammo_type, 0, 255, 1);
	PHelper( ).CreateU16(items, PrepareKey(pref, *s_name, "Ammo: in magazine"), &a_elapsed, 0, 30, 1);

	if (m_scope_status == eAddonAttachable)
		PHelper( ).CreateFlag8(items, PrepareKey(pref, *s_name, "Addons\\Scope"), &m_addon_flags, eWeaponAddonScope);

	if (m_silencer_status == eAddonAttachable)
		PHelper( ).CreateFlag8(items, PrepareKey(pref, *s_name, "Addons\\Silencer"), &m_addon_flags, eWeaponAddonSilencer);

	if (m_grenade_launcher_status == eAddonAttachable)
		PHelper( ).CreateFlag8(items, PrepareKey(pref, *s_name, "Addons\\Podstvolnik"), &m_addon_flags, eWeaponAddonGrenadeLauncher);
}
////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemWeaponShotGun
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemWeaponShotGun::CSE_ALifeItemWeaponShotGun(pcstr caSection) : CSE_ALifeItemWeaponMagazined(caSection)
{
	m_AmmoIDs.clear( );
}

CSE_ALifeItemWeaponShotGun::~CSE_ALifeItemWeaponShotGun( )
{ }

void CSE_ALifeItemWeaponShotGun::UPDATE_Read(CNetPacket& P)
{
	inherited::UPDATE_Read(P);
	m_AmmoIDs.clear( );
	u8 AmmoCount = P.r_u8( );
	for (u8 i = 0; i < AmmoCount; i++)
	{
		m_AmmoIDs.push_back(P.r_u8( ));
	}
}

void CSE_ALifeItemWeaponShotGun::UPDATE_Write(CNetPacket& P)
{
	inherited::UPDATE_Write(P);
	P.w_u8(u8(m_AmmoIDs.size( )));
	for (u32 i = 0; i < m_AmmoIDs.size( ); i++)
	{
		P.w_u8(u8(m_AmmoIDs[i]));
	}
}

void CSE_ALifeItemWeaponShotGun::STATE_Read(CNetPacket& P, u16 size)
{
	inherited::STATE_Read(P, size);
}

void CSE_ALifeItemWeaponShotGun::STATE_Write(CNetPacket& P)
{
	inherited::STATE_Write(P);
}

void CSE_ALifeItemWeaponShotGun::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemWeaponMagazined
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemWeaponMagazined::CSE_ALifeItemWeaponMagazined(pcstr caSection) : CSE_ALifeItemWeapon(caSection)
{
	m_u8CurFireMode = 0;
}

CSE_ALifeItemWeaponMagazined::~CSE_ALifeItemWeaponMagazined( )
{ }

void CSE_ALifeItemWeaponMagazined::UPDATE_Read(CNetPacket& P)
{
	inherited::UPDATE_Read(P);
	m_u8CurFireMode = P.r_u8( );
}

void CSE_ALifeItemWeaponMagazined::UPDATE_Write(CNetPacket& P)
{
	inherited::UPDATE_Write(P);
	P.w_u8(m_u8CurFireMode);
}

void CSE_ALifeItemWeaponMagazined::STATE_Read(CNetPacket& P, u16 size)
{
	inherited::STATE_Read(P, size);
}

void CSE_ALifeItemWeaponMagazined::STATE_Write(CNetPacket& P)
{
	inherited::STATE_Write(P);
}

void CSE_ALifeItemWeaponMagazined::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemWeaponMagazinedWGL
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemWeaponMagazinedWGL::CSE_ALifeItemWeaponMagazinedWGL(pcstr caSection) : CSE_ALifeItemWeaponMagazined(caSection)
{
	m_bGrenadeMode = 0;
}

CSE_ALifeItemWeaponMagazinedWGL::~CSE_ALifeItemWeaponMagazinedWGL( )
{ }

void CSE_ALifeItemWeaponMagazinedWGL::UPDATE_Read(CNetPacket& P)
{
	m_bGrenadeMode = !!P.r_u8( );
	inherited::UPDATE_Read(P);
}

void CSE_ALifeItemWeaponMagazinedWGL::UPDATE_Write(CNetPacket& P)
{
	P.w_u8(m_bGrenadeMode ? 1 : 0);
	inherited::UPDATE_Write(P);
}

void CSE_ALifeItemWeaponMagazinedWGL::STATE_Read(CNetPacket& P, u16 size)
{
	inherited::STATE_Read(P, size);
}

void CSE_ALifeItemWeaponMagazinedWGL::STATE_Write(CNetPacket& P)
{
	inherited::STATE_Write(P);
}

void CSE_ALifeItemWeaponMagazinedWGL::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemAmmo
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemAmmo::CSE_ALifeItemAmmo(pcstr caSection) : CSE_ALifeItem(caSection)
{
	a_elapsed = m_boxSize = (u16)pSettings->r_s32(caSection, "box_size");
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection, "visual"))
	{
		set_visual(pSettings->r_string(caSection, "visual"));
	}
}

CSE_ALifeItemAmmo::~CSE_ALifeItemAmmo( )
{ }

void CSE_ALifeItemAmmo::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited::STATE_Read(tNetPacket, size);
	tNetPacket.r_u16(a_elapsed);
}

void CSE_ALifeItemAmmo::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
	tNetPacket.w_u16(a_elapsed);
}

void CSE_ALifeItemAmmo::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);
	tNetPacket.r_u16(a_elapsed);
}

void CSE_ALifeItemAmmo::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);
	tNetPacket.w_u16(a_elapsed);
}

void CSE_ALifeItemAmmo::FillProps(pcstr pref, PropItemVec& values)
{
	inherited::FillProps(pref, values);
	PHelper( ).CreateU16(values, PrepareKey(pref, *s_name, "Ammo: left"), &a_elapsed, 0, m_boxSize, m_boxSize);
}

bool CSE_ALifeItemAmmo::can_switch_online( ) const
{
	return inherited::can_switch_online( );
}

bool CSE_ALifeItemAmmo::can_switch_offline( ) const
{
	return (inherited::can_switch_offline( ) && a_elapsed != 0);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemDetector
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemDetector::CSE_ALifeItemDetector(pcstr caSection) : CSE_ALifeItem(caSection)
{
	m_ef_detector_type = pSettings->r_u32(caSection, "ef_detector_type");
}

CSE_ALifeItemDetector::~CSE_ALifeItemDetector( )
{ }

u32	 CSE_ALifeItemDetector::ef_detector_type( ) const
{
	return m_ef_detector_type;
}

void CSE_ALifeItemDetector::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	if (m_wVersion > 20)
	{
		inherited::STATE_Read(tNetPacket, size);
	}
}

void CSE_ALifeItemDetector::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
}

void CSE_ALifeItemDetector::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);
}

void CSE_ALifeItemDetector::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);
}

void CSE_ALifeItemDetector::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemDetector
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemArtefact::CSE_ALifeItemArtefact(pcstr caSection) : CSE_ALifeItem(caSection)
{
	m_fAnomalyValue = 100.f;
}

CSE_ALifeItemArtefact::~CSE_ALifeItemArtefact( )
{ }

void CSE_ALifeItemArtefact::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited::STATE_Read(tNetPacket, size);
}

void CSE_ALifeItemArtefact::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
}

void CSE_ALifeItemArtefact::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);
}

void CSE_ALifeItemArtefact::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);
}

void CSE_ALifeItemArtefact::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
	PHelper( ).CreateFloat(items, PrepareKey(pref, *s_name, "Anomaly value:"), &m_fAnomalyValue, 0.f, 200.f);
}

BOOL CSE_ALifeItemArtefact::Net_Relevant( )
{
	return							(inherited::Net_Relevant( ));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemPDA
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemPDA::CSE_ALifeItemPDA(pcstr caSection) : CSE_ALifeItem(caSection)
{
	m_original_owner = 0xffff;
	m_specific_character = NULL;
	m_info_portion = NULL;
}

CSE_ALifeItemPDA::~CSE_ALifeItemPDA( )
{ }

void CSE_ALifeItemPDA::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited::STATE_Read(tNetPacket, size);
	if (m_wVersion > 58)
	{
		tNetPacket.r(&m_original_owner, sizeof(m_original_owner));
	}

	if ((m_wVersion > 89) && (m_wVersion < 98))
	{
		int tmp, tmp2;
		tNetPacket.r(&tmp, sizeof(int));
		tNetPacket.r(&tmp2, sizeof(int));
		m_info_portion = NULL;
		m_specific_character = NULL;
	}
	else
	{
		tNetPacket.r_stringZ(m_specific_character);
		tNetPacket.r_stringZ(m_info_portion);
	}
}

void CSE_ALifeItemPDA::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
	tNetPacket.w(&m_original_owner, sizeof(m_original_owner));

#ifdef XRGAME_EXPORTS
	tNetPacket.w_stringZ(m_specific_character);
	tNetPacket.w_stringZ(m_info_portion);
#else
	shared_str		tmp_1 = NULL;
	shared_str						tmp_2 = NULL;

	tNetPacket.w_stringZ(tmp_1);
	tNetPacket.w_stringZ(tmp_2);
#endif

}

void CSE_ALifeItemPDA::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);
}

void CSE_ALifeItemPDA::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);
}

void CSE_ALifeItemPDA::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemDocument
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemDocument::CSE_ALifeItemDocument(pcstr caSection) : CSE_ALifeItem(caSection)
{
	m_wDoc = NULL;
}

CSE_ALifeItemDocument::~CSE_ALifeItemDocument( )
{ }

void CSE_ALifeItemDocument::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited::STATE_Read(tNetPacket, size);

	if (m_wVersion < 98)
	{
		u16 tmp;
		tNetPacket.r_u16(tmp);
		m_wDoc = NULL;
	}
	else
	{
		tNetPacket.r_stringZ(m_wDoc);
	}
}

void CSE_ALifeItemDocument::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
	tNetPacket.w_stringZ(m_wDoc);
}

void CSE_ALifeItemDocument::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);
}

void CSE_ALifeItemDocument::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);
}

void CSE_ALifeItemDocument::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
//	PHelper().CreateU16			(items, PrepareKey(pref, *s_name, "Document index :"), &m_wDocIndex, 0, 65535);
	PHelper( ).CreateRText(items, PrepareKey(pref, *s_name, "Info portion :"), &m_wDoc);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemGrenade
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemGrenade::CSE_ALifeItemGrenade(pcstr caSection) : CSE_ALifeItem(caSection)
{
	m_ef_weapon_type = READ_IF_EXISTS(pSettings, r_u32, caSection, "ef_weapon_type", u32(-1));
}

CSE_ALifeItemGrenade::~CSE_ALifeItemGrenade( )
{ }

u32	CSE_ALifeItemGrenade::ef_weapon_type( ) const
{
	VERIFY(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

void CSE_ALifeItemGrenade::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited::STATE_Read(tNetPacket, size);
}

void CSE_ALifeItemGrenade::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
}

void CSE_ALifeItemGrenade::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);
}

void CSE_ALifeItemGrenade::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);
}

void CSE_ALifeItemGrenade::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemExplosive
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemExplosive::CSE_ALifeItemExplosive(pcstr caSection) : CSE_ALifeItem(caSection)
{ }

CSE_ALifeItemExplosive::~CSE_ALifeItemExplosive( )
{ }

void CSE_ALifeItemExplosive::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited::STATE_Read(tNetPacket, size);
}

void CSE_ALifeItemExplosive::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
}

void CSE_ALifeItemExplosive::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);
}

void CSE_ALifeItemExplosive::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);
}

void CSE_ALifeItemExplosive::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemBolt
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemBolt::CSE_ALifeItemBolt(pcstr caSection) : CSE_ALifeItem(caSection)
{
	m_flags.set(flUseSwitches, FALSE);
	m_flags.set(flSwitchOffline, FALSE);
	m_ef_weapon_type = READ_IF_EXISTS(pSettings, r_u32, caSection, "ef_weapon_type", u32(-1));
}

CSE_ALifeItemBolt::~CSE_ALifeItemBolt( )
{ }

u32	CSE_ALifeItemBolt::ef_weapon_type( ) const
{
	VERIFY(m_ef_weapon_type != u32(-1));
	return m_ef_weapon_type;
}

void CSE_ALifeItemBolt::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
}

void CSE_ALifeItemBolt::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited::STATE_Read(tNetPacket, size);
}

void CSE_ALifeItemBolt::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);
}

void CSE_ALifeItemBolt::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);
}

bool CSE_ALifeItemBolt::can_save( ) const
{
	return						(false);//!attached());
}

bool CSE_ALifeItemBolt::used_ai_locations( ) const
{
	return false;
}

void CSE_ALifeItemBolt::FillProps(pcstr pref, PropItemVec& values)
{
	inherited::FillProps(pref, values);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemCustomOutfit
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemCustomOutfit::CSE_ALifeItemCustomOutfit(pcstr caSection) : CSE_ALifeItem(caSection)
{
	m_ef_equipment_type = pSettings->r_u32(caSection, "ef_equipment_type");
}

CSE_ALifeItemCustomOutfit::~CSE_ALifeItemCustomOutfit( )
{ }

u32 CSE_ALifeItemCustomOutfit::ef_equipment_type( ) const
{
	return m_ef_equipment_type;
}

void CSE_ALifeItemCustomOutfit::STATE_Read(CNetPacket& tNetPacket, u16 size)
{
	inherited::STATE_Read(tNetPacket, size);
}

void CSE_ALifeItemCustomOutfit::STATE_Write(CNetPacket& tNetPacket)
{
	inherited::STATE_Write(tNetPacket);
}

void CSE_ALifeItemCustomOutfit::UPDATE_Read(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Read(tNetPacket);
	tNetPacket.r_float_q8(m_fCondition, 0.0f, 1.0f);
}

void CSE_ALifeItemCustomOutfit::UPDATE_Write(CNetPacket& tNetPacket)
{
	inherited::UPDATE_Write(tNetPacket);
	tNetPacket.w_float_q8(m_fCondition, 0.0f, 1.0f);
}

void CSE_ALifeItemCustomOutfit::FillProps(pcstr pref, PropItemVec& items)
{
	inherited::FillProps(pref, items);
}

BOOL CSE_ALifeItemCustomOutfit::Net_Relevant( )
{
	return true;
}
