#pragma once

typedef void __stdcall		xrHemisphereIterator(F32 x, F32 y, F32 z, F32 energy, LPVOID param);

void	xrHemisphereBuild		(int quality, F32 energy, xrHemisphereIterator* it, LPVOID param);
int		xrHemisphereVertices	(int quality, const Fvector*& verts);
int		xrHemisphereIndices		(int quality, const u16*& indices);
