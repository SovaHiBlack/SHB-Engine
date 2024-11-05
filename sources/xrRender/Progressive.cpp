// Progressive.cpp: implementation of the CProgressive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "..\XR_3DA\mesh.h"
#include "Progressive.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgressive::CProgressive() : CVisual()
{
	xSWI			= 0;
	last_lod		= 0;
}

CProgressive::~CProgressive()
{
}

void CProgressive::Release	()
{
	CVisual::Release();
	xr_free			(nSWI.sw);
	if (xSWI)		{
		xr_free			(xSWI->sw);
		xr_delete		(xSWI);
	}
}

void CProgressive::Load		(pcstr N, IReader *data, u32 dwFlags)
{
	CVisual::Load	(N,data,dwFlags);

	// normal SWI
	destructor<IReader> lods (data->open_chunk	(OGF_SWIDATA));
	nSWI.reserved[0]	= lods().r_u32();	// reserved 16 bytes
	nSWI.reserved[1]	= lods().r_u32();
	nSWI.reserved[2]	= lods().r_u32();
	nSWI.reserved[3]	= lods().r_u32();
	nSWI.count			= lods().r_u32();
	VERIFY				(NULL==nSWI.sw);
	nSWI.sw				= xr_alloc<FSlideWindow>(nSWI.count);
	lods().r			(nSWI.sw,nSWI.count*sizeof(FSlideWindow));

	// fast
#if RENDER==R_R2
	if (m_fast)			{
		destructor<IReader>	geomdef	(data->open_chunk		(OGF_FASTPATH));
		destructor<IReader>	def		(geomdef().open_chunk	(OGF_SWIDATA));

		xSWI				= xr_new<FSlideWindowItem>();
		xSWI->reserved[0]	= def().r_u32();	// reserved 16 bytes
		xSWI->reserved[1]	= def().r_u32();
		xSWI->reserved[2]	= def().r_u32();
		xSWI->reserved[3]	= def().r_u32();
		xSWI->count			= def().r_u32();
		VERIFY				(NULL==xSWI->sw);
		xSWI->sw			= xr_alloc<FSlideWindow>(xSWI->count);
		def().r				(xSWI->sw,xSWI->count*sizeof(FSlideWindow));
	}
#endif
}

void CProgressive::Render	(f32 LOD)
{
#if RENDER==R_R2
	if (m_fast && RImplementation.phase==CRender::PHASE_SMAP)
	{
		int lod_id			= iFloor((1.f-clampr(LOD,0.f,1.f))* f32(xSWI->count-1)+0.5f);
		VERIFY				(lod_id>=0 && lod_id<int(xSWI->count));
		FSlideWindow& SW	= xSWI->sw[lod_id];
		RCache.set_Geometry	(m_fast->rm_geom);
		RCache.Render		(D3DPT_TRIANGLELIST,m_fast->vBase,0,SW.num_verts,m_fast->iBase+SW.offset,SW.num_tris);
		RCache.stat.r.s_static.add	(SW.num_verts);
	} else {
		int lod_id		= last_lod;
		if (LOD>=0.f){
			clamp			(LOD,0.f,1.f);
			lod_id			= iFloor((1.f-LOD)* f32(nSWI.count-1)+0.5f);
			last_lod		= lod_id;
		}
		VERIFY				(lod_id>=0 && lod_id<int(nSWI.count));
		FSlideWindow& SW	= nSWI.sw[lod_id];
		RCache.set_Geometry	(rm_geom);
		RCache.Render		(D3DPT_TRIANGLELIST,vBase,0,SW.num_verts,iBase+SW.offset,SW.num_tris);
		RCache.stat.r.s_static.add	(SW.num_verts);
	}
#else
	int lod_id		= last_lod;
	if (LOD>=0.f){
		clamp		(LOD,0.f,1.f);
		lod_id		= iFloor((1.f-LOD)* f32(nSWI.count-1)+0.5f);
		last_lod	= lod_id;
	}
	VERIFY						(lod_id>=0 && lod_id<int(nSWI.count));
	FSlideWindow& SW			= nSWI.sw[lod_id];
	RCache.set_Geometry			(rm_geom);
	RCache.Render				(D3DPT_TRIANGLELIST,vBase,0,SW.num_verts,iBase+SW.offset,SW.num_tris);
	RCache.stat.r.s_static.add	(SW.num_verts);
#endif
}

#define PCOPY(a)	a = pFrom->a
void	CProgressive::Copy(IRenderVisual* pSrc)
{
	CVisual::Copy	(pSrc);
	CProgressive* pFrom = (CProgressive*)pSrc;
	PCOPY			(nSWI);
	PCOPY			(xSWI);
}
