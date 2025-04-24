#pragma once

#include "inventory_item_object.h"

class CCartridge
{
public:
	CCartridge( );
	void Load(pcstr section, u8 LocalAmmoType);

	shared_str m_ammoSect;
	enum
	{
		cfTracer = (1 << 0),
		cfRicochet = (1 << 1),
		cfCanBeUnlimited = (1 << 2),
		cfExplosive = (1 << 3)
	};

	f32 m_kDist;
	f32 m_kDisp;
	f32 m_kHit;
	f32 m_kImpulse;
	f32 m_kPierce;
	f32 m_kAP;
	f32 m_kAirRes;
	s32 m_buckShot;
	f32 m_impair;
	f32 fWallmarkSize;

	u8 m_u8ColorID;
	u8 m_LocalAmmoType;

	u16 bullet_material_idx;
	flags8 m_flags;

	shared_str m_InvShortName;
};

class CWeaponAmmo : public CInventoryItemObject
{
	typedef CInventoryItemObject inherited;

public:
	CWeaponAmmo( );
	virtual							~CWeaponAmmo( );

	virtual CWeaponAmmo* cast_weapon_ammo( )
	{
		return this;
	}
	virtual void					Load(pcstr section);
	virtual BOOL					net_Spawn(CSE_Abstract* DC);
	virtual void					net_Destroy( );
	virtual void					net_Export(CNetPacket& P);
	virtual void					net_Import(CNetPacket& P);
	virtual void					OnH_B_Chield( );
	virtual void					OnH_B_Independent(bool just_before_destroy);
	virtual void					UpdateCL( );
	virtual void					renderable_Render( );

	virtual bool					Useful( ) const;
	virtual f32					Weight( );

	bool							Get(CCartridge& cartridge);

	f32 m_kDist;
	f32 m_kDisp;
	f32 m_kHit;
	f32 m_kImpulse;
	f32 m_kPierce;
	f32 m_kAP;
	f32 m_kAirRes;
	s32 m_buckShot;
	f32 m_impair;
	f32 fWallmarkSize;
	u8 m_u8ColorID;

	u16 m_boxSize;
	u16 m_boxCurr;
	bool m_tracer;

public:
	virtual CInventoryItem* can_make_killing(const CInventory* inventory) const;
};
