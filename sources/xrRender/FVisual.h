// FVisual.h: interface for the FVisual class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\XR_3DA\fbasicvisual.h"

class	Fvisual					: public		IRender_Visual, public IRender_Mesh
{
public:
	IRender_Mesh*				m_fast			;	
public:
	virtual void				Render			(f32 LOD		);		// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
	virtual void				Load			(pcstr N, IReader *data, u32 dwFlags);
	virtual void				Copy			(IRender_Visual *pFrom	);
	virtual void				Release			();

	Fvisual();
	virtual ~Fvisual();
};
