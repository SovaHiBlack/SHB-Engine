////////////////////////////////////////////////////////////////////////////
//	Module 		: ScriptZone.cpp
//	Description : Script zone object
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "ScriptZone.h"
#include "script_game_object.h"
#include "xrserver_objects_alife_monsters.h"
#include "..\XR_3DA\xr_collide_form.h"
#include "script_callback_ex.h"
#include "GameObject_space.h"

#ifdef DEBUG
#	include "level.h"
#	include "DebugRenderer.h"
#endif

#include "SmartZone.h"

CScriptZone::CScriptZone( )
{ }

CScriptZone::~CScriptZone( )
{ }

void CScriptZone::reinit( )
{
	inherited::reinit( );
}

BOOL CScriptZone::net_Spawn(CSE_Abstract* DC)
{
	feel_touch.clear( );

	if (!inherited::net_Spawn(DC))
	{
		return FALSE;
	}

	return TRUE;
}

void CScriptZone::net_Destroy( )
{
	inherited::net_Destroy( );
}

void CScriptZone::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	const fSphere& s = CFORM( )->getSphere( );
	fVector3 P;
	XFORM( ).transform_tiny(P, s.P);
	feel_touch_update(P, s.R);
}

void CScriptZone::feel_touch_new(CObject* tpObject)
{
	CGameObject* l_tpGameObject = smart_cast<CGameObject*>(tpObject);
	if (!l_tpGameObject)
	{
		return;
	}

	callback(GameObject::eZoneEnter)(lua_game_object( ), l_tpGameObject->lua_game_object( ));
}

void CScriptZone::feel_touch_delete(CObject* tpObject)
{
	CGameObject* l_tpGameObject = smart_cast<CGameObject*>(tpObject);

	if (!l_tpGameObject || l_tpGameObject->getDestroy( ))
	{
		return;
	}

	callback(GameObject::eZoneExit)(lua_game_object( ), l_tpGameObject->lua_game_object( ));
}

void CScriptZone::net_Relcase(CObject* O)
{
	CGameObject* l_tpGameObject = smart_cast<CGameObject*>(O);
	if (!l_tpGameObject)
	{
		return;
	}

	xr_vector<CObject*>::iterator	I = std::find(feel_touch.begin( ), feel_touch.end( ), O);
	if (I != feel_touch.end( ))
	{
		callback(GameObject::eZoneExit)(lua_game_object( ), l_tpGameObject->lua_game_object( ));
	}
}

BOOL CScriptZone::feel_touch_contact(CObject* O)
{
	return (((CCF_Shape*)CFORM( ))->Contact(O));
}

#ifdef DEBUG
void CScriptZone::OnRender( )
{
	if (!bDebug)
	{
		return;
	}

	RCache.OnFrameEnd( );
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

bool CScriptZone::active_contact(u16 id) const
{
	xr_vector<CObject*>::const_iterator	I = feel_touch.begin( );
	xr_vector<CObject*>::const_iterator	E = feel_touch.end( );
	for (; I != E; ++I)
	{
		if ((*I)->ID( ) == id)
		{
			return true;
		}
	}

	return false;
}

using namespace luabind;

#pragma optimize("s",on)
void CScriptZone::script_register(lua_State* L)
{
	module(L)
		[
			class_<CScriptZone, DLL_Pure>("ce_script_zone")
				.def(constructor<>( ))
		];
}

void CSmartZone::script_register(lua_State* L)
{
	module(L)
		[
			class_<CSmartZone, DLL_Pure>("ce_smart_zone")
				.def(constructor<>( ))
		];
}
