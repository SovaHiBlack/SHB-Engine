#include "stdafx.h"
#include "Geometry.h"
#include "PHDynamicData.h"
#include "ExtendedGeom.h"
#include "dcylinder//dCylinder.h"

static void computeFinalTx(dGeomID geom_transform, dReal* final_pos, dReal* final_R)
{
	R_ASSERT2(dGeomGetClass(geom_transform) == dGeomTransformClass, "is not a geom transform");
	dGeomID obj = dGeomTransformGetGeom(geom_transform);
	const dReal* R = dGeomGetRotation(geom_transform);
	const dReal* pos = dGeomGetPosition(geom_transform);
	dMULTIPLY0_331(final_pos, R, dGeomGetPosition(obj));
	final_pos[0] += pos[0];
	final_pos[1] += pos[1];
	final_pos[2] += pos[2];
	dMULTIPLY0_333(final_R, R, dGeomGetRotation(obj));
}

void GetBoxExtensions(dGeomID box, const dReal* axis, const dReal* pos, const dReal* rot, f32 center_prg, dReal* lo_ext, dReal* hi_ext)
{
	R_ASSERT2(dGeomGetClass(box) == dBoxClass, "is not a box");
	dVector3 length;
	dGeomBoxGetLengths(box, length);
	dReal dif = dDOT(pos, axis) - center_prg;
	dReal ful_ext = dFabs(dDOT14(axis, rot + 0)) * length[0] + dFabs(dDOT14(axis, rot + 1)) * length[1] + dFabs(dDOT14(axis, rot + 2)) * length[2];
	ful_ext /= 2.0f;
	*lo_ext = -ful_ext + dif;
	*hi_ext = ful_ext + dif;
}

void GetCylinderExtensions(dGeomID cyl, const dReal* axis, const dReal* pos, const dReal* rot, f32 center_prg, dReal* lo_ext, dReal* hi_ext)
{
	R_ASSERT2(dGeomGetClass(cyl) == dCylinderClassUser, "is not a cylinder");
	dReal radius;
	dReal length;
	dGeomCylinderGetParams(cyl, &radius, &length);
	dReal dif = dDOT(pos, axis) - center_prg;
	dReal _cos = dFabs(dDOT14(axis, rot + 1));
	dReal cos1 = dDOT14(axis, rot + 0);
	dReal cos3 = dDOT14(axis, rot + 2);
	dReal _sin = _sqrt(cos1 * cos1 + cos3 * cos3);
	length /= 2.0f;
	dReal ful_ext = _cos * length + _sin * radius;
	*lo_ext = -ful_ext + dif;
	*hi_ext = ful_ext + dif;
}

void GetSphereExtensions(dGeomID sphere, const dReal* axis, const dReal* pos, f32 center_prg, dReal* lo_ext, dReal* hi_ext)
{
	R_ASSERT2(dGeomGetClass(sphere) == dSphereClass, "is not a sphere");
	dReal radius = dGeomSphereGetRadius(sphere);
	dReal dif = dDOT(pos, axis) - center_prg;
	*lo_ext = -radius + dif;
	*hi_ext = radius + dif;
}

void TransformedGeometryExtensionLocalParams(dGeomID geom_transform, const dReal* axis, f32 center_prg, dReal* local_axis, dReal& local_center_prg)
{
	R_ASSERT2(dGeomGetClass(geom_transform) == dGeomTransformClass, "is not a geom transform");
	const dReal* rot = dGeomGetRotation(geom_transform);
	const dReal* pos = dGeomGetPosition(geom_transform);
	dVector3 local_pos;

	dMULTIPLY1_331(local_axis, rot, axis);
	dMULTIPLY1_331(local_pos, rot, pos);
	local_center_prg = center_prg - dDOT(local_pos, local_axis);
}

CODEGeom::CODEGeom( )
{
	m_geom_transform = NULL;
	m_bone_id = u16(-1);
}

CODEGeom::~CODEGeom( )
{
	if (m_geom_transform)
	{
		destroy( );
	}
}

void CODEGeom::get_mass(dMass& m, const fVector3& ref_point, f32 density)
{
	get_mass(m);
	dMassAdjust(&m, density * volume( ));
	fVector3 l;
	l.sub(local_center( ), ref_point);
	dMassTranslate(&m, l.x, l.y, l.z);
}

