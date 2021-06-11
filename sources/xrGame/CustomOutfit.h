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

	virtual void					Load(const char* section);

	//уменьшенная версия хита, для вызова, когда костюм надет на персонажа
	virtual void					Hit(float P, ALife::EHitType hit_type);

	//коэффициенты на которые домножается хит
	//при соответствующем типе воздействия
	//если на персонаже надет костюм
	float							GetHitTypeProtection(ALife::EHitType hit_type, S16 element);
	float							GetDefHitTypeProtection(ALife::EHitType hit_type);

	float							HitThruArmour(float hit_power, S16 element, float AP);
	//коэффициент на который домножается потеря силы
	//если на персонаже надет костюм
	float							GetPowerLoss( );


	virtual void					OnMoveToSlot( );
	virtual void					OnMoveToRuck( );

protected:
	HitImmunity::HitTypeSVec		m_HitTypeProtection;
	float							m_fPowerLoss;

	shared_str						m_ActorVisual;
	shared_str						m_full_icon_name;
	SBoneProtections* m_boneProtection;

protected:
	u32								m_ef_equipment_type;

public:
	float							m_additional_weight;
	float							m_additional_weight2;
	shared_str						m_NightVisionSect;
	virtual u32						ef_equipment_type( ) const;
	virtual	BOOL					BonePassBullet(int boneID);
	const shared_str& GetFullIconName( ) const
	{
		return m_full_icon_name;
	};

	virtual void			net_Export(CNetPacket& P);
	virtual void			net_Import(CNetPacket& P);
};
