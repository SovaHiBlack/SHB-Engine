#include "stdafx.h"

#include "..\XR_3DA\skeletonX.h"
#include "..\XR_3DA\skeletoncustom.h"

//#pragma optimize("a",on)
// it means no aliasing inside the function, /Oa compiler option
void __stdcall xrSkin2W_x86(vertRender*		D,
							vertBoned2W*	S,
							u32				vCount,
							CBoneInstance*	Bones) 
{
	// Prepare
	int U_Count			= vCount;
	vertBoned2W*	V	= S;
	vertBoned2W*	E	= V+U_Count;
	fVector3			P0;
	fVector3			N0;
	fVector3			P1;
	fVector3			N1;

	// NON-Unrolled loop
	for (; S!=E; ){
		if (S->matrix1!=S->matrix0){
			fMatrix4x4& M0		= Bones[S->matrix0].mRenderTransform;
			fMatrix4x4& M1		= Bones[S->matrix1].mRenderTransform;
			M0.transform_tiny(P0,S->P);
			M0.transform_dir (N0,S->N);
			M1.transform_tiny(P1,S->P);
			M1.transform_dir (N1,S->N);
			D->P.lerp		(P0,P1,S->w);
			D->N.lerp		(N0,N1,S->w);
			D->u			= S->u;
			D->v			= S->v;
		}else{
			fMatrix4x4& M0		= Bones[S->matrix0].mRenderTransform;
			M0.transform_tiny(D->P,S->P);
			M0.transform_dir (D->N,S->N);
			D->u			= S->u;
			D->v			= S->v;
		}
		S++; D++;
	}
}
