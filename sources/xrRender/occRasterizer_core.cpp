#include "stdafx.h"
#include "occRasterizer.h"

static occTri*	currentTri	= 0;
static u32		dwPixels	= 0;
static f32	currentA[3];
static f32 currentB[3];
static f32 currentC[3];

const int BOTTOM = 0, TOP = 1;

void i_order	(f32* A, f32* B, f32* C)
{
	f32* min;
	f32* max;
	f32* mid;
	if (A[1] <= B[1])
	{
        if (B[1] <= C[1]) 
		{  
			min = A; mid = B; max = C; 
		}
        else // C < B
			if   (A[1] <= C[1]) 
			{
				min = A; mid = C; max = B; 
			}
			else 
			{ 
				min = C; mid = A; max = B; 
			}
	}
	else // B < A
	{
		if (A[1] <= C[1]) 
		{ 
			min = B; mid = A; max = C; 
		}
		else // C < A
			if (B[1] <= C[1]) 
			{ 
				min = B; mid = C; max = A; 
			}
			else 
			{ 
				min = C; mid = B; max = A; 
			}
	}
	
	currentA[0]	= min[0]+2;	currentB[0]	= mid[0]+2;	currentC[0]	= max[0]+2;
	currentA[1]	= min[1]+2;	currentB[1]	= mid[1]+2;	currentC[1]	= max[1]+2;
	currentA[2]	= min[2];	currentB[2]	= mid[2];	currentC[2]	= max[2];
}

// Find the closest min/max pixels of a point
IC void Vclamp(int& v, int a, int b)
{
	if (v<a)	v=a; else if (v>=b) v=b-1;
}
IC BOOL shared(occTri* T1, occTri* T2)
{
	if (T1==T2)					return TRUE;
	if (T1->adjacent[0]==T2)	return TRUE;
	if (T1->adjacent[1]==T2)	return TRUE;
	if (T1->adjacent[2]==T2)	return TRUE;
	return FALSE;
}
IC BOOL lesser(f32& a, f32& b)
{
	u32* A = (u32*)(&a);
	u32* B = (u32*)(&b);
	return *A<*B;
}

const f32		one_div_3	= 1.f/3.f;

// Rasterize a scan line between given X point values, corresponding Z values and current color
void i_scan		(int curY, f32 leftX, f32 lhx, f32 rightX, f32 rhx, f32 startZ, f32 endZ)
{
	// calculate span(s)
	f32	start_c	= leftX+lhx;
	f32	end_c	= rightX+rhx;
	
	f32	startR	= leftX-lhx;
	f32	endR	= rightX-rhx;
	
	f32	startT = startR;
	f32 endT = end_c;
	f32	startX = start_c;
	f32 endX = endR;
	if (start_c<startR)		{startT	= start_c;	startX	= startR;	}
	if (end_c<endR)			{endT	= endR;		endX	= end_c;	}
	
	// guard-banding and clipping
	int minT		= iFloor(startT)-1, maxT = iCeil(endT)+1;
	Vclamp			(minT,1,occ_dim-1);
	Vclamp			(maxT,1,occ_dim-1);
	if (minT >= maxT)		return;
	
	int minX		= iCeil(startX), maxX = iFloor(endX);
	Vclamp			(minX,0,occ_dim);
	Vclamp			(maxX,0,occ_dim);
	int limLeft,limRight;
	if (minX >  maxX)	{ limLeft=maxX; limRight=minX;	}
	else				{ limLeft=minX; limRight=maxX;	}
	
	// interpolate
	F32 lenR		= endR - startR;
	F32 Zlen		= endZ - startZ;
	F32 Z			= startZ + (minT - startR)/lenR * Zlen;		// interpolate Z to the start
	F32 Zend		= startZ + (maxT - startR)/lenR * Zlen;		// interpolate Z to the end
	F32 dZ		= (Zend-Z)/(maxT-minT);						// incerement in Z / pixel wrt dX
	
	// gain access to buffers
	occTri** pFrame	= Raster.get_frame();
	F32*	pDepth	= Raster.get_depth();
	
	// left connector
	int	i_base		= curY*occ_dim;
	int i			= i_base+minT;
	int limit		= i_base+limLeft;
	for (; i<limit; i++, Z+=dZ)
	{
		if (shared(currentTri,pFrame[i-1])) 
		{
			F32 ZR = (Z+2*pDepth[i-1])*one_div_3;
			if (ZR<pDepth[i])	{ pFrame[i]	= currentTri; pDepth[i]	= ZR; dwPixels++; }
		}
	}

	// compute the scanline 
	limit				= i_base+maxX;
	for (; i<limit; i++, Z+=dZ) 
	{
		if (Z<pDepth[i])		{ pFrame[i]	= currentTri; pDepth[i] = Z;  dwPixels++; }
	}
	
	// right connector
	i				= i_base+maxT-1;
	limit			= i_base+limRight;
	Z				= Zend-dZ;
	for (; i>=limit; i--, Z-=dZ)
	{
		if (shared(currentTri,pFrame[i+1])) {
			F32 ZR = (Z+2*pDepth[i+1])*one_div_3;
			if (ZR<pDepth[i])	{ pFrame[i]	= currentTri; pDepth[i]	= ZR; dwPixels++; }
		}
	}
}

