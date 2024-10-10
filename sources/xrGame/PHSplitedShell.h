#pragma once

#include "PHShell.h"

class CPHSplitedShell: 
	public CPHShell
{
	f32					m_max_AABBradius;
	virtual		void		SetMaxAABBRadius	(f32 size){m_max_AABBradius=size;}

protected:
	virtual		void		Collide				()										;
	virtual		void		get_spatial_params	()										;
	virtual		void		DisableObject		()										;

public:
	CPHSplitedShell								(){m_max_AABBradius=dInfinity;}
};
