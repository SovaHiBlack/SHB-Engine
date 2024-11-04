// occRasterizer.h: interface for the occRasterizer class.
//////////////////////////////////////////////////////////////////////
#pragma once

const int	occ_dim_0			= 64;
const int	occ_dim_1			= occ_dim_0/2;
const int	occ_dim_2			= occ_dim_1/2;
const int	occ_dim_3			= occ_dim_2/2;
const int	occ_dim				= occ_dim_0+4;	// 2 pixel border around frame

class occTri
{
public:	
	occTri*			adjacent	[3];
	fVector3			raster		[3];
	fPlane3			plane;
	f32				area;
	u32				flags;
	u32				skip;
	fVector3			center;
};

const f32			occQ_s32	= f32(0x40000000);	// [-2..2]
const f32			occQ_s16	= f32(16384-1);		// [-2..2]
typedef	s32			occD;

class occRasterizer  
{
private:
	occTri*			bufFrame	[occ_dim][occ_dim];
	f32				bufDepth	[occ_dim][occ_dim];

	occD			bufDepth_0	[occ_dim_0][occ_dim_0];
	occD			bufDepth_1	[occ_dim_1][occ_dim_1];
	occD			bufDepth_2	[occ_dim_2][occ_dim_2];
	occD			bufDepth_3	[occ_dim_3][occ_dim_3];
public:
	IC int			df_2_s32		(f32 d)	{ return iFloor	(d*occQ_s32);				}
	IC s16			df_2_s16		(f32 d)	{ return s16(iFloor	(d*occQ_s16));			}
	IC int			df_2_s32up		(f32 d)	{ return iCeil	(d*occQ_s32);				}
	IC s16			df_2_s16up		(f32 d)	{ return s16(iCeil	(d*occQ_s16));			}
	IC f32		ds32_2_f		(s32 d)		{ return f32(d)/occQ_s32;					}
	IC f32		ds16_2_f		(s16 d)		{ return f32(d)/occQ_s16;					}

	void			clear		();
	void			propagade	();
	u32				rasterize	(occTri* T);
	BOOL			test		(f32 x0, f32 y0, f32 x1, f32 y1, f32 z);
	
	occTri**		get_frame	()			{ return &(bufFrame[0][0]);	}
	f32*			get_depth	()			{ return &(bufDepth[0][0]);	}
	occD*			get_depth_level	(int level)	
	{
		switch (level)
		{
		case 0:		return &(bufDepth_0[0][0]);	
		case 1:		return &(bufDepth_1[0][0]);	
		case 2:		return &(bufDepth_2[0][0]);	
		case 3:		return &(bufDepth_3[0][0]);	
		default:	return NULL;
		}
	}
	
	occRasterizer	();
	~occRasterizer	();
};

extern occRasterizer	Raster;