IC void i_test_micro( int x, int y)
{
	if (x<1) return; else if (x>=occ_dim-1)	return;
	if (y<1) return; else if (y>=occ_dim-1)	return;
	int	pos		= y*occ_dim+x;
	int	pos_up	= pos-occ_dim;
	int	pos_down= pos+occ_dim;
	
	occTri**	pFrame	= Raster.get_frame();
	occTri* T1	= pFrame[pos_up		];
	occTri* T2	= pFrame[pos_down	];
	if (T1 && shared(T1,T2))	
	{
		F32*		pDepth	= Raster.get_depth();
		F32 ZR			= (pDepth[pos_up]+pDepth[pos_down])/2;
		if (ZR<pDepth[pos])	{ pFrame[pos] = T1; pDepth[pos] = ZR; }
	}
}
void i_test		( int x, int y)
{
	i_test_micro	(x,y-1);
	i_test_micro	(x,y+1);
	i_test_micro	(x,y);
}

void i_edge		( int x1, int y1, int x2, int y2)
{
    int dx	= _abs(x2 - x1);
    int dy	= _abs(y2 - y1);

    int sx = x2 >= x1 ? 1 : -1;
    int sy = y2 >= y1 ? 1 : -1;
	
    if ( dy <= dx ){
        int d  = ( dy << 1 ) - dx;
        int d1 = dy << 1;
        int d2 = ( dy - dx ) << 1;
		
		i_test(x1,y1);
        for  (int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx){
            if ( d > 0) { d += d2; y += sy; }
			else        d += d1;
			i_test(x,y);
        }
    }else{
        int d  = ( dx << 1 ) - dy;
        int d1 = dx << 1;
        int d2 = ( dx - dy ) << 1;
		
		i_test(x1,y1);
        for  (int x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy ){
            if ( d > 0) { d += d2; x += sx; }
            else        d += d1;
			i_test(x,y);
        }
    }
}


/* 
Rasterises 1 section of the triangle a 'section' of a triangle is the portion of a triangle between 
2 horizontal scan lines corresponding to 2 vertices of a triangle
p2.y >= p1.y, p1, p2 are start/end vertices
E1 E2 are the triangle edge differences of the 2 bounding edges for this section
*/

