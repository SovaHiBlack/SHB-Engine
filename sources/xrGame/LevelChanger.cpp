////////////////////////////////////////////////////////////////////////////
//	Module 		: LevelChanger.cpp
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LevelChanger.h"
#include "Hit.h"
#include "Actor.h"
#include "xrserver_objects_alife.h"
#include "level.h"
#include "ai_object_location.h"
#include "AISpace.h"
#include "level_graph.h"
#include "game_level_cross_table.h"

#include "HudManager.h"
#include "ui/UIGame.h"

#include "patrol_path.h"
#include "patrol_path_storage.h"

xr_vector<CLevelChanger*>	g_lchangers;

CLevelChanger::~CLevelChanger( )
{ }

void CLevelChanger::Center(fVector3& C) const
{
	XFORM( ).transform_tiny(C, CFORM( )->getSphere( ).P);
}

f32 CLevelChanger::Radius( ) const
{
	return CFORM( )->getRadius( );
}

void CLevelChanger::net_Destroy( )
{
	inherited::net_Destroy( );
	xr_vector<CLevelChanger*>::iterator it = std::find(g_lchangers.begin( ), g_lchangers.end( ), this);
	if (it != g_lchangers.end( ))
	{
		g_lchangers.erase(it);
	}
}

BOOL CLevelChanger::net_Spawn(CSE_Abstract* DC)
{
	m_entrance_time = 0;
	CCF_Shape* l_pShape = xr_new<CCF_Shape>(this);
	collidable.model = l_pShape;

	CSE_Abstract* l_tpAbstract = (CSE_Abstract*)(DC);
	CSE_ALifeLevelChanger* l_tpALifeLevelChanger = smart_cast<CSE_ALifeLevelChanger*>(l_tpAbstract);
	R_ASSERT(l_tpALifeLevelChanger);

	m_game_vertex_id = l_tpALifeLevelChanger->m_tNextGraphID;
	m_level_vertex_id = l_tpALifeLevelChanger->m_dwNextNodeID;
	m_position = l_tpALifeLevelChanger->m_tNextPosition;
	m_angles = l_tpALifeLevelChanger->m_tAngles;

	m_bSilentMode = !!l_tpALifeLevelChanger->m_bSilentMode;
	if (ai( ).get_level_graph( ))
	{
		//. this information should be computed in xrAI
		ai_location( ).level_vertex(ai( ).level_graph( ).vertex(u32(-1), Position( )));
		ai_location( ).game_vertex(ai( ).cross_table( ).vertex(ai_location( ).level_vertex_id( )).game_vertex_id( ));
	}

	feel_touch.clear( );

	for (u32 i = 0; i < l_tpALifeLevelChanger->shapes.size( ); ++i)
	{
		CSE_Shape::shape_def& S = l_tpALifeLevelChanger->shapes[i];
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

	BOOL bOk = inherited::net_Spawn(DC);
	if (bOk)
	{
		l_pShape->ComputeBounds( );
		fVector3 P;
		XFORM( ).transform_tiny(P, CFORM( )->getSphere( ).P);
		setEnabled(TRUE);
	}

	g_lchangers.push_back(this);
	return bOk;
}

void CLevelChanger::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	const fSphere& s = CFORM( )->getSphere( );
	fVector3 P;
	XFORM( ).transform_tiny(P, s.P);
	feel_touch_update(P, s.R);

	update_actor_invitation( );
}

void CLevelChanger::feel_touch_new(CObject* tpObject)
{
	CActor* l_tpActor = smart_cast<CActor*>(tpObject);
	VERIFY(l_tpActor);
	if (!l_tpActor->g_Alive( ))
	{
		return;
	}

	if (m_bSilentMode)
	{
		CNetPacket p;
		p.w_begin(M_CHANGE_LEVEL);
		p.w(&m_game_vertex_id, sizeof(m_game_vertex_id));
		p.w(&m_level_vertex_id, sizeof(m_level_vertex_id));
		p.w_vec3(m_position);
		p.w_vec3(m_angles);
		Level( ).Send(p, net_flags(TRUE));
		return;
	}

	fVector3		p;
	fVector3		r;
	bool			b = get_reject_pos(p, r);
	CUIGame* pGame = smart_cast<CUIGame*>(HUD( ).GetUI( )->UIGame( ));
	if (pGame)
	{
		pGame->ChangeLevel(m_game_vertex_id, m_level_vertex_id, m_position, m_angles, p, r, b);
	}

	m_entrance_time = Device.fTimeGlobal;
}

bool CLevelChanger::get_reject_pos(fVector3& p, fVector3& r)
{
	p.set(0.0f, 0.0f, 0.0f);
	r.set(0.0f, 0.0f, 0.0f);
	//--		db.actor:set_actor_position(patrol("t_way"):point(0))
	//--		local dir = patrol("t_look"):point(0):sub(patrol("t_way"):point(0))
	//--		db.actor:set_actor_direction(-dir:getH())

	if (m_ini_file && m_ini_file->section_exist("pt_move_if_reject"))
	{
		pcstr p_name = m_ini_file->r_string("pt_move_if_reject", "path");
		const CPatrolPath* patrol_path = ai( ).patrol_paths( ).path(p_name);
		VERIFY(patrol_path);

		const CPatrolPoint* pt;
		pt = &patrol_path->vertex(0)->data( );
		p = pt->position( );

		fVector3 tmp;
		pt = &patrol_path->vertex(1)->data( );
		tmp.sub(pt->position( ), p);
		tmp.getHP(r.y, r.x);
		return true;
	}

	return false;
}

BOOL CLevelChanger::feel_touch_contact(CObject* object)
{
	return ((((CCF_Shape*)CFORM( ))->Contact(object)) && smart_cast<CActor*>(object));
}

void CLevelChanger::update_actor_invitation( )
{
	if (m_bSilentMode)
	{
		return;
	}

	xr_vector<CObject*>::iterator it = feel_touch.begin( );
	xr_vector<CObject*>::iterator it_e = feel_touch.end( );

	for (; it != it_e; ++it)
	{
		CActor* l_tpActor = smart_cast<CActor*>(*it);
		VERIFY(l_tpActor);

		if (m_entrance_time + 5.0f < Device.fTimeGlobal)
		{
			CUIGame* pGame = smart_cast<CUIGame*>(HUD( ).GetUI( )->UIGame( ));
			fVector3 p;
			fVector3 r;
			bool b = get_reject_pos(p, r);
			if (pGame)
			{
				pGame->ChangeLevel(m_game_vertex_id, m_level_vertex_id, m_position, m_angles, p, r, b);
			}

			m_entrance_time = Device.fTimeGlobal;
		}
	}
}
