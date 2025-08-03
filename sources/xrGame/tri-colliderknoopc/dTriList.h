#pragma once

#include "ode_include.h"


/* Class ID */

extern s32 dTriListClass;



/* Single precision, no padding vector3 used for storage */

struct dcVector3
{
	f32 x;
	f32 y;
	f32 z;
};



/* Per triangle callback */

typedef s32 dTriCallback(dGeomID TriList, dGeomID RefObject, s32 TriangleIndex);

void dGeomTriListSetCallback(dGeomID g, dTriCallback* Callback);

dTriCallback* dGeomTriListGetCallback(dGeomID g);



/* Per object callback */

typedef void dTriArrayCallback(dGeomID TriList, dGeomID RefObject, const s32* TriIndices, s32 TriCount);

void dGeomTriListSetArrayCallback(dGeomID g, dTriArrayCallback* ArrayCallback);

dTriArrayCallback* dGeomTriListGetArrayCallback(dGeomID g);



/* Construction */

dxGeom* dCreateTriList(dSpaceID space, dTriCallback* Callback, dTriArrayCallback* ArrayCallback);



/* Setting data */

void dGeomTriListBuild(dGeomID g, const dcVector3* Vertices, s32 VertexCount, const s32* Indices, s32 IndexCount);



/* Getting data */

void dGeomTriListGetTriangle(dGeomID g, s32 Index, dVector3* v0, dVector3* v1, dVector3* v2);