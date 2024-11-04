#include "stdafx.h"
#include "ISpatial.h"

extern fVector3	c_spatial_offset[8];

class	walker
{
public:
	u32			o_count;
	u32			n_count;
public:
	walker					()
	{
		o_count	= 0;
		n_count	= 0;
	}
	void		walk		(ISpatial_NODE* N, fVector3& n_C, f32 n_R)
	{
		// test items
		n_count			+=		1;
		o_count			+=		N->items.size();

		// recurse
		f32	c_R		=		n_R/2;
		for (u32 octant=0; octant<8; octant++)
		{
			if (0==N->children[octant])	continue;
			fVector3		c_C;
			c_C.mad	(n_C,c_spatial_offset[octant],c_R);
			walk						(N->children[octant],c_C,c_R);
		}
	}
};

BOOL	ISpatial_DB::verify			()
{
	walker		W;		W.walk		(m_root,m_center,m_bounds);
	BOOL		bResult = (W.o_count == stat_objects) && (W.n_count == stat_nodes);
	VERIFY		(bResult);
	return		bResult;
}
