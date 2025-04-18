#include "stdafx.h"

#include "..\XR_3DA\skeletonX.h"
#include "..\XR_3DA\skeletoncustom.h"

void __stdcall xrSkin1W_x86(	vertRender*		D,
								vertBoned1W*	S,
								u32				vCount,
								CBoneInstance*	Bones) 
{
//	return;
	// Prepare
	int U_Count			= vCount/8;
	vertBoned1W*	V	= S;
	vertBoned1W*	E	= V+U_Count*8;

	// Unrolled loop
	for (; S!=E; )
	{
		fMatrix4x4& M0		= Bones[S->matrix].mRenderTransform;
		M0.transform_tiny(D->P,S->P);
		M0.transform_dir (D->N,S->N);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++;
		
		fMatrix4x4& M1		= Bones[S->matrix].mRenderTransform;
		M1.transform_tiny(D->P,S->P);
		M1.transform_dir (D->N,S->N);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++;
		
		fMatrix4x4& M2		= Bones[S->matrix].mRenderTransform;
		M2.transform_tiny(D->P,S->P);
		M2.transform_dir (D->N,S->N);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++;
		
		fMatrix4x4& M3		= Bones[S->matrix].mRenderTransform;
		M3.transform_tiny(D->P,S->P);
		M3.transform_dir (D->N,S->N);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++; 
		
		fMatrix4x4& M4		= Bones[S->matrix].mRenderTransform;
		M4.transform_tiny(D->P,S->P);
		M4.transform_dir (D->N,S->N);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++;
		
		fMatrix4x4& M5		= Bones[S->matrix].mRenderTransform;
		M5.transform_tiny(D->P,S->P);
		M5.transform_dir (D->N,S->N);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++;
		
		fMatrix4x4& M6		= Bones[S->matrix].mRenderTransform;
		M6.transform_tiny(D->P,S->P);
		M6.transform_dir (D->N,S->N);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++;
		
		fMatrix4x4& M7		= Bones[S->matrix].mRenderTransform;
		M7.transform_tiny(D->P,S->P);
		M7.transform_dir (D->N,S->N);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++; 
	}
	
	// The end part
	vertBoned1W* E2 = V+vCount;
	for (; S!=E2; )
	{
		fMatrix4x4& M		= Bones[S->matrix].mRenderTransform;
		M.transform_tiny(D->P,S->P);
		M.transform_dir (D->N,S->N);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++;
	}
}
 