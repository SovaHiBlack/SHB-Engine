#pragma once

#include "RocketLauncher.h"
#include "WeaponShotgun.h"
#include "script_export_space.h"

class CWeaponRG6 : public CRocketLauncher, public CWeaponShotgun
{
	typedef CRocketLauncher		inheritedRL;
	typedef CWeaponShotgun		inheritedSG;

public:
	virtual			~CWeaponRG6( );
	virtual BOOL	net_Spawn(CSE_Abstract* DC);
	virtual void	Load(pcstr section);
	virtual void	OnEvent(CNetPacket& P, u16 type);

protected:
	virtual void	FireStart( );
	virtual u8		AddCartridge(u8 cnt);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CWeaponRG6)
#undef script_type_list
#define script_type_list save_type_list(CWeaponRG6)
