#include "stdafx.h"

#include "CustomOutfit.h"
#include "PhysicsShell.h"
#include "Inventory_space.h"
#include "Inventory.h"
#include "Actor.h"
#include "game_cl_base.h"
#include "Level.h"
#include "BoneProtections.h"
#include "Torch.h"

CCustomOutfit::CCustomOutfit( )
{
	m_slot = OUTFIT_SLOT;

	m_flags.set(FUsingCondition, TRUE);

	m_HitTypeProtection.resize(ALife::eHitTypeMax);
	for (s32 i = 0; i < ALife::eHitTypeMax; i++)
	{
		m_HitTypeProtection[i] = 1.0f;
	}

	m_boneProtection = xr_new<SBoneProtections>( );
}

CCustomOutfit::~CCustomOutfit( )
{
	xr_delete(m_boneProtection);
}

void CCustomOutfit::net_Export(CNetPacket& P)
{
	inherited::net_Export(P);
	P.w_float_q8(m_fCondition, 0.0f, 1.0f);
}

void CCustomOutfit::net_Import(CNetPacket& P)
{
	inherited::net_Import(P);
	P.r_float_q8(m_fCondition, 0.0f, 1.0f);
}

void CCustomOutfit::Load(pcstr section)
{
	inherited::Load(section);

	m_HitTypeProtection[ALife::eHitTypeBurn] = pSettings->r_float(section, "burn_protection");
	m_HitTypeProtection[ALife::eHitTypeStrike] = pSettings->r_float(section, "strike_protection");
	m_HitTypeProtection[ALife::eHitTypeShock] = pSettings->r_float(section, "shock_protection");
	m_HitTypeProtection[ALife::eHitTypeWound] = pSettings->r_float(section, "wound_protection");
	m_HitTypeProtection[ALife::eHitTypeRadiation] = pSettings->r_float(section, "radiation_protection");
	m_HitTypeProtection[ALife::eHitTypeTelepatic] = pSettings->r_float(section, "telepatic_protection");
	m_HitTypeProtection[ALife::eHitTypeChemicalBurn] = pSettings->r_float(section, "chemical_burn_protection");
	m_HitTypeProtection[ALife::eHitTypeExplosion] = pSettings->r_float(section, "explosion_protection");
	m_HitTypeProtection[ALife::eHitTypeFireWound] = pSettings->r_float(section, "fire_wound_protection");
	m_HitTypeProtection[ALife::eHitTypePhysicStrike] = READ_IF_EXISTS(pSettings, r_float, section, "physic_strike_protection", 0.0f);

	if (pSettings->line_exist(section, "actor_visual"))
	{
		m_ActorVisual = pSettings->r_string(section, "actor_visual");
	}
	else
	{
		m_ActorVisual = NULL;
	}

	m_ef_equipment_type = pSettings->r_u32(section, "ef_equipment_type");
	if (pSettings->line_exist(section, "power_loss"))
	{
		m_fPowerLoss = pSettings->r_float(section, "power_loss");
	}
	else
	{
		m_fPowerLoss = 1.0f;
	}

	m_additional_weight = pSettings->r_float(section, "additional_inventory_weight");
	m_additional_weight2 = pSettings->r_float(section, "additional_inventory_weight2");

	if (pSettings->line_exist(section, "nightvision_sect"))
	{
		m_NightVisionSect = pSettings->r_string(section, "nightvision_sect");
	}
	else
	{
		m_NightVisionSect = NULL;
	}

	m_full_icon_name = pSettings->r_string(section, "full_icon_name");
}

void CCustomOutfit::Hit(f32 hit_power, ALife::EHitType hit_type)
{
	hit_power *= m_HitTypeK[hit_type];
	ChangeCondition(-hit_power);
}

f32 CCustomOutfit::GetDefHitTypeProtection(ALife::EHitType hit_type)
{
	return (1.0f - m_HitTypeProtection[hit_type] * GetCondition( ));
}

f32 CCustomOutfit::GetHitTypeProtection(ALife::EHitType hit_type, s16 element)
{
	f32 fBase = m_HitTypeProtection[hit_type] * GetCondition( );
	f32 bone = m_boneProtection->getBoneProtection(element);
	return (1.0f - fBase * bone);
}

f32	CCustomOutfit::HitThruArmour(f32 hit_power, s16 element, f32 AP)
{
	f32 BoneArmour = m_boneProtection->getBoneArmour(element) * GetCondition( ) * (1.0f - AP);
	f32 NewHitPower = hit_power - BoneArmour;
	if (NewHitPower < hit_power * m_boneProtection->m_fHitFrac)
	{
		return (hit_power * m_boneProtection->m_fHitFrac);
	}

	return NewHitPower;
}

BOOL CCustomOutfit::BonePassBullet(s32 boneID)
{
	return m_boneProtection->getBonePassBullet(s16(boneID));
}

void CCustomOutfit::OnMoveToSlot( )
{
	if (m_pCurrentInventory)
	{
		CActor* pActor = smart_cast<CActor*> (m_pCurrentInventory->GetOwner( ));
		if (pActor)
		{
			if (m_ActorVisual.size( ))
			{
				shared_str NewVisual = NULL;
				pstr TeamSection = Game( ).getTeamSection(pActor->g_Team( ));
				if (TeamSection)
				{
					if (pSettings->line_exist(TeamSection, *cNameSect( )))
					{
						NewVisual = pSettings->r_string(TeamSection, *cNameSect( ));
						string256 SkinName;
						strcpy_s(SkinName, pSettings->r_string("mp_skins_path", "skin_path"));
						strcat_s(SkinName, *NewVisual);
						strcat_s(SkinName, ".ogf");
						NewVisual._set(SkinName);
					}
				}

				if (!NewVisual.size( ))
				{
					NewVisual = m_ActorVisual;
				}

				pActor->ChangeVisual(NewVisual);
			}

			if (pSettings->line_exist(cNameSect( ), "bones_koeff_protection"))
			{
				m_boneProtection->reload(pSettings->r_string(cNameSect( ), "bones_koeff_protection"), smart_cast<CKinematics*>(pActor->Visual( )));
			}
		}
	}
}

void CCustomOutfit::OnMoveToRuck( )
{
	if (m_pCurrentInventory)
	{
		CActor* pActor = smart_cast<CActor*> (m_pCurrentInventory->GetOwner( ));
		if (pActor)
		{
			CTorch* pTorch = smart_cast<CTorch*>(pActor->inventory( ).ItemFromSlot(TORCH_SLOT));
			if (pTorch)
			{
				pTorch->SwitchNightVision(false);
			}

			if (m_ActorVisual.size( ))
			{
				shared_str DefVisual = pActor->GetDefaultVisualOutfit( );
				if (DefVisual.size( ))
				{
					pActor->ChangeVisual(DefVisual);
				}
			}
		}
	}
}

u32	CCustomOutfit::ef_equipment_type( ) const
{
	return m_ef_equipment_type;
}

f32 CCustomOutfit::GetPowerLoss( )
{
	if (m_fPowerLoss < 1.0f && GetCondition( ) <= 0.0f)
	{
		return 1.0f;
	}

	return m_fPowerLoss;
}
