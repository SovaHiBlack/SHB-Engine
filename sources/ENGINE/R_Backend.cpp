#include "stdafx.h"

ENGINE_API CBackend			RCache;

// Create Quad-IB
void CBackend::CreateQuadIB		()
{
	const unsigned int dwTriCount	= 4*1024;
	const unsigned int dwIdxCount	= dwTriCount*2*3;
	unsigned short* Indices		= 0;
	unsigned int		dwUsage			= D3DUSAGE_WRITEONLY;
	if (HW.Caps.geometry.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
	R_CHK(HW.pDevice->CreateIndexBuffer(dwIdxCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&QuadIB,NULL));
	R_CHK(QuadIB->Lock(0,0,(void**)&Indices,0));
	{
		int		Cnt = 0;
		int		ICnt= 0;
		for (int i=0; i<dwTriCount; i++)
		{
			Indices[ICnt++]= unsigned short(Cnt+0);
			Indices[ICnt++]= unsigned short(Cnt+1);
			Indices[ICnt++]= unsigned short(Cnt+2);

			Indices[ICnt++]= unsigned short(Cnt+3);
			Indices[ICnt++]= unsigned short(Cnt+2);
			Indices[ICnt++]= unsigned short(Cnt+1);

			Cnt+=4;
		}
	}
	R_CHK(QuadIB->Unlock());
}

// Device dependance
void CBackend::OnDeviceCreate	()
{
	CreateQuadIB		();

	// streams
	Vertex.Create		();
	Index.Create		();

	// invalidate caching
	Invalidate			();
}

void CBackend::OnDeviceDestroy()
{
	// streams
	Index.Destroy		();
	Vertex.Destroy		();

	// Quad
	_RELEASE			(QuadIB);
}
