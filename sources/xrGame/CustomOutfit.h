#pragma once

#include "InventoryItemObject.h"

struct SBoneProtections;

class CCustomOutfit : public CInventoryItemObject
{
private:
	typedef	CInventoryItemObject inherited;

public:
	CCustomOutfit( );
	virtual							~CCustomOutfit( );

	virtual void					Load(pcstr section);

	//уменьшенная версия хита, для вызова, когда костюм надет на персонажа
	virtual void					Hit(f32 P, ALife::EHitType hit_type);

	//коэффициенты на которые домножается хит
	//при соответствующем типе воздействия
	//если на персонаже надет костюм
	f32								GetHitTypeProtection(ALife::EHitType hit_type, s16 element);
	f32								GetDefHitTypeProtection(ALife::EHitType hit_type);

	f32								HitThruArmour(f32 hit_power, s16 element, f32 AP);
	//коэффициент на который домножается потеря силы
	//если на персонаже надет костюм
	f32								GetPowerLoss( );

	virtual void					OnMoveToSlot( );
	virtual void					OnMoveToRuck( );

protected:
	HitImmunity::HitTypeSVec		m_HitTypeProtection;
	f32								m_fPowerLoss;

	shared_str						m_ActorVisual;
	shared_str						m_full_icon_name;
	SBoneProtections* m_boneProtection;

protected:
	u32								m_ef_equipment_type;

public:
	f32								m_additional_weight;
	f32								m_additional_weight2;
	shared_str						m_NightVisionSect;
	virtual u32						ef_equipment_type( ) const;
	virtual	BOOL					BonePassBullet(s32 boneID);
	const shared_str& GetFullIconName( ) const
	{
		return m_full_icon_name;
	}

	virtual void			net_Export(CNetPacket& P);
	virtual void			net_Import(CNetPacket& P);
};
