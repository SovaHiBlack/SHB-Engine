#pragma once

#include "../ode_include.h"

struct dxCylinder;
extern s32 dCylinderClassUser;

dxGeom *dCreateCylinder (dSpaceID space, dReal r, dReal lz);
void dGeomCylinderSetParams (dGeomID g, dReal radius, dReal length);
void dGeomCylinderGetParams (dGeomID g, dReal *radius, dReal *length);
