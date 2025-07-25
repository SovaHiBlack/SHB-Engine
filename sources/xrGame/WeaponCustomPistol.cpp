#include "stdafx.h"

#include "Entity.h"
#include "WeaponCustomPistol.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponCustomPistol::CWeaponCustomPistol(pcstr name) : CWeaponMagazined(name, eST_WEAPON_PISTOL)
{ }

CWeaponCustomPistol::~CWeaponCustomPistol( )
{ }

void CWeaponCustomPistol::switch2_Fire( )
{
	m_bFireSingleShot = true;
	bWorking = false;
	m_iShotNum = 0;
	m_bStopedAfterQueueFired = false;
}

void CWeaponCustomPistol::FireEnd( )
{
	if (fTime <= 0.0f)
	{
		m_bPending = false;
		inherited::FireEnd( );
	}
}
