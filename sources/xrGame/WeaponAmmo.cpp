#include "stdafx.h"
#include "WeaponAmmo.h"
#include "PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
#include "Actor_flags.h"
#include "Inventory.h"
#include "Weapon.h"
#include "BulletManager.h"
#include "AISpace.h"
#include "gamemtllib.h"
#include "level.h"
#include "StringTable.h"

#define BULLET_MANAGER_SECTION "bullet_manager"

CCartridge::CCartridge( )
{
	m_flags.assign(cfTracer | cfRicochet);
	m_ammoSect = NULL;
	m_kDist = m_kDisp = m_kHit = m_kImpulse = m_kPierce = 1.0f;
	m_kAP = 0.0f;
	m_kAirRes = 0.0f;
	m_buckShot = 1;
	m_impair = 1.0f;

	bullet_material_idx = u16(-1);
}

void CCartridge::Load(pcstr section, u8 LocalAmmoType)
{
	m_ammoSect = section;
	m_LocalAmmoType = LocalAmmoType;
	m_kDist = pSettings->r_float(section, "k_dist");
	m_kDisp = pSettings->r_float(section, "k_disp");
	m_kHit = pSettings->r_float(section, "k_hit");
	m_kImpulse = pSettings->r_float(section, "k_impulse");
	m_kPierce = pSettings->r_float(section, "k_pierce");
	m_kAP = READ_IF_EXISTS(pSettings, r_float, section, "k_ap", 0.0f);
	m_u8ColorID = READ_IF_EXISTS(pSettings, r_u8, section, "tracer_color_ID", 0);

	if (pSettings->line_exist(section, "k_air_resistance"))
	{
		m_kAirRes = pSettings->r_float(section, "k_air_resistance");
	}
	else
	{
		m_kAirRes = pSettings->r_float(BULLET_MANAGER_SECTION, "air_resistance_k");
	}

	m_flags.set(cfTracer, pSettings->r_bool(section, "tracer"));
	m_buckShot = pSettings->r_s32(section, "buck_shot");
	m_impair = pSettings->r_float(section, "impair");
	fWallmarkSize = pSettings->r_float(section, "wm_size");

	m_flags.set(cfCanBeUnlimited | cfRicochet, TRUE);
	if (pSettings->line_exist(section, "can_be_unlimited"))
	{
		m_flags.set(cfCanBeUnlimited, pSettings->r_bool(section, "can_be_unlimited"));
	}

	if (pSettings->line_exist(section, "explosive"))
	{
		m_flags.set(cfExplosive, pSettings->r_bool(section, "explosive"));
	}

	bullet_material_idx = GMLib.GetMaterialIdx(WEAPON_MATERIAL_NAME);
	VERIFY(u16(-1) != bullet_material_idx);
	VERIFY(fWallmarkSize > 0);

	m_InvShortName = CStringTable( ).translate(pSettings->r_string(section, "inv_name_short"));
}

CWeaponAmmo::CWeaponAmmo( )
{
	m_weight = .2f;
	m_flags.set(Fbelt, TRUE);
}

CWeaponAmmo::~CWeaponAmmo( )
{ }

void CWeaponAmmo::Load(pcstr section)
{
	inherited::Load(section);

	m_kDist = pSettings->r_float(section, "k_dist");
	m_kDisp = pSettings->r_float(section, "k_disp");
	m_kHit = pSettings->r_float(section, "k_hit");
	m_kImpulse = pSettings->r_float(section, "k_impulse");
	m_kPierce = pSettings->r_float(section, "k_pierce");
	m_kAP = READ_IF_EXISTS(pSettings, r_float, section, "k_ap", 0.0f);
	m_u8ColorID = READ_IF_EXISTS(pSettings, r_u8, section, "tracer_color_ID", 0);

	if (pSettings->line_exist(section, "k_air_resistance"))
	{
		m_kAirRes = pSettings->r_float(section, "k_air_resistance");
	}
	else
	{
		m_kAirRes = pSettings->r_float(BULLET_MANAGER_SECTION, "air_resistance_k");
	}

	m_tracer = !!pSettings->r_bool(section, "tracer");
	m_buckShot = pSettings->r_s32(section, "buck_shot");
	m_impair = pSettings->r_float(section, "impair");
	fWallmarkSize = pSettings->r_float(section, "wm_size");
	R_ASSERT(fWallmarkSize > 0);

	m_boxSize = (u16)pSettings->r_s32(section, "box_size");
	m_boxCurr = m_boxSize;
}

