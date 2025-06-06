////////////////////////////////////////////////////////////////////////////
//	Module 		: TeamBaseZone.cpp
//	Description : Team base zone object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TeamBaseZone.h"
#include "xrserver_objects_alife_monsters.h"
#include "Hit.h"
#include "Actor.h"
#include "HUDManager.h"
#include "level.h"
#include "xrserver.h"
#include "game_cl_base.h"
#include "clsid_game.h"
#include "MapLocation.h"
#include "MapManager.h"
#include "..\XR_3DA\IGame_Persistent.h"

#ifdef DEBUG
#	include "DebugRenderer.h"
#endif

CTeamBaseZone::CTeamBaseZone( )
{ }

CTeamBaseZone::~CTeamBaseZone( )
{ }

void CTeamBaseZone::reinit( )
{
	inherited::reinit( );
}

void CTeamBaseZone::Center(fVector3& C) const
{
	XFORM( ).transform_tiny(C, CFORM( )->getSphere( ).P);
}

f32 CTeamBaseZone::Radius( ) const
{
	return (CFORM( )->getRadius( ));
}

BOOL CTeamBaseZone::net_Spawn(CSE_Abstract* DC)
{
	CCF_Shape* l_pShape = xr_new<CCF_Shape>(this);
	collidable.model = l_pShape;

	CSE_Abstract* l_tpAbstract = (CSE_Abstract*)(DC);
	CSE_ALifeTeamBaseZone* l_tpALifeScriptZone = smart_cast<CSE_ALifeTeamBaseZone*>(l_tpAbstract);
	R_ASSERT(l_tpALifeScriptZone);

	feel_touch.clear( );

	for (u32 i = 0; i < l_tpALifeScriptZone->shapes.size( ); ++i)
	{
		CSE_Shape::shape_def& S = l_tpALifeScriptZone->shapes[i];
		switch (S.type)
		{
			case 0:
			{
				l_pShape->add_sphere(S.data.sphere);
			}
			break;
			case 1:
			{
				l_pShape->add_box(S.data.box);
			}
			break;
		}
	}

	m_Team = l_tpALifeScriptZone->m_team;

	BOOL bOk = inherited::net_Spawn(DC);
	if (bOk)
	{
		l_pShape->ComputeBounds( );
		fVector3 P;
		XFORM( ).transform_tiny(P, CFORM( )->getSphere( ).P);
		setEnabled(TRUE);
	}

	return bOk;
}

void CTeamBaseZone::net_Destroy( )
{
	Level( ).MapManager( ).RemoveMapLocationByObjectID(ID( ));
	inherited::net_Destroy( );
}

void CTeamBaseZone::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	const fSphere& s = CFORM( )->getSphere( );
	fVector3 P;
	XFORM( ).transform_tiny(P, s.P);
	feel_touch_update(P, s.R);
}

void CTeamBaseZone::feel_touch_new(CObject* tpObject)
{
	if (OnServer( ) && tpObject->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		CNetPacket P_;

		u_EventGen(P_, GE_GAME_EVENT, ID( ));
		P_.w_u16(GAME_EVENT_PLAYER_ENTER_TEAM_BASE);
		P_.w_u16(tpObject->ID( ));
		P_.w_u8(GetZoneTeam( ));
		u_EventSend(P_, net_flags(TRUE, TRUE));
	}
}

void CTeamBaseZone::feel_touch_delete(CObject* tpObject)
{
	if (OnServer( ) && tpObject->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		CNetPacket P_;
		u_EventGen(P_, GE_GAME_EVENT, ID( ));
		P_.w_u16(GAME_EVENT_PLAYER_LEAVE_TEAM_BASE);
		P_.w_u16(tpObject->ID( ));
		P_.w_u8(GetZoneTeam( ));
		u_EventSend(P_, net_flags(TRUE, TRUE));
	}
}

BOOL CTeamBaseZone::feel_touch_contact(CObject* O)
{
	CActor* pActor = smart_cast<CActor*>(O);
	if (!pActor) return (FALSE);
	return ((CCF_Shape*)CFORM( ))->Contact(O);
}

#ifdef DEBUG
extern	flags32	dbg_net_Draw_Flags;
void CTeamBaseZone::OnRender( )
{
	if (!bDebug)
	{
		return;
	}

	if (!(dbg_net_Draw_Flags.is_any((1 << 3))))
	{
		return;
	}

	fVector3 l_half;
	l_half.set(0.5f, 0.5f, 0.5f);
	fMatrix4x4 l_ball;
	fMatrix4x4 l_box;
	xr_vector<CCF_Shape::shape_def>& l_shapes = ((CCF_Shape*)CFORM( ))->Shapes( );
	xr_vector<CCF_Shape::shape_def>::iterator l_pShape;

	for (l_pShape = l_shapes.begin( ); l_shapes.end( ) != l_pShape; ++l_pShape)
	{
		switch (l_pShape->type)
		{
			case 0:
			{
				fSphere& l_sphere = l_pShape->data.sphere;
				l_ball.scale(l_sphere.R, l_sphere.R, l_sphere.R);
				fVector3 l_p;
				XFORM( ).transform(l_p, l_sphere.P);
				l_ball.translate_add(l_p);
				Level( ).debug_renderer( ).draw_ellipse(l_ball, D3DCOLOR_XRGB(0, 255, 255));
			}
			break;
			case 1:
			{
				l_box.mul(XFORM( ), l_pShape->data.box);
				Level( ).debug_renderer( ).draw_obb(l_box, l_half, D3DCOLOR_XRGB(0, 255, 255));
			}
			break;
		}
	}
}
#endif
