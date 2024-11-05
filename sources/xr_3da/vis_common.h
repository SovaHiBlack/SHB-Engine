#pragma once

#pragma pack(push,4)
struct vis_data
{
	fSphere		sphere;				//
	fBox3		box;				//
	u32			marker;				// for different sub-renders
	u32			accept_frame;		// when it was requisted accepted for main render
	u32			hom_frame;			// when to perform test - shedule
	u32			hom_tested;			// when it was last time tested

	IC void clear( )
	{
		sphere.P.set(0.0f, 0.0f, 0.0f);
		sphere.R = 0.0f;
		box.invalidate( );
		marker = 0;
		accept_frame = 0;
		hom_frame = 0;
		hom_tested = 0;
	}
};
#pragma pack(pop)
