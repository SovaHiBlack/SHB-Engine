#pragma once
#include "..\XR_3DA\fhierrarhyvisual.h"

class	FLOD	:	public FHierrarhyVisual
{
	typedef FHierrarhyVisual inherited;
public:
	struct _vertex
	{
		Fvector		v;
		fVector2	t;
		u32			c_rgb_hemi;	// rgb,hemi
		u8			c_sun;
	};
	struct _face
	{
		_vertex		v[4]	;
		Fvector		N		;
	};
	struct _hw 
	{
		Fvector		p0		;
		Fvector		p1		;
		Fvector		n0		;
		Fvector		n1		;
		u32			sun_af	;
		fVector2	t0		;
		fVector2	t1		;
		u32			rgbh0	;
		u32			rgbh1	;
	};

	ref_geom		geom		;
	_face			facets		[8];
	f32			lod_factor	;
public:
	virtual void Render			(f32 LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void Load			(pcstr N, IReader *data, u32 dwFlags);
	virtual void Copy			(IRender_Visual *pFrom	);
};