BOOL CWeaponAmmo::net_Spawn(CSE_Abstract* DC)
{
	BOOL bResult = inherited::net_Spawn(DC);
	CSE_Abstract* e = (CSE_Abstract*)(DC);
	CSE_ALifeItemAmmo* l_pW = smart_cast<CSE_ALifeItemAmmo*>(e);
	m_boxCurr = l_pW->a_elapsed;

	if (m_boxCurr > m_boxSize)
	{
		l_pW->a_elapsed = m_boxCurr = m_boxSize;
	}

	return bResult;
}

void CWeaponAmmo::net_Destroy( )
{
	inherited::net_Destroy( );
}

void CWeaponAmmo::OnH_B_Chield( )
{
	inherited::OnH_B_Chield( );
}

void CWeaponAmmo::OnH_B_Independent(bool just_before_destroy)
{
	if (!Useful( ))
	{

		if (Local( ))
		{
			DestroyObject( );
		}

		m_ready_to_destroy = true;
	}

	inherited::OnH_B_Independent(just_before_destroy);
}

bool CWeaponAmmo::Useful( ) const
{
	// ���� IItem ��� �� ��������� �������������, ������� true
	return !!m_boxCurr;
}

bool CWeaponAmmo::Get(CCartridge& cartridge)
{
	if (!m_boxCurr) return false;
	cartridge.m_ammoSect = cNameSect( );
	cartridge.m_kDist = m_kDist;
	cartridge.m_kDisp = m_kDisp;
	cartridge.m_kHit = m_kHit;
	cartridge.m_kImpulse = m_kImpulse;
	cartridge.m_kPierce = m_kPierce;
	cartridge.m_kAP = m_kAP;
	cartridge.m_kAirRes = m_kAirRes;
	cartridge.m_u8ColorID = m_u8ColorID;
	cartridge.m_flags.set(CCartridge::cfTracer, m_tracer);
	cartridge.m_buckShot = m_buckShot;
	cartridge.m_impair = m_impair;
	cartridge.fWallmarkSize = fWallmarkSize;
	cartridge.bullet_material_idx = GMLib.GetMaterialIdx(WEAPON_MATERIAL_NAME);
	cartridge.m_InvShortName = NameShort( );
	--m_boxCurr;
	if (m_pCurrentInventory)
	{
		m_pCurrentInventory->InvalidateState( );
	}

	return true;
}

void CWeaponAmmo::renderable_Render( )
{
	if (!m_ready_to_destroy)
	{
		inherited::renderable_Render( );
	}
}

void CWeaponAmmo::UpdateCL( )
{
	VERIFY2(_valid(renderable.xform), *cName( ));
	inherited::UpdateCL( );
	VERIFY2(_valid(renderable.xform), *cName( ));

	VERIFY2(_valid(renderable.xform), *cName( ));
}

void CWeaponAmmo::net_Export(CNetPacket& P)
{
	inherited::net_Export(P);

	P.w_u16(m_boxCurr);
}

void CWeaponAmmo::net_Import(CNetPacket& P)
{
	inherited::net_Import(P);

	P.r_u16(m_boxCurr);
}

CInventoryItem* CWeaponAmmo::can_make_killing(const CInventory* inventory) const
{
	VERIFY(inventory);

	TIItemContainer::const_iterator	I = inventory->m_all.begin( );
	TIItemContainer::const_iterator	E = inventory->m_all.end( );
	for (; I != E; ++I)
	{
		CWeapon* weapon = smart_cast<CWeapon*>(*I);
		if (!weapon)
		{
			continue;
		}

		xr_vector<shared_str>::const_iterator	i = std::find(weapon->m_ammoTypes.begin( ), weapon->m_ammoTypes.end( ), cNameSect( ));
		if (i != weapon->m_ammoTypes.end( ))
		{
			return weapon;
		}
	}

	return 0;
}

f32 CWeaponAmmo::Weight( )
{
	f32 res = inherited::Weight( );

	res *= (f32)m_boxCurr / (f32)m_boxSize;

	return res;
}
