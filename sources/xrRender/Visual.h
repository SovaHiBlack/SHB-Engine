// Visual.h: interface for the CVisual class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\XR_3DA\BasicVisual.h"

class	CVisual : public		IRenderVisual, public IRenderMesh
{
public:
	IRenderMesh*				m_fast			;

public:
	virtual void				Render			(f32 LOD		);		// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
	virtual void				Load			(pcstr N, IReader *data, u32 dwFlags);
	virtual void				Copy(IRenderVisual* pFrom);
	virtual void				Release			();

	CVisual();
	virtual ~CVisual();
};
