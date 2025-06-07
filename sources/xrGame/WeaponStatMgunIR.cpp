#include "stdafx.h"
#include "WeaponStatMgun.h"
#include "xr_level_controller.h"

void CWeaponStatMgun::OnMouseMove(s32 dx, s32 dy)
{
	if (Remote( ))
	{
		return;
	}

	f32 scale = psMouseSens * psMouseSensScale / 50.0f;
	f32 h;
	f32 p;
	m_destEnemyDir.getHP(h, p);
	if (dx)
	{
		f32 d = f32(dx) * scale;
		h -= d;
		SetDesiredDir(h, p);
	}

	if (dy)
	{
		f32 d = ((psMouseInvert.test(1)) ? -1 : 1) * f32(dy) * scale * 3.0f / 4.0f;
		p -= d;
		SetDesiredDir(h, p);
	}
}

void CWeaponStatMgun::OnKeyboardPress(s32 dik)
{
	if (Remote( ))
	{
		return;
	}

	switch (dik)
	{
		case kWPN_FIRE:
		{
			FireStart( );
		}
			break;
	}
}

void CWeaponStatMgun::OnKeyboardRelease(s32 dik)
{
	if (Remote( ))
	{
		return;
	}

	switch (dik)
	{
		case kWPN_FIRE:
		{
			FireEnd( );
		}
			break;
	}
}

void CWeaponStatMgun::OnKeyboardHold(s32 dik)
{ }
