////////////////////////////////////////////////////////////////////////////
//	Module 		: SpaceRestrictor.cpp
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "SpaceRestrictor.h"
#include "xrServer_Objects_ALife.h"
#include "level.h"
#include "space_restriction_manager.h"
#include "restriction_space.h"
#include "AISpace.h"
#include "CustomZone.h"

#ifdef DEBUG
#	include "DebugRenderer.h"
#	include "hudmanager.h"
#endif

CSpaceRestrictor::~CSpaceRestrictor( )
{ }

void CSpaceRestrictor::Center(fVector3& C) const
{
	XFORM( ).transform_tiny(C, CFORM( )->getSphere( ).P);
}

f32 CSpaceRestrictor::Radius( ) const
{
	return CFORM( )->getRadius( );
}

BOOL CSpaceRestrictor::net_Spawn(CSE_Abstract* data)
{
	actual(false);

	CSE_Abstract* abstract = (CSE_Abstract*)data;
	CSE_ALifeSpaceRestrictor* se_shape = smart_cast<CSE_ALifeSpaceRestrictor*>(abstract);
	R_ASSERT(se_shape);

	m_space_restrictor_type = se_shape->m_space_restrictor_type;

	CCF_Shape* shape = xr_new<CCF_Shape>(this);
	collidable.model = shape;

	for (u32 i = 0; i < se_shape->shapes.size( ); ++i)
	{
		CShapeData::shape_def& S = se_shape->shapes[i];
		switch (S.type)
		{
			case 0:
			{
				shape->add_sphere(S.data.sphere);
			}
			break;
			case 1:
			{
				shape->add_box(S.data.box);
			}
			break;
		}
	}

	shape->ComputeBounds( );

	BOOL result = inherited::net_Spawn(data);

	if (!result)
	{
		return FALSE;
	}

	setEnabled(FALSE);
	setVisible(FALSE);

	if (!ai( ).get_level_graph( ) || (RestrictionSpace::ERestrictorTypes(se_shape->m_space_restrictor_type) == RestrictionSpace::eRestrictorTypeNone))
	{
		return TRUE;
	}

	Level( ).space_restriction_manager( ).register_restrictor(this, RestrictionSpace::ERestrictorTypes(se_shape->m_space_restrictor_type));

	return TRUE;
}

void CSpaceRestrictor::net_Destroy( )
{
	inherited::net_Destroy( );

	if (!ai( ).get_level_graph( ))
	{
		return;
	}

	if (RestrictionSpace::ERestrictorTypes(m_space_restrictor_type) == RestrictionSpace::eRestrictorTypeNone)
	{
		return;
	}

	Level( ).space_restriction_manager( ).unregister_restrictor(this);
}

bool CSpaceRestrictor::inside(const fSphere& sphere) const
{
	if (!actual( ))
	{
		prepare( );
	}

	if (!m_selfbounds.intersect(sphere))
	{
		return false;
	}

	return prepared_inside(sphere);
}

BOOL CSpaceRestrictor::UsedAI_Locations( )
{
	return FALSE;
}

void CSpaceRestrictor::spatial_move( )
{
	inherited::spatial_move( );
	actual(false);
}

void CSpaceRestrictor::prepare( ) const
{
	Center(m_selfbounds.P);
	m_selfbounds.R = Radius( );

	m_spheres.resize(0);
	m_boxes.resize(0);

	const CCF_Shape* shape = (const CCF_Shape*)collidable.model;

	typedef xr_vector<CCF_Shape::shape_def> SHAPES;

	SHAPES::const_iterator I = shape->shapes.begin( );
	SHAPES::const_iterator E = shape->shapes.end( );
	for (; I != E; ++I)
	{
		switch ((*I).type)
		{
			case 0:
			{	// sphere
				fSphere temp;
				const fSphere& sphere = (*I).data.sphere;
				XFORM( ).transform_tiny(temp.P, sphere.P);
				temp.R = sphere.R;
				m_spheres.push_back(temp);
			}
			break;
			case 1:
			{	// box
				fMatrix4x4				sphere;
				const fMatrix4x4& box = (*I).data.box;
				sphere.mul_43(XFORM( ), box);

				// Build points
				fVector3 A;
				fVector3 B[8];
				CPlanes temp;
				A.set(-0.5f, -0.5f, -0.5f);
				sphere.transform_tiny(B[0], A);
				A.set(-0.5f, -0.5f, +0.5f);
				sphere.transform_tiny(B[1], A);
				A.set(-0.5f, +0.5f, +0.5f);
				sphere.transform_tiny(B[2], A);
				A.set(-0.5f, +0.5f, -0.5f);
				sphere.transform_tiny(B[3], A);
				A.set(+0.5f, +0.5f, +0.5f);
				sphere.transform_tiny(B[4], A);
				A.set(+0.5f, +0.5f, -0.5f);
				sphere.transform_tiny(B[5], A);
				A.set(+0.5f, -0.5f, +0.5f);
				sphere.transform_tiny(B[6], A);
				A.set(+0.5f, -0.5f, -0.5f);
				sphere.transform_tiny(B[7], A);

				temp.m_planes[0].build(B[0], B[3], B[5]);
				temp.m_planes[1].build(B[1], B[2], B[3]);
				temp.m_planes[2].build(B[6], B[5], B[4]);
				temp.m_planes[3].build(B[4], B[2], B[1]);
				temp.m_planes[4].build(B[3], B[2], B[4]);
				temp.m_planes[5].build(B[1], B[0], B[6]);

				m_boxes.push_back(temp);
			}
			break;
			default:
			{
				NODEFAULT;
			}
		}
	}

	actual(true);
}