void CODEGeom::get_mass(dMass& m, const fVector3& ref_point)
{
	get_mass(m);
	fVector3 l;
	l.sub(local_center( ), ref_point);
	dMassTranslate(&m, l.x, l.y, l.z);
}

void CODEGeom::add_self_mass(dMass& m, const fVector3& ref_point, f32 density)
{
	dMass self_mass;
	get_mass(self_mass, ref_point, density);
	dMassAdd(&m, &self_mass);
}

void CODEGeom::add_self_mass(dMass& m, const fVector3& ref_point)
{
	dMass self_mass;
	get_mass(self_mass, ref_point);
	dMassAdd(&m, &self_mass);
}

void CODEGeom::get_local_center_bt(fVector3& center)
{
	if (!m_geom_transform)
	{
		return;
	}

	if (!geom( ))		//geom is not transformed
	{
		center.set(0.0f, 0.0f, 0.0f);
	}

	center.set(*((const fVector3*)dGeomGetPosition(geom( ))));
}

void CODEGeom::get_local_form_bt(fMatrix4x4& form)
{
	PHDynamicData::DMXPStoFMX(dGeomGetRotation(geom( )), dGeomGetPosition(geom( )), form);
}

void CODEGeom::get_global_center_bt(fVector3& center)
{
	center.set(*((const fVector3*)dGeomGetPosition(m_geom_transform)));
	dVector3 add;
	dMULTIPLY0_331(add, dGeomGetRotation(m_geom_transform), dGeomGetPosition(geom( )));
	center.x += add[0];
	center.y += add[1];
	center.z += add[2];
}

void CODEGeom::get_global_form_bt(fMatrix4x4& form)
{
	dMULTIPLY0_331((dReal*)(&form.c), dGeomGetRotation(m_geom_transform), dGeomGetPosition(geom( )));
	form.c.add(*((const fVector3*)dGeomGetPosition(m_geom_transform)));
	dMULTIPLY3_333((dReal*)(&form), dGeomGetRotation(m_geom_transform), dGeomGetRotation(geom( )));
	//PHDynamicData::DMXtoFMX((dReal*)(&form),form);
}

void CODEGeom::set_static_ref_form(const fMatrix4x4& form)
{
	dGeomSetPosition(geometry_transform( ), form.c.x, form.c.y, form.c.z);
	fMatrix3x3 m33;
	m33.set(form);
	dMatrix3 R;
	PHDynamicData::FMX33toDMX(m33, R);
	dGeomSetRotation(geometry_transform( ), R);
}

void CODEGeom::set_position(const fVector3& /*ref_point*/)
{
	dGeomUserDataResetLastPos(geom( ));
}

void CODEGeom::set_body(dBodyID body)
{
	if (m_geom_transform)
	{
		dGeomSetBody(m_geom_transform, body);
	}
}

void CODEGeom::add_to_space(dSpaceID space)
{
	if (m_geom_transform)
	{
		dSpaceAdd(space, m_geom_transform);
	}
}

void CODEGeom::remove_from_space(dSpaceID space)
{
	if (m_geom_transform)
	{
		dSpaceRemove(space, m_geom_transform);
	}
}

void CODEGeom::clear_cashed_tries( )
{
	if (!m_geom_transform)
	{
		return;
	}

	dGeomID g = geom( );
	if (g)
	{
		VERIFY(dGeomGetUserData(g));
		dGeomUserDataClearCashedTries(g);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataClearCashedTries(m_geom_transform);
	}
}

void CODEGeom::set_material(u16 ul_material)
{
	if (!m_geom_transform)
	{
		return;
	}

	if (geom( ))
	{
		VERIFY(dGeomGetUserData(geom( )));
		dGeomGetUserData(geom( ))->material = ul_material;
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomGetUserData(m_geom_transform)->material = ul_material;
	}
}

void CODEGeom::set_contact_cb(ContactCallbackFun* ccb)
{
	if (!m_geom_transform)
	{
		return;
	}

	if (geom( ))
	{
		VERIFY(dGeomGetUserData(geom( )));
		dGeomUserDataSetContactCallback(geom( ), ccb);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataSetContactCallback(m_geom_transform, ccb);
	}
}

