#include "stdafx.h"
#include "climableobject.h "
#include "PHStaticGeomShell.h"
#include "xrServer_Objects_ALife.h"
#include "PHCharacter.h"
#include "MathUtils.h"

#ifdef DEBUG
#	include "DebugRenderer.h"
#	include "level.h"
#	include "PHDebug.h"
#endif

static const f32 down_leader_extension_tolerance = 0.2f;
static const f32 up_leader_extension_tolerance = 0.0f;

IC void OrientToNorm(const fVector3& normal, fMatrix4x4& form, fObb& box)
{
	fVector3* ax_pointer = (fVector3*)&form;
	f32* s_pointer = (f32*)&(box.m_halfsize);
	f32 max_dot = abs(ax_pointer[0].dotproduct(normal));
	f32 min_size = box.m_halfsize.x;
	s32 max_ax_i = 0, min_size_i = 0;
	for (s32 i = 1; 3 > i; ++i)
	{
		f32 dot_pr = abs(ax_pointer[i].dotproduct(normal));
		if (max_dot < dot_pr)
		{
			max_ax_i = i; max_dot = dot_pr;
		}
		if (min_size > s_pointer[i])
		{
			min_size_i = i; min_size = s_pointer[i];
		}
	}
	VERIFY(min_size_i == max_ax_i);
	if (ax_pointer[max_ax_i].dotproduct(normal) < 0.f)
	{
		ax_pointer[max_ax_i].invert( );
		ax_pointer[(max_ax_i + 1) % 3].invert( );
	}
}

class CPHLeaderGeomShell : public CPHStaticGeomShell
{
	CClimableObject* m_pClimable;
public:
	CPHLeaderGeomShell(CClimableObject* climable);
	void					near_callback(CPHObject* obj);
};

CPHLeaderGeomShell::CPHLeaderGeomShell(CClimableObject* climable)
{
	m_pClimable = climable;
}
void CPHLeaderGeomShell::near_callback(CPHObject* obj)
{
	if (obj && obj->CastType( ) == CPHObject::tpCharacter)
	{
		CPHCharacter* ch = static_cast<CPHCharacter*>(obj);
		ch->SetElevator(m_pClimable);
	}
}

CClimableObject::CClimableObject( )
{
	m_pStaticShell = NULL;
}
CClimableObject::~CClimableObject( )
{

}
void CClimableObject::Load(pcstr section)
{
	inherited::Load(section);
}
BOOL CClimableObject::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract* e = (CSE_Abstract*)(DC);
	CSE_ALifeObjectClimable* CLB = smart_cast<CSE_ALifeObjectClimable*>(e);
	const fMatrix4x4& b = CLB->shapes[0].data.box;
	m_box.m_halfsize.set(b._11, b._22, b._33);
	m_radius = _max(_max(m_box.m_halfsize.x, m_box.m_halfsize.y), m_box.m_halfsize.z);

	//m_box.m_halfsize.set(1.f,1.f,1.f);
	BOOL ret = inherited::net_Spawn(DC);
	const f32 f_min_width = 0.2f;
	fVector3 shift;
	shift.set(0.0f, 0.0f, 0.0f);
	SORT(b._11, m_axis.set(XFORM( ).i); m_axis.mul(m_box.m_halfsize.x), m_side.set(XFORM( ).i); m_side.mul(m_box.m_halfsize.x), m_norm.set(XFORM( ).i); if (m_box.m_halfsize.x < f_min_width)
	{
		m_box.m_halfsize.x = f_min_width; shift.set(1.f, 0.f, 0.f);
	}; m_norm.mul(m_box.m_halfsize.x),
		b._22, m_axis.set(XFORM( ).j); m_axis.mul(m_box.m_halfsize.y), m_side.set(XFORM( ).j); m_side.mul(m_box.m_halfsize.y), m_norm.set(XFORM( ).j); if (m_box.m_halfsize.y < f_min_width)
	{
		m_box.m_halfsize.y = f_min_width; shift.set(0.f, 1.f, 0.f);
	}; m_norm.mul(m_box.m_halfsize.y),
		b._33, m_axis.set(XFORM( ).k); m_axis.mul(m_box.m_halfsize.z), m_side.set(XFORM( ).k); m_side.mul(m_box.m_halfsize.z), m_norm.set(XFORM( ).k); if (m_box.m_halfsize.z < f_min_width)
	{
		m_box.m_halfsize.z = f_min_width; shift.set(0.f, 0.f, 1.f);
	}; m_norm.mul(m_box.m_halfsize.z)
		);
	shift.mul(f_min_width);

	XFORM( ).transform_dir(shift);
	CObject::Position( ).sub(shift);
	m_box.xform_set(Fidentity);
	m_pStaticShell = xr_new<CPHLeaderGeomShell>(this);
	P_BuildStaticGeomShell(smart_cast<CPHStaticGeomShell*>(m_pStaticShell), smart_cast<CGameObject*>(this), 0, m_box);
	m_pStaticShell->SetMaterial("materials\\fake_ladders");

	if (m_axis.y < 0.f)
	{
		m_axis.invert( );
		m_side.invert( );

	}
