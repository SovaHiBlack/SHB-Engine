#pragma once

#include "InventoryItemObject.h"

class CCartridge
{
public:
	CCartridge( );
	void						Load(const char* section, U8 LocalAmmoType);

	shared_str										m_ammoSect;
	enum
	{
		cfTracer = (1 << 0),
		cfRicochet = (1 << 1),
		cfCanBeUnlimited = (1 << 2),
		cfExplosive = (1 << 3)
	};

	float											m_kDist;
	float											m_kDisp;
	float											m_kHit;
	float											m_kImpulse;
	float											m_kPierce;
	float											m_kAP;
	float											m_kAirRes;
	int												m_buckShot;
	float											m_impair;
	float											fWallmarkSize;

	U8												m_u8ColorID;
	U8												m_LocalAmmoType;

	U16												bullet_material_idx;
	Flags8											m_flags;

	shared_str										m_InvShortName;
};

class CWeaponAmmo : public CInventoryItemObject
{
	using inherited = CInventoryItemObject;

public:
	CWeaponAmmo( );
	virtual						~CWeaponAmmo( );

	virtual CWeaponAmmo* cast_weapon_ammo( )
	{
		return this;
	}
	virtual void				Load(const char* section);
	virtual BOOL				net_Spawn(CSE_Abstract* DC);
	virtual void				net_Destroy( );
	virtual void				net_Export(CNetPacket& P);
	virtual void				net_Import(CNetPacket& P);
	virtual void				OnH_B_Chield( );
	virtual void				OnH_B_Independent(bool just_before_destroy);
	virtual void				UpdateCL( );
	virtual void				renderable_Render( );

	virtual bool				Useful( ) const;
	virtual float				Weight( );

	bool						Get(CCartridge& cartridge);

	float											m_kDist;
	float											m_kDisp;
	float											m_kHit;
	float											m_kImpulse;
	float											m_kPierce;
	float											m_kAP;
	float											m_kAirRes;
	int												m_buckShot;
	float											m_impair;
	float											fWallmarkSize;
	U8												m_u8ColorID;

	U16												m_boxSize;
	U16												m_boxCurr;
	bool											m_tracer;

	virtual CInventoryItem* can_make_killing(const CInventory* inventory) const;
};
