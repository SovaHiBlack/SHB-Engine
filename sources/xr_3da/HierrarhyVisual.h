// HierrarhyVisual.h: interface for the CHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "BasicVisual.h"

class ENGINE_API CHierrarhyVisual : public IRenderVisual
{
public:
	xr_vector<IRenderVisual*>		children;
	BOOL							bDontDelete;

public:
	CHierrarhyVisual( );
	virtual 		~CHierrarhyVisual( );

	virtual void	Load(pcstr N, IReader* data, u32 dwFlags);
	virtual void	Copy(IRenderVisual* pFrom);
	virtual void	Release( );
};
