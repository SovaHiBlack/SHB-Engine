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
	Fvector			raster		[3];
	Fplane			plane;
	F32			area;
	u32				flags;
	u32				skip;
	Fvector			center;
};

const F32			occQ_s32	= F32(0x40000000);	// [-2..2]
const F32			occQ_s16	= F32(16384-1);		// [-2..2]
typedef	s32			occD;

class occRasterizer  
{
private:
	occTri*			bufFrame	[occ_dim][occ_dim];
	F32				bufDepth	[occ_dim][occ_dim];

	occD			bufDepth_0	[occ_dim_0][occ_dim_0];
	occD			bufDepth_1	[occ_dim_1][occ_dim_1];
	occD			bufDepth_2	[occ_dim_2][occ_dim_2];
	occD			bufDepth_3	[occ_dim_3][occ_dim_3];
public:
	IC int			df_2_s32		(F32 d)	{ return iFloor	(d*occQ_s32);				}
	IC s16			df_2_s16		(F32 d)	{ return s16(iFloor	(d*occQ_s16));			}
	IC int			df_2_s32up		(F32 d)	{ return iCeil	(d*occQ_s32);				}
	IC s16			df_2_s16up		(F32 d)	{ return s16(iCeil	(d*occQ_s16));			}
	IC F32		ds32_2_f		(s32 d)		{ return F32(d)/occQ_s32;					}
	IC F32		ds16_2_f		(s16 d)		{ return F32(d)/occQ_s16;					}

	void			clear		();
	void			propagade	();
	u32				rasterize	(occTri* T);
	BOOL			test		(F32 x0, F32 y0, F32 x1, F32 y1, F32 z);
	
	occTri**		get_frame	()			{ return &(bufFrame[0][0]);	}
	F32*			get_depth	()			{ return &(bufDepth[0][0]);	}
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