bool CSpaceRestrictor::prepared_inside(const fSphere& sphere) const
{
	VERIFY(actual( ));

	{
		SPHERES::const_iterator I = m_spheres.begin( );
		SPHERES::const_iterator E = m_spheres.end( );
		for (; I != E; ++I)
		{
			if (sphere.intersect(*I))
			{
				return true;
			}
		}
	}

	{
		BOXES::const_iterator		I = m_boxes.begin( );
		BOXES::const_iterator		E = m_boxes.end( );
		for (; I != E; ++I)
		{
			for (u32 i = 0; i < PLANE_COUNT; ++i)
			{
				if ((*I).m_planes[i].classify(sphere.P) > sphere.R)
				{
					goto continue_loop;
				}
			}

			return true;
continue_loop:
			continue;
		}
	}

	return false;
}

#ifdef DEBUG
extern	flags32	dbg_net_Draw_Flags;

void CSpaceRestrictor::OnRender( )
{
	if (!bDebug)
	{
		return;
	}

	if (!(dbg_net_Draw_Flags.is_any((1 << 2))))
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

	u32 Color = 0;
	CCustomZone* custom_zone = smart_cast<CCustomZone*>(this);
	if (custom_zone && custom_zone->IsEnabled( ))
	{
		Color = D3DCOLOR_XRGB(0, 255, 255);
	}
	else
	{
		Color = D3DCOLOR_XRGB(255, 0, 0);
	}

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

				Level( ).debug_renderer( ).draw_ellipse(l_ball, Color);
			}
			break;
			case 1:
			{
				l_box.mul(XFORM( ), l_pShape->data.box);
				Level( ).debug_renderer( ).draw_obb(l_box, l_half, Color);
			}
			break;
		}
	}

	if (Device.vCameraPosition.distance_to(XFORM( ).c) < 100.0f)
	{
		fMatrix4x4 res;
		res.mul(Device.mFullTransform, XFORM( ));

		fVector4 v_res;

		f32 delta_height = 0.0f;

		// get up on 2 meters
		fVector3 shift;
		static f32 gx = 0.0f;
		static f32 gy = 2.0f;
		static f32 gz = 0.0f;
		shift.set(gx, gy, gz);
		res.transform(v_res, shift);

		// check if the object in sight
		if (v_res.z < 0.0f || v_res.w < 0.0f)
		{
			return;
		}

		if (v_res.x < -1.0f || v_res.x > 1.0f || v_res.y < -1.0f || v_res.y > 1.0f)
		{
			return;
		}

		// get real (x,y)
		f32 x = (1.0f + v_res.x) / 2.0f * (Device.dwWidth);
		f32 y = (1.0f - v_res.y) / 2.0f * (Device.dwHeight) - delta_height;

		HUD( ).Font( ).pFontMedium->SetColor(0xffff0000);
		HUD( ).Font( ).pFontMedium->OutSet(x, y -= delta_height);
		HUD( ).Font( ).pFontMedium->OutNext(Name( ));
		CCustomZone* z = smart_cast<CCustomZone*>(this);
		if (z)
		{
			string64 str;
			switch (z->ZoneState( ))
			{
				case CCustomZone::eZoneStateIdle:
				{
					strcpy(str, "IDLE");
				}
				break;
				case CCustomZone::eZoneStateAwaking:
				{
					strcpy(str, "AWAKING");
				}
				break;
				case CCustomZone::eZoneStateBlowout:
				{
					strcpy(str, "BLOWOUT");
				}
				break;
				case CCustomZone::eZoneStateAccumulate:
				{
					strcpy(str, "ACCUMULATE");
				}
				break;
				case CCustomZone::eZoneStateDisabled:
				{
					strcpy(str, "DISABLED");
				}
				break;
			}

			HUD( ).Font( ).pFontMedium->OutNext(str);
		}
	}
}
#endif

using namespace luabind;

#pragma optimize("s",on)
void CSpaceRestrictor::script_register(lua_State* L)
{
	module(L)
		[
			class_<CSpaceRestrictor, CGameObject>("CSpaceRestrictor")
				.def(constructor<>( ))
		];
}
