#include "stdafx.h"
#include "radioactivezone.h"
#include "hudmanager.h"
#include "level.h"
#include "xrmessages.h"
#include "..\XR_3DA\Bone.h"
#include "clsid_game.h"
#include "game_base_space.h"
#include "Hit.h"
#include "actor.h"

CRadioactiveZone::CRadioactiveZone( )
{ }

CRadioactiveZone::~CRadioactiveZone( )
{ }

void CRadioactiveZone::Load(pcstr section)
{
	inherited::Load(section);
}

bool CRadioactiveZone::BlowoutState( )
{
	bool result = inherited::BlowoutState( );
	if (!result)
	{
		UpdateBlowout( );
	}

	return result;
}

void CRadioactiveZone::Affect(SZoneObjectInfo* O)
{
	// вермя срабатывания не чаще, чем заданный период
	if (m_dwDeltaTime < m_dwPeriod)
	{
		return;
	}

	CGameObject* GO = O->object;
	if (GO)
	{
		fVector3 pos;
		XFORM( ).transform_tiny(pos, CFORM( )->getSphere( ).P);

#ifdef DEBUG
		char pow[255];
		sprintf_s(pow, "zone hit. %.3f", Power(GO->Position( ).distance_to(pos)));
		if (bDebug)
		{
			Msg("%s %s", *GO->cName( ), pow);
		}
#endif // def DEBUG

		fVector3 dir;
		dir.set(0.0f, 0.0f, 0.0f);

		fVector3 position_in_bone_space;
		f32 power = Power(GO->Position( ).distance_to(pos));
		f32 impulse = 0.0f;
		if (power > EPSILON_5)
		{
			position_in_bone_space.set(0.0f, 0.0f, 0.0f);
			CreateHit(GO->ID( ), ID( ), dir, power, BI_NONE, position_in_bone_space, impulse, ALife::eHitTypeRadiation);
		}
	}
}

void CRadioactiveZone::feel_touch_new(CObject* O)
{
	inherited::feel_touch_new(O);
}

BOOL CRadioactiveZone::feel_touch_contact(CObject* O)
{
	CActor* A = smart_cast<CActor*>(O);
	if (A)
	{
		if (!((CCF_Shape*)CFORM( ))->Contact(O))
		{
			return FALSE;
		}

		return A->feel_touch_on_contact(this);
	}
	else
	{
		return FALSE;
	}
}

void CRadioactiveZone::UpdateWorkload(u32	dt)
{
	inherited::UpdateWorkload(dt);
}
