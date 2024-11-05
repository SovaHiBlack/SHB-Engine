// Progressive.h: interface for the CProgressive class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Visual.h"
struct	FSlideWindowItem;

class	CProgressive : public CVisual
{
protected:
	FSlideWindowItem	nSWI		;
	FSlideWindowItem*	xSWI		;
	u32					last_lod	;
public:
	CProgressive();
	virtual 			~CProgressive();
	virtual void 		Render		(f32 LOD);		// LOD - Level Of Detail  [0.0f - min, 1.0f - max], -1 = Ignored
	virtual void 		Load		(pcstr N, IReader *data,u32 dwFlags);
	virtual void 		Copy(IRenderVisual* pFrom);
	virtual void 		Release		();
private:
	CProgressive(const CProgressive& other);
	void	operator=			( const CProgressive& other);
};
