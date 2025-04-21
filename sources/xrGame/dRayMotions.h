#pragma once

struct dxRayMotions;
extern s32 dRayMotionsClassUser;

dxGeom* dCreateRayMotions(dSpaceID space);
void dGeomRayMotionsSet(dGeomID g, const dReal* p, const dReal* d, dReal l);
void dGeomRayMotionSetGeom(dGeomID rm, dGeomID g);