IC void i_section	(int Sect, BOOL bMiddle)
{
	// Find the start/end Y pixel coord, set the starting pts for scan line ends
	int		startY, endY;
	F32* startp1;
	F32* startp2;
	F32	E1[3];
	F32 E2[3];

	if (Sect == BOTTOM) { 
		startY	= iCeil(currentA[1]); endY = iFloor(currentB[1])-1; 
		startp1 = startp2 = currentA;
		if (bMiddle)	endY ++;
		
		// check 'endY' for out-of-triangle 
		int test = iFloor(currentC[1]);
		if (endY   >=test) endY --;

		// Find the edge differences
		E1[0] = currentB[0]-currentA[0]; E2[0] = currentC[0]-currentA[0];
		E1[1] = currentB[1]-currentA[1]; E2[1] = currentC[1]-currentA[1];
		E1[2] = currentB[2]-currentA[2]; E2[2] = currentC[2]-currentA[2];
	}
	else { 
		startY  = iCeil(currentB[1]); endY = iFloor(currentC[1]); 
		startp1 = currentA; startp2 = currentB;
		if (bMiddle)	startY --;
		
		// check 'startY' for out-of-triangle 
		int test = iCeil(currentA[1]);
		if (startY < test) startY ++;

		// Find the edge differences
		E1[0] = currentC[0]-currentA[0]; E2[0] = currentC[0]-currentB[0];
		E1[1] = currentC[1]-currentA[1]; E2[1] = currentC[1]-currentB[1];
		E1[2] = currentC[2]-currentA[2]; E2[2] = currentC[2]-currentB[2];
	}
	Vclamp(startY,0,occ_dim);
	Vclamp(endY,  0,occ_dim);
	if (startY >= endY) return;
	
	// Compute the inverse slopes of the lines, ie rate of change of X by Y
	F32 mE1	= E1[0]/E1[1];
	F32 mE2	= E2[0]/E2[1];
	
	// Initial Y offset for left and right (due to pixel rounding)
	F32	e1_init_dY = F32(startY) - startp1[1];
	F32 e2_init_dY = F32(startY) - startp2[1];
	F32	t;
	F32 leftX;
	F32 leftZ;
	F32 rightX;
	F32 rightZ;
	F32 left_dX;
	F32 right_dX;
	F32 left_dZ;
	F32 right_dZ;
	
	// find initial values, step values
	if ( ((mE1<mE2)&&(Sect==BOTTOM)) || ((mE1>mE2)&&(Sect==TOP)) ) 
	{ 
		// E1 is on the Left
		// Initial Starting values for left (from E1)
		t		= e1_init_dY/E1[1]; // Initial fraction of offset
		leftX	= startp1[0] + E1[0]*t; left_dX = mE1;
		leftZ	= startp1[2] + E1[2]*t; left_dZ = E1[2]/E1[1];

		// Initial Ending values for right	(from E2)
		t		= e2_init_dY/E2[1]; // Initial fraction of offset
		rightX	= startp2[0] + E2[0]*t; right_dX = mE2;
		rightZ	= startp2[2] + E2[2]*t; right_dZ = E2[2]/E2[1];
	}
	else { 
		// E2 is on left
		// Initial Starting values for left (from E2)
		t		= e2_init_dY/E2[1]; // Initial fraction of offset
		leftX	= startp2[0] + E2[0]*t; left_dX = mE2;
		leftZ	= startp2[2] + E2[2]*t; left_dZ = E2[2]/E2[1];

		// Initial Ending values for right	(from E1)
		t		= e1_init_dY/E1[1]; // Initial fraction of offset
		rightX	= startp1[0] + E1[0]*t; right_dX = mE1;
		rightZ	= startp1[2] + E1[2]*t; right_dZ = E1[2]/E1[1];
	}

	// Now scan all lines in this section
	F32 lhx = left_dX/2;	leftX	+= lhx;	// half pixel
	F32 rhx = right_dX/2;	rightX	+= rhx;	// half pixel
	for (; startY<=endY; startY++) 
	{
		i_scan	(startY, leftX, lhx, rightX, rhx, leftZ, rightZ);
		leftX	+= left_dX; rightX += right_dX;
		leftZ	+= left_dZ; rightZ += right_dZ;
	}
}

void __stdcall i_section_b0	()
{	i_section	(BOTTOM,0);	}
void __stdcall i_section_b1	()
{	i_section	(BOTTOM,1);	}
void __stdcall i_section_t0	()
{	i_section	(TOP,0);	}
void __stdcall i_section_t1	()
{	i_section	(TOP,1);	}

u32 occRasterizer::rasterize	(occTri* T)
{
	// Order the vertices by Y
	currentTri			= T;
	dwPixels			= 0;
	i_order				(&(T->raster[0].x), &(T->raster[1].x),&(T->raster[2].x));

	// Rasterize sections
	if (currentB[1]-iFloor(currentB[1]) > .5f)	
	{
		i_section_b1	();	// Rasterise First Section
		i_section_t0	();	// Rasterise Second Section
	} else {
		i_section_b0	();	// Rasterise First Section
		i_section_t1	();	// Rasterise Second Section
	}
	return				dwPixels;
}