void CODEGeom::set_obj_contact_cb(ObjectContactCallbackFun* occb)
{
	if (!m_geom_transform)
	{
		return;
	}

	if (geom( ))
	{
		VERIFY(dGeomGetUserData(geom( )));
		dGeomUserDataSetObjectContactCallback(geom( ), occb);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataSetObjectContactCallback(m_geom_transform, occb);
	}
}

void CODEGeom::add_obj_contact_cb(ObjectContactCallbackFun* occb)
{
	if (!m_geom_transform)
	{
		return;
	}

	if (geom( ))
	{
		VERIFY(dGeomGetUserData(geom( )));
		dGeomUserDataAddObjectContactCallback(geom( ), occb);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataAddObjectContactCallback(m_geom_transform, occb);
	}
}

void CODEGeom::remove_obj_contact_cb(ObjectContactCallbackFun* occb)
{
	if (!m_geom_transform)
	{
		return;
	}

	if (geom( ))
	{
		VERIFY(dGeomGetUserData(geom( )));
		dGeomUserDataRemoveObjectContactCallback(geom( ), occb);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataRemoveObjectContactCallback(m_geom_transform, occb);
	}
}

void CODEGeom::set_callback_data(pvoid cd)
{
	if (!m_geom_transform)
	{
		return;
	}

	if (geom( ))
	{
		VERIFY(dGeomGetUserData(geom( )));
		dGeomUserDataSetCallbackData(geom( ), cd);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataSetCallbackData(m_geom_transform, cd);
	}
}

pvoid CODEGeom::get_callback_data( )
{
	if (!m_geom_transform)
	{
		return NULL;
	}

	if (geom( ))
	{
		VERIFY(dGeomGetUserData(geom( )));
		return dGeomGetUserData(geom( ))->callback_data;
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		return dGeomGetUserData(m_geom_transform)->callback_data;
	}
}

void CODEGeom::set_ref_object(CPhysicsShellHolder* ro)
{
	if (!m_geom_transform)
	{
		return;
	}

	if (geom( ))
	{
		VERIFY(dGeomGetUserData(geom( )));
		dGeomUserDataSetPhysicsRefObject(geom( ), ro);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataSetPhysicsRefObject(m_geom_transform, ro);
	}
}

void CODEGeom::set_ph_object(CPHObject* o)
{
	if (!m_geom_transform)
	{
		return;
	}

	if (geom( ))
	{
		VERIFY(dGeomGetUserData(geom( )));
		dGeomGetUserData(geom( ))->ph_object = o;
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomGetUserData(m_geom_transform)->ph_object = o;
	}
}

void CODEGeom::move_local_basis(const fMatrix4x4& inv_new_mul_old)
{
	fMatrix4x4 new_form;
	get_local_form(new_form);
	new_form.mulA_43(inv_new_mul_old);
	set_local_form(new_form);
}

void CODEGeom::build(const fVector3& ref_point)
{
	init( );
	set_position(ref_point);
}

void CODEGeom::init( )
{
	dGeomID geom = create( );
	m_geom_transform = dCreateGeomTransform(0);
	dGeomTransformSetCleanup(m_geom_transform, 0);
	dGeomSetData(m_geom_transform, 0);
	dGeomTransformSetGeom(m_geom_transform, geom);
	dGeomTransformSetInfo(m_geom_transform, 1);
	dGeomCreateUserData(geom);
	dGeomUserDataSetBoneId(geom, m_bone_id);
}

void CODEGeom::destroy( )
{
	if (!m_geom_transform)
	{
		return;
	}

	if (geom( ))
	{
		dGeomDestroyUserData(geom( ));
		dGeomDestroy(geom( ));
		dGeomTransformSetGeom(m_geom_transform, 0);
	}

	dGeomDestroyUserData(m_geom_transform);
	dGeomDestroy(m_geom_transform);
	m_geom_transform = NULL;
}

CBoxGeom::CBoxGeom(const fObb& box)
{
	m_box = box;
}

void CBoxGeom::get_mass(dMass& m)
{
	fVector3& hside = m_box.m_halfsize;
	dMassSetBox(&m, 1.0f, hside.x * 2.0f, hside.y * 2.0f, hside.z * 2.0f);
	dMatrix3 DMatx;
	PHDynamicData::FMX33toDMX(m_box.m_rotate, DMatx);
	dMassRotate(&m, DMatx);
}

f32 CBoxGeom::volume( )
{
	return m_box.m_halfsize.x * m_box.m_halfsize.y * m_box.m_halfsize.z * 8.f;
}

