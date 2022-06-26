#pragma once

#include "../dCylinder/dCylinder.h"
#include "TriPrimitiveCollideClassDef.h"
#include "dcTriListCollider.h"
struct dxCylinder {	// cylinder
	dReal radius,lz;	// radius, length along z axis */
};



IC f32 dcTriListCollider::	dCylProj(dxGeom* cylinder,const dReal* normal)
{
	VERIFY(dGeomGetClass(cylinder)== dCylinderClassUser);
	f32 hlz;
	f32 radius;
	dGeomCylinderGetParams(cylinder,&radius,&hlz);
	const dReal* R=dGeomGetRotation(cylinder);
	hlz*=0.5f;
	f32 cos1=dFabs(dDOT14(normal,R+1));
	cos1=cos1<REAL(1.) ? cos1 : REAL(1.); //cos1 may slightly exeed 1.f
	f32 sin1=_sqrt(REAL(1.)-cos1*cos1);
	//////////////////////////////
	return cos1*hlz+sin1*radius;
}
