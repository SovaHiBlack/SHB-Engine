#include "stdafx.h"
#include "WeaponStatMgun.h"
#include "xr_level_controller.h"


void CWeaponStatMgun::OnMouseMove			(int dx, int dy)
{
	if (Remote())	return;

	f32 scale		= psMouseSens * psMouseSensScale/50.f;
	f32 h,p;
	m_destEnemyDir.getHP(h,p);
	if (dx){
		f32 d		= f32(dx)*scale;
		h			-= d;
		SetDesiredDir						(h,p);
	}
	if (dy){
		f32 d		= ((psMouseInvert.test(1))?-1:1)* f32(dy)*scale*3.f/4.f;
		p			-= d;
		SetDesiredDir						(h,p);
	}
}

void CWeaponStatMgun::OnKeyboardPress		(int dik)
{
	if (Remote())							return;

	switch (dik)	
	{
	case kWPN_FIRE:					
		FireStart();
		break;
	};
}

void CWeaponStatMgun::OnKeyboardRelease	(int dik)
{
	if (Remote())							return;
	switch (dik)	
	{
	case kWPN_FIRE:
		FireEnd();
		break;
	};
}

void CWeaponStatMgun::OnKeyboardHold		(int dik)
{}
