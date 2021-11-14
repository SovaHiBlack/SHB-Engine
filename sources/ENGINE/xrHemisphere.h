#pragma once

typedef void __stdcall		xrHemisphereIterator(float x, float y, float z, float energy, LPVOID param);

void	xrHemisphereBuild		(int quality, float energy, xrHemisphereIterator* it, LPVOID param);
int		xrHemisphereVertices	(int quality, const Fvector3*& verts);
int		xrHemisphereIndices		(int quality, const unsigned short*& indices);