f32 CBoxGeom::radius( )
{
	return m_box.m_halfsize.x;
}

void CODEGeom::get_final_tx_bt(const dReal*& p, const dReal*& R, dReal* bufV, dReal* bufM)
{
	VERIFY(m_geom_transform);
	//dGeomID		g		=	geometry_bt()						;
	get_final_tx(m_geom_transform, p, R, bufV, bufM);
}

void CODEGeom::get_final_tx(dGeomID g, const dReal*& p, const dReal*& R, dReal* bufV, dReal* bufM)
{
	if (is_transform(g))
	{
		computeFinalTx(g, bufV, bufM);
		R = bufM; p = bufV;
	}
	else
	{
		R = dGeomGetRotation(g);
		p = dGeomGetPosition(g);
	}
}
void CBoxGeom::get_extensions_bt(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext)
{
	VERIFY(m_geom_transform);
	const dReal* rot = NULL;
	const dReal* pos = NULL;
	dVector3 p;
	dMatrix3 r;
	dGeomID g = geometry_bt( );
	get_final_tx_bt(pos, rot, p, r);
	GetBoxExtensions(g, cast_fp(axis), pos, rot, center_prg, &lo_ext, &hi_ext);
}

void CBoxGeom::get_max_area_dir_bt(fVector3& dir)
{
	dVector3 length;
	dVector3 ddir;
	dGeomBoxGetLengths(geometry( ), length);
	dReal S1 = length[0] * length[1];
	dReal S2 = length[0] * length[2];
	dReal S3 = length[1] * length[2];
	const dReal* R = dGeomGetRotation(geometry( ));
	if (S1 > S2)
	{
		if (S1 > S3)
		{	//S1
			ddir[0] = R[2];
			ddir[1] = R[6];
			ddir[2] = R[10];
		}
		else
		{	//S3
			ddir[0] = R[0];
			ddir[1] = R[4];
			ddir[2] = R[8];
		}
	}
	else
	{
		if (S2 > S3)
		{	//S2
			ddir[0] = R[1];
			ddir[1] = R[5];
			ddir[2] = R[9];
		}
		else
		{	//S3
			ddir[0] = R[0];
			ddir[1] = R[4];
			ddir[2] = R[8];
		}
	}

	if (geom( ))
	{
		const dReal* TR = dGeomGetRotation(geometry_transform( ));
		dir.x = dDOT(ddir, TR);
		dir.y = dDOT(ddir, TR + 4);
		dir.z = dDOT(ddir, TR + 8);
	}
	else
	{
		dir.x = ddir[0];
		dir.y = ddir[1];
		dir.z = ddir[2];
	}
}

const fVector3& CBoxGeom::local_center( )
{
	return m_box.m_translate;
}

void CBoxGeom::get_local_form(fMatrix4x4& form)
{
	form._14 = 0;
	form._24 = 0;
	form._34 = 0;
	form._44 = 1;
	form.i.set(m_box.m_rotate.i);
	form.j.set(m_box.m_rotate.j);
	form.k.set(m_box.m_rotate.k);
	form.c.set(m_box.m_translate);
}

void CBoxGeom::set_local_form(const fMatrix4x4& form)
{
	m_box.m_rotate.i.set(form.i);
	m_box.m_rotate.j.set(form.j);
	m_box.m_rotate.k.set(form.k);
	m_box.m_translate.set(form.c);
}

dGeomID CBoxGeom::create( )
{
	return dCreateBox(0, m_box.m_halfsize.x * 2.0f, m_box.m_halfsize.y * 2.0f, m_box.m_halfsize.z * 2.0f);
}

void CBoxGeom::set_position(const fVector3& ref_point)
{
	inherited::set_position(ref_point);
	dVector3 local_position = {m_box.m_translate.x - ref_point.x, m_box.m_translate.y - ref_point.y, m_box.m_translate.z - ref_point.z};
	dGeomSetPosition(geom( ), local_position[0], local_position[1], local_position[2]);
	dMatrix3 R;
	PHDynamicData::FMX33toDMX(m_box.m_rotate, R);
	dGeomSetRotation(geom( ), R);
}

CSphereGeom::CSphereGeom(const fSphere& sphere)
{
	m_sphere = sphere;
}

void CSphereGeom::get_mass(dMass& m)
{
	dMassSetSphere(&m, 1.0f, m_sphere.R);
}

