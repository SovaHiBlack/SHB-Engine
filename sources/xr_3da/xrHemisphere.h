#pragma once

typedef void __stdcall		xrHemisphereIterator(f32 x, f32 y, f32 z, f32 energy, LPVOID param);

void	xrHemisphereBuild		(s32 quality, f32 energy, xrHemisphereIterator* it, LPVOID param);
s32		xrHemisphereVertices	(s32 quality, const fVector3*& verts);
s32		xrHemisphereIndices		(s32 quality, const u16*& indices);