//	shedule_unregister();
	processing_deactivate( );
	m_pStaticShell->set_ObjectContactCallback(ObjectContactCallback);
	return ret;
}
void CClimableObject::net_Destroy( )
{
	inherited::net_Destroy( );
	m_pStaticShell->Deactivate( );
	xr_delete(m_pStaticShell);
}
void CClimableObject::shedule_Update(u32 dt)							// Called by shedule
{
	inherited::shedule_Update(dt);
}
void CClimableObject::UpdateCL( )								// Called each frame, so no need for d
{
	inherited::UpdateCL( );
}

void	CClimableObject::Center(fVector3& C) const
{
	C.set(XFORM( ).c);
}
f32	CClimableObject::Radius( ) const
{
	return							m_radius;
}

f32		CClimableObject::DDLowerP(CPHCharacter* actor, fVector3& out_dir)const
{
	VERIFY(actor);
	fVector3 pos;
	LowerPoint(out_dir);
	actor->GetFootCenter(pos);
	out_dir.sub(pos);
	return to_mag_and_dir(out_dir);
}
f32		CClimableObject::DDUpperP(CPHCharacter* actor, fVector3& out_dir)const
{
	VERIFY(actor);
	fVector3 pos;
	UpperPoint(out_dir);
	actor->GetFootCenter(pos);
	out_dir.sub(pos);
	return to_mag_and_dir(out_dir);
}

void		CClimableObject::DefineClimbState(CPHCharacter* actor)const
{ }
f32		CClimableObject::DDAxis(fVector3& dir)const
{
	dir.set(m_axis);
	return to_mag_and_dir(dir);
}

f32	CClimableObject::DDSide(fVector3& dir)const
{
	dir.set(m_side);
	return to_mag_and_dir(dir);
}
f32	CClimableObject::DDNorm(fVector3& dir)const
{
	dir.set(m_norm);
	return to_mag_and_dir(dir);
}
f32		CClimableObject::DDToAxis(CPHCharacter* actor, fVector3& out_dir)const
{
	VERIFY(actor);
	DToAxis(actor, out_dir);
	return to_mag_and_dir(out_dir);
}

void	CClimableObject::POnAxis(CPHCharacter* actor, fVector3& P)const
{
	VERIFY(actor);
	actor->GetFootCenter(P);
	prg_pos_on_axis(Position( ), m_axis, P);
}
void		CClimableObject::LowerPoint(fVector3& P)const
{
	P.sub(XFORM( ).c, m_axis);
	P.add(m_norm);
}

void		CClimableObject::UpperPoint(fVector3& P)const
{
	P.add(XFORM( ).c, m_axis);
	P.add(m_norm);
}

void		CClimableObject::DToAxis(CPHCharacter* actor, fVector3& dir)const
{
	VERIFY(actor);
	POnAxis(actor, dir);
	fVector3 pos;
	actor->GetFootCenter(pos);
	dir.sub(pos);
}
void CClimableObject::DSideToAxis(CPHCharacter* actor, fVector3& dir)const
{
	VERIFY(actor);
	DToAxis(actor, dir);
	fVector3 side;
	side.set(m_side);
	to_mag_and_dir(side);
	side.mul(side.dotproduct(dir));
	dir.set(side);
}

f32 CClimableObject::DDSideToAxis(CPHCharacter* actor, fVector3& dir)const
{
	VERIFY(actor);
	DToAxis(actor, dir);
	fVector3 side;
	side.set(m_side);
	to_mag_and_dir(side);
	f32 dot = side.dotproduct(dir);
	if (dot > 0.0f)
	{
		dir.set(side);
		return dot;
	}
	else
	{
		dir.set(side);
		dir.invert( );
		return -dot;
	}
}

void CClimableObject::DToPlain(CPHCharacter* actor, fVector3& dist) const
{
	VERIFY(actor);
	DToAxis(actor, dist);
	fVector3 norm;
	norm.set(m_norm);
	to_mag_and_dir(norm);
	f32 dot = norm.dotproduct(dist);
	norm.mul(dot);
	dist.set(norm);
}

f32 CClimableObject::DDToPlain(CPHCharacter* actor, fVector3& dir)const
{
	VERIFY(actor);
	DToPlain(actor, dir);
	return to_mag_and_dir(dir);
}

bool CClimableObject::InTouch(CPHCharacter* actor)const
{
	VERIFY(actor);
	fVector3 dir;
	const f32 normal_tolerance = 0.05f;
	f32 foot_radius = actor->FootRadius( );
	return (DDToPlain(actor, dir) < foot_radius + m_norm.magnitude( ) + normal_tolerance &&
			DDSideToAxis(actor, dir) < m_side.magnitude( )) && InRange(actor);
}

f32 CClimableObject::AxDistToUpperP(CPHCharacter* actor) const
{
	VERIFY(actor);
	fVector3 v1;
	fVector3 v2;
	actor->GetFootCenter(v1);
	UpperPoint(v2);
	v2.sub(v1);
	v1.set(m_axis); to_mag_and_dir(v1);
	return v1.dotproduct(v2);
}

f32 CClimableObject::AxDistToLowerP(CPHCharacter* actor)const
{
	VERIFY(actor);
	fVector3 v1;
	fVector3 v2;
	actor->GetFootCenter(v1);
	LowerPoint(v2);
	v2.sub(v1);
	v1.set(m_axis); to_mag_and_dir(v1);
	return -v1.dotproduct(v2);
}
bool CClimableObject::InRange(CPHCharacter* actor)const
{
	VERIFY(actor);
	return AxDistToLowerP(actor) > -down_leader_extension_tolerance && AxDistToUpperP(actor) + actor->FootRadius( ) > -up_leader_extension_tolerance;
}

bool CClimableObject::BeforeLadder(CPHCharacter* actor, f32 tolerance/*=0.f*/)const
{
	VERIFY(actor);
	fVector3 d;
	DToAxis(actor, d);
	fVector3 n;
	n.set(Norm( ));
	f32 width = to_mag_and_dir(n);
	return d.dotproduct(n) < -(width + actor->FootRadius( ) / 2.f + tolerance);
}

BOOL CClimableObject::UsedAI_Locations( )
{
	return FALSE;
}

void CClimableObject::ObjectContactCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* /*material_1*/, SGameMtl* /*material_2*/)
{
	dxGeomUserData* usr_data_1 = retrieveGeomUserData(c.geom.g1);
	dxGeomUserData* usr_data_2 = retrieveGeomUserData(c.geom.g2);
	dxGeomUserData* usr_data_ch = NULL;
	dxGeomUserData* usr_data_lad = NULL;
	CClimableObject* this_object = NULL;
	CPHCharacter* ch = NULL;
	f32 norm_sign = 0.0f;
	if (bo1)
	{
		usr_data_ch = usr_data_2;
		usr_data_lad = usr_data_1;
		norm_sign = -1.0f;
	}
	else
	{
		norm_sign = 1.0f;
		usr_data_ch = usr_data_1;
		usr_data_lad = usr_data_2;
	}

	if (usr_data_ch && usr_data_ch->ph_object && usr_data_ch->ph_object->CastType( ) == CPHObject::tpCharacter)
	{
		ch = static_cast<CPHCharacter*>(usr_data_ch->ph_object);
	}
	else
	{
		do_colide = false;
		return;
	}

	VERIFY(ch);
	VERIFY(usr_data_lad);
	this_object = static_cast<CClimableObject*>(usr_data_lad->ph_ref_object);
	VERIFY(this_object);
	if (!this_object->BeforeLadder(ch, -0.1f))
	{
		do_colide = false;
	}
}

#ifdef DEBUG
extern	flags32	dbg_net_Draw_Flags;
void CClimableObject::OnRender( )
{
	if (!dbg_net_Draw_Flags.test(1 << 10) && !ph_dbg_draw_mask.test(phDbgLadder)) return;

	fMatrix4x4 form;
	m_box.xform_get(form);
	//form.mulA(XFORM());
	Level( ).debug_renderer( ).draw_obb(XFORM( ), m_box.m_halfsize, D3DCOLOR_XRGB(0, 0, 255));
	fVector3 p1;
	fVector3 p2;
	fVector3 d;
	d.set(m_axis);
	p1.add(XFORM( ).c, d);
	p2.sub(XFORM( ).c, d);
	Level( ).debug_renderer( ).draw_line(Fidentity, p1, p2, D3DCOLOR_XRGB(255, 0, 0));

	d.set(m_side);
	p1.add(XFORM( ).c, d);
	p2.sub(XFORM( ).c, d);
	Level( ).debug_renderer( ).draw_line(Fidentity, p1, p2, D3DCOLOR_XRGB(255, 0, 0));

	d.set(m_norm);
	d.mul(10.f);
	p1.add(XFORM( ).c, d);
	p2.set(XFORM( ).c);
	Level( ).debug_renderer( ).draw_line(Fidentity, p1, p2, D3DCOLOR_XRGB(0, 255, 0));
}
#endif
