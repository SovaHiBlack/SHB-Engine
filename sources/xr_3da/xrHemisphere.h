#pragma once

typedef void __stdcall		xrHemisphereIterator(f32 x, f32 y, f32 z, f32 energy, LPVOID param);

void	xrHemisphereBuild		(int quality, f32 energy, xrHemisphereIterator* it, LPVOID param);
int		xrHemisphereVertices	(int quality, const Fvector*& verts);
int		xrHemisphereIndices		(int quality, const u16*& indices);