f32 CSphereGeom::volume( )
{
	return (4.0f * M_PI * m_sphere.R * m_sphere.R * m_sphere.R / 3.0f);
}

f32 CSphereGeom::radius( )
{
	return m_sphere.R;
}

void CSphereGeom::get_extensions_bt(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext)
{
	VERIFY(m_geom_transform);
	const dReal* rot = NULL;
	const dReal* pos = NULL;
	dVector3 p;
	dMatrix3 r;
	dGeomID g = geometry_bt( );
	get_final_tx_bt(pos, rot, p, r);
	GetSphereExtensions(g, cast_fp(axis), pos, center_prg, &lo_ext, &hi_ext);
}

const fVector3& CSphereGeom::local_center( )
{
	return m_sphere.P;
}

void CSphereGeom::get_local_form(fMatrix4x4& form)
{
	form.identity( );
	form.c.set(m_sphere.P);
}

void CSphereGeom::set_local_form(const fMatrix4x4& form)
{
	m_sphere.P.set(form.c);
}

dGeomID CSphereGeom::create( )
{
	return dCreateSphere(0, m_sphere.R);
}

void CSphereGeom::set_position(const fVector3& ref_point)
{
	inherited::set_position(ref_point);
	dVector3 local_position = {m_sphere.P.x - ref_point.x, m_sphere.P.y - ref_point.y, m_sphere.P.z - ref_point.z};

	dGeomSetPosition(geom( ), local_position[0], local_position[1], local_position[2]);
}

CCylinderGeom::CCylinderGeom(const fCylinder& cyl)
{
	m_cylinder = cyl;
}

void CCylinderGeom::get_mass(dMass& m)
{
	dMassSetCylinder(&m, 1.0f, 2, m_cylinder.m_radius, m_cylinder.m_height);
	dMatrix3 DMatx;
	fMatrix3x3 m33;
	m33.j.set(m_cylinder.m_direction);
	fVector3::generate_orthonormal_basis(m33.j, m33.k, m33.i);
	PHDynamicData::FMX33toDMX(m33, DMatx);
	dMassRotate(&m, DMatx);
}

f32 CCylinderGeom::volume( )
{
	return M_PI * m_cylinder.m_radius * m_cylinder.m_radius * m_cylinder.m_height;
}

f32 CCylinderGeom::radius( )
{
	return m_cylinder.m_radius;
}

void CCylinderGeom::get_extensions_bt(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext)
{
	VERIFY(m_geom_transform);
	const dReal* rot = NULL;
	const dReal* pos = NULL;
	dVector3 p;
	dMatrix3 r;
	dGeomID g = geometry_bt( );
	get_final_tx_bt(pos, rot, p, r);
	GetCylinderExtensions(g, cast_fp(axis), pos, rot, center_prg, &lo_ext, &hi_ext);
}

const fVector3& CCylinderGeom::local_center( )
{
	return m_cylinder.m_center;
}

void CCylinderGeom::get_local_form(fMatrix4x4& form)
{
	form._14 = 0;
	form._24 = 0;
	form._34 = 0;
	form._44 = 1;
	form.j.set(m_cylinder.m_direction);
	fVector3::generate_orthonormal_basis(form.j, form.k, form.i);
	form.c.set(m_cylinder.m_center);
}

void CCylinderGeom::set_local_form(const fMatrix4x4& form)
{
	m_cylinder.m_center.set(form.c);
	m_cylinder.m_direction.set(form.j);
}

dGeomID CCylinderGeom::create( )
{
	return dCreateCylinder(0, m_cylinder.m_radius, m_cylinder.m_height);
}

void CCylinderGeom::set_position(const fVector3& ref_point)
{
	inherited::set_position(ref_point);
	dVector3 local_position = {m_cylinder.m_center.x - ref_point.x, m_cylinder.m_center.y - ref_point.y, m_cylinder.m_center.z - ref_point.z};

	dGeomSetPosition(geom( ), local_position[0], local_position[1], local_position[2]);
	dMatrix3 R;
	fMatrix3x3 m33;
	m33.j.set(m_cylinder.m_direction);
	fVector3::generate_orthonormal_basis(m33.j, m33.k, m33.i);
	PHDynamicData::FMX33toDMX(m33, R);
	dGeomSetRotation(geom( ), R);
}
