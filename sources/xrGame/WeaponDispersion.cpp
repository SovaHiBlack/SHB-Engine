// WeaponDispersion.cpp: разбос при стрельбе

#include "stdafx.h"

#include "Weapon.h"
//#include "InventoryOwner.h"
#include "Actor.h"
#include "InventoryItem_impl.h"

#include "ActorEffector.h"
#include "effectorshot.h"
#include "EffectorShotX.h"

//возвращает 1, если оружие в отличном состоянии и >1 если повреждено
f32 CWeapon::GetConditionDispersionFactor( ) const
{
	return (1.0f + fireDispersionConditionFactor * (1.0f - GetCondition( )));
}

f32 CWeapon::GetFireDispersion(bool with_cartridge)
{
	if (!with_cartridge)
	{
		return GetFireDispersion(1.0f);
	}

	if (!m_magazine.empty( ))
	{
		m_fCurrentCartirdgeDisp = m_magazine.back( ).m_kDisp;
	}

	return GetFireDispersion(m_fCurrentCartirdgeDisp);
}

//текущая дисперсия (в радианах) оружия с учетом используемого патрона
f32 CWeapon::GetFireDispersion(f32 cartridge_k)
{
	//учет базовой дисперсии, состояние оружия и влияение патрона
	f32 fire_disp = fireDispersionBase * cartridge_k * GetConditionDispersionFactor( );

	//вычислить дисперсию, вносимую самим стрелком
	const CInventoryOwner* pOwner = smart_cast<const CInventoryOwner*>(H_Parent( ));
	VERIFY(pOwner);

	f32 parent_disp = pOwner->GetWeaponAccuracy( );
	fire_disp += parent_disp;

	return fire_disp;
}

//////////////////////////////////////////////////////////////////////////
// Для эффекта отдачи оружия
void CWeapon::AddShotEffector( )
{
	inventory_owner( ).on_weapon_shot_start(this);
}

void CWeapon::RemoveShotEffector( )
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(H_Parent( ));
	if (pInventoryOwner)
	{
		pInventoryOwner->on_weapon_shot_stop(this);
	}
}

void CWeapon::ClearShotEffector( )
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(H_Parent( ));
	if (pInventoryOwner)
	{
		pInventoryOwner->on_weapon_hide(this);
	}
}
