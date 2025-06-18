// Do NOT build this file seperately. It is included in dTriList.cpp automatically.


#include "../..\XR_3DA\cl_intersect.h"
#include "dTriCollideK.h"
#include "dSortTriPrimitive.h"
#include "../dCylinder/dCylinder.h"
#include "../MathUtils.h"
#include "../level.h"
#ifdef	DEBUG
#include "../phdebug.h"
#endif

dcTriListCollider::dcTriListCollider(dxGeom* Geometry)
{
	this->Geometry = Geometry;
	GeomData = (dxTriList*)dGeomGetClassData(Geometry);
}

dcTriListCollider::~dcTriListCollider( )
{ }

s32 dCollideBP(const dxGeom* o1, const dxGeom* o2, s32 flags, dContactGeom* contact, s32 skip);	// ODE internal function


#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))
#define SURFACE(Ptr, Stride) ((dSurfaceParameters*) (((byte*)Ptr) + (Stride-sizeof(dSurfaceParameters))))

int dcTriListCollider::CollideBox(dxGeom* Box, s32 Flags, dContactGeom* Contacts, s32 Stride)
{
	fVector3 AABB;
	dVector3 BoxSides;
	dGeomBoxGetLengths(Box, BoxSides);
	dReal* R = const_cast<dReal*>(dGeomGetRotation(Box));
	AABB.x = (dFabs(BoxSides[0] * R[0]) + dFabs(BoxSides[1] * R[1]) + dFabs(BoxSides[2] * R[2])) / 2.0f + 10.0f * EPSILON_3;
	AABB.y = (dFabs(BoxSides[0] * R[4]) + dFabs(BoxSides[1] * R[5]) + dFabs(BoxSides[2] * R[6])) / 2.0f + 10.0f * EPSILON_3;
	AABB.z = (dFabs(BoxSides[0] * R[8]) + dFabs(BoxSides[1] * R[9]) + dFabs(BoxSides[2] * R[10])) / 2.0f + 10.0f * EPSILON_3;
	dBodyID box_body = dGeomGetBody(Box);
	if (box_body)
	{
		const dReal* velocity = dBodyGetLinearVel(box_body);
		AABB.x += dFabs(velocity[0]) * 0.04f;
		AABB.y += dFabs(velocity[1]) * 0.04f;
		AABB.z += dFabs(velocity[2]) * 0.04f;
	}

	BoxTri	bt(*this);
	return dSortTriPrimitiveCollide
	(bt,
	 Box,
	 Geometry,
	 Flags,
	 Contacts,
	 Stride,
	 AABB
	);
}

s32 dcTriListCollider::CollideCylinder(dxGeom* Cylinder, s32 Flags, dContactGeom* Contacts, s32 Stride)
{
	fVector3 AABB;
	dReal CylinderRadius, CylinderLength;


	dGeomCylinderGetParams(Cylinder, &CylinderRadius, &CylinderLength);

	dReal* R = const_cast<dReal*>(dGeomGetRotation(Cylinder));

	AABB.x = REAL(0.5) * dFabs(R[1] * CylinderLength) + (_sqrt(R[0] * R[0] + R[2] * R[2]) * CylinderRadius);

	AABB.y = REAL(0.5) * dFabs(R[5] * CylinderLength) + (_sqrt(R[4] * R[4] + R[6] * R[6]) * CylinderRadius);

	AABB.z = REAL(0.5) * dFabs(R[9] * CylinderLength) + (_sqrt(R[8] * R[8] + R[10] * R[10]) * CylinderRadius);

	const dReal* velocity = dBodyGetLinearVel(dGeomGetBody(Cylinder));
	AABB.x += dFabs(velocity[0]) * 0.04f;
	AABB.y += dFabs(velocity[1]) * 0.04f;
	AABB.z += dFabs(velocity[2]) * 0.04f;

	CylTri	ct(*this);
	return dSortTriPrimitiveCollide
	(
		ct,
		Cylinder,
		Geometry,
		Flags,
		Contacts,
		Stride,
		AABB
	);
}


////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
s32 dcTriListCollider::CollideSphere(dxGeom* Sphere, s32 Flags, dContactGeom* Contacts, s32 Stride)
{
	const f32 SphereRadius = dGeomSphereGetRadius(Sphere);
	fVector3 AABB;


	// Make AABB 
	AABB.x = SphereRadius;
	AABB.y = SphereRadius;
	AABB.z = SphereRadius;

	const dReal* velocity = dBodyGetLinearVel(dGeomGetBody(Sphere));
	AABB.x += dFabs(velocity[0]) * 0.04f;
	AABB.y += dFabs(velocity[1]) * 0.04f;
	AABB.z += dFabs(velocity[2]) * 0.04f;
	SphereTri	st(*this);
	return dSortTriPrimitiveCollide(st, Sphere, Geometry, Flags, Contacts, Stride, AABB);
}
