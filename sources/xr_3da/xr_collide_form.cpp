#include "stdafx.h"
#include "igame_level.h"
#include "xr_collide_form.h"
#include "Object.h"
#include "xr_area.h"
#include "x_ray.h"
#include "xrLevel.h"
#include "mesh.h"
#include "skeletoncustom.h"
#include "frustum.h"

using namespace	collide;
//----------------------------------------------------------------------
// Class	: CXR_CFObject
// Purpose	: stores collision form
//----------------------------------------------------------------------
ICollisionForm::ICollisionForm(CObject* _owner, ECollisionFormType tp)
{
	owner = _owner;
	m_type = tp;
	bv_sphere.identity( );
}

ICollisionForm::~ICollisionForm( )
{ }

//----------------------------------------------------------------------------------
void CCF_Skeleton::SElement::center(fVector3& center) const
{
	switch (type)
	{
		case SBoneShape::stBox:
		{
			center.set(-b_IM.c.dotproduct(b_IM.i), -b_IM.c.dotproduct(b_IM.j), -b_IM.c.dotproduct(b_IM.k));
		}
		break;
		case SBoneShape::stSphere:
		{
			center.set(s_sphere.P);
		}
		break;
		case SBoneShape::stCylinder:
		{
			center.set(c_cylinder.m_center);
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}
}
bool	pred_find_elem(const CCF_Skeleton::SElement& E, u16 elem)
{
	return E.elem_id < elem;
}
bool CCF_Skeleton::_ElementCenter(u16 elem_id, fVector3& e_center)
{
	ElementVecIt it = std::lower_bound(elements.begin( ), elements.end( ), elem_id, pred_find_elem);
	if (it->elem_id == elem_id)
	{
		it->center(e_center);
		return true;
	}

	return false;
}

IC bool RAYvsOBB(const fMatrix4x4& IM, const fVector3& b_hsize, const fVector3& S, const fVector3& D, f32& R, BOOL bCull)
{
	fBox3 E = {-b_hsize.x, -b_hsize.y, -b_hsize.z,	b_hsize.x,	b_hsize.y,	b_hsize.z};
	// XForm world-2-local
	fVector3	SL;
	fVector3	DL;
	fVector3	PL;
	IM.transform_tiny(SL, S);
	IM.transform_dir(DL, D);

	// Actual test
	fBox3::ERP_Result rp_res = E.Pick2(SL, DL, PL);
	if ((rp_res == fBox3::rpOriginOutside) || (!bCull && (rp_res == fBox3::rpOriginInside)))
	{
		f32 d = PL.distance_to_sqr(SL);
		if (d < R * R)
		{
			R = _sqrt(d);
			VERIFY(R >= 0.f);
			return true;
		}
	}

	return false;
}
IC bool RAYvsSPHERE(const fSphere& s_sphere, const fVector3& S, const fVector3& D, f32& R, BOOL bCull)
{
	fSphere::ERP_Result rp_res = s_sphere.intersect(S, D, R);
	VERIFY(R >= 0.f);
	return				((rp_res == fSphere::rpOriginOutside) || (!bCull && (rp_res == fSphere::rpOriginInside)));
}
IC bool RAYvsCYLINDER(const fCylinder& c_cylinder, const fVector3& S, const fVector3& D, f32& R, BOOL bCull)
{
	// Actual test
	fCylinder::ERP_Result rp_res = c_cylinder.intersect(S, D, R);
	VERIFY(R >= 0.f);
	return				((rp_res == fCylinder::rpOriginOutside) || (!bCull && (rp_res == fCylinder::rpOriginInside)));
}

CCF_Skeleton::CCF_Skeleton(CObject* O) : ICollisionForm(O, cftObject)
{
	CKinematics* K = PKinematics(O->Visual( )); VERIFY3(K, "Can't create skeleton without Kinematics.", *O->cNameVisual( ));
	bv_box.set(K->vis.box);
	bv_box.getsphere(bv_sphere.P, bv_sphere.R);
	vis_mask = 0;
}

void CCF_Skeleton::BuildState( )
{
	dwFrame = Device.dwFrame;
	CKinematics* K = PKinematics(owner->Visual( ));
	K->CalculateBones( );
	const fMatrix4x4& L2W = owner->XFORM( );

	if (vis_mask != K->LL_GetBonesVisible( ))
	{
		vis_mask = K->LL_GetBonesVisible( );
		elements.clear_not_free( );
		bv_box.set(K->vis.box);
		bv_box.getsphere(bv_sphere.P, bv_sphere.R);
		for (u16 i = 0; i < K->LL_BoneCount( ); i++)
		{
			if (!K->LL_GetBoneVisible(i))
			{
				continue;
			}

			SBoneShape& shape = K->LL_GetData(i).shape;
			if (SBoneShape::stNone == shape.type)
			{
				continue;
			}

			if (shape.flags.is(SBoneShape::sfNoPickable))
			{
				continue;
			}

			elements.push_back(SElement(i, shape.type));
		}
	}

	for (ElementVecIt I = elements.begin( ); I != elements.end( ); I++)
	{
		if (!I->valid( ))
		{
			continue;
		}

		SBoneShape& shape = K->LL_GetData(I->elem_id).shape;
		fMatrix4x4					ME;
		fMatrix4x4					T;
		fMatrix4x4					TW;
		const fMatrix4x4& Mbone = K->LL_GetTransform(I->elem_id);
		switch (I->type)
		{
			case SBoneShape::stBox:
			{
				const fObb& B = shape.box;
				B.xform_get(ME);
				I->b_hsize.set(B.m_halfsize);
				// prepare matrix World to Element
				T.mul_43(Mbone, ME);		// model space
				TW.mul_43(L2W, T);		// world space
				bool b = I->b_IM.invert_b(TW);
				// check matrix validity
				if (!b)
				{
					Msg("! ERROR: invalid bone xform (Slipch?). Bone disabled.");
					Msg("! ERROR: bone_id=[%d], world_pos[%f,%f,%f]", I->elem_id, VPUSH(TW.c));
					Msg("visual name %s", owner->cNameVisual( ));
					Msg("object name %s", owner->cName( ));
					I->elem_id = u16(-1);				//. hack - disable invalid bone
				}
			}
			break;
			case SBoneShape::stSphere:
			{
				const fSphere& S = shape.sphere;
				Mbone.transform_tiny(I->s_sphere.P, S.P);
				L2W.transform_tiny(I->s_sphere.P);
				I->s_sphere.R = S.R;
			}
			break;
			case SBoneShape::stCylinder:
			{
				const fCylinder& C = shape.cylinder;
				Mbone.transform_tiny(I->c_cylinder.m_center, C.m_center);
				L2W.transform_tiny(I->c_cylinder.m_center);
				Mbone.transform_dir(I->c_cylinder.m_direction, C.m_direction);
				L2W.transform_dir(I->c_cylinder.m_direction);
				I->c_cylinder.m_height = C.m_height;
				I->c_cylinder.m_radius = C.m_radius;
			}
			break;
		}
	}
}

void CCF_Skeleton::BuildTopLevel( )
{
	dwFrameTL = Device.dwFrame;
	IRenderVisual* K = owner->Visual( );
	fBox3& B = K->vis.box;
	bv_box.min.average(B.min);
	bv_box.max.average(B.max);
	bv_box.grow(0.05f);
	bv_sphere.P.average(K->vis.sphere.P);
	bv_sphere.R += K->vis.sphere.R;
	bv_sphere.R *= 0.5f;
	VERIFY(_valid(bv_sphere));
}

BOOL CCF_Skeleton::_RayQuery(const collide::ray_defs& Q, collide::rq_results& R)
{
	if (dwFrameTL != Device.dwFrame)
	{
		BuildTopLevel( );
	}

	fSphere w_bv_sphere;
	owner->XFORM( ).transform_tiny(w_bv_sphere.P, bv_sphere.P);
	w_bv_sphere.R = bv_sphere.R;

	// 
	f32 tgt_dist = Q.range;
	f32 aft[2];
	s32 quant;
	fSphere::ERP_Result res = w_bv_sphere.intersect(Q.start, Q.dir, tgt_dist, quant, aft);
	if ((fSphere::rpNone == res) || ((fSphere::rpOriginOutside == res) && (aft[0] > tgt_dist)))
	{
		return FALSE;
	}

	if (dwFrame != Device.dwFrame)
	{
		BuildState( );
	}
	else
	{
		CKinematics* K = PKinematics(owner->Visual( ));
		if (K->LL_GetBonesVisible( ) != vis_mask)
		{	// Model changed between ray-picks
			dwFrame = Device.dwFrame - 1;
			BuildState( );
		}
	}

	BOOL bHIT = FALSE;
	for (ElementVecIt I = elements.begin( ); I != elements.end( ); I++)
	{
		if (!I->valid( ))
		{
			continue;
		}

		bool res = false;
		f32 range = Q.range;
		switch (I->type)
		{
			case SBoneShape::stBox:
			{
				res = RAYvsOBB(I->b_IM, I->b_hsize, Q.start, Q.dir, range, Q.flags & CDB::OPT_CULL);
			}
			break;
			case SBoneShape::stSphere:
			{
				res = RAYvsSPHERE(I->s_sphere, Q.start, Q.dir, range, Q.flags & CDB::OPT_CULL);
			}
			break;
			case SBoneShape::stCylinder:
			{
				res = RAYvsCYLINDER(I->c_cylinder, Q.start, Q.dir, range, Q.flags & CDB::OPT_CULL);
			}
			break;
		}

		if (res)
		{
			bHIT = TRUE;
			R.append_result(owner, range, I->elem_id, Q.flags & CDB::OPT_ONLYNEAREST);
			if (Q.flags & CDB::OPT_ONLYFIRST) break;
		}
	}

	return bHIT;
}

//----------------------------------------------------------------------------------
CCF_EventBox::CCF_EventBox(CObject* O) : ICollisionForm(O, cftShape)
{
	fVector3 A[8];
	fVector3 B[8];
	A[0].set(-1, -1, -1);
	A[1].set(-1, -1, +1);
	A[2].set(-1, +1, +1);
	A[3].set(-1, +1, -1);
	A[4].set(+1, +1, +1);
	A[5].set(+1, +1, -1);
	A[6].set(+1, -1, +1);
	A[7].set(+1, -1, -1);

	const fMatrix4x4& T = O->XFORM( );
	for (s32 i = 0; i < 8; i++)
	{
		A[i].mul(.5f);
		T.transform_tiny(B[i], A[i]);
	}

	bv_box.set(-0.5f, -0.5f, -0.5f, +0.5f, +0.5f, +0.5f);
	fVector3 R;
	R.set(bv_box.min);
	T.transform_dir(R);
	bv_sphere.R = R.magnitude( );

	Planes[0].build(B[0], B[3], B[5]);
	Planes[1].build(B[1], B[2], B[3]);
	Planes[2].build(B[6], B[5], B[4]);
	Planes[3].build(B[4], B[2], B[1]);
	Planes[4].build(B[3], B[2], B[4]);
	Planes[5].build(B[1], B[0], B[6]);
}

BOOL CCF_EventBox::Contact(CObject* O)
{
	IRenderVisual* V = O->Visual( );
	fVector3& P = V->vis.sphere.P;
	f32 R = V->vis.sphere.R;

	fVector3 PT;
	O->XFORM( ).transform_tiny(PT, P);

	for (s32 i = 0; i < 6; i++)
	{
		if (Planes[i].classify(PT) > R)
		{
			return FALSE;
		}
	}

	return TRUE;
}
BOOL CCF_EventBox::_RayQuery(const collide::ray_defs& Q, collide::rq_results& R)
{
	return FALSE;
}
/*
void CCF_EventBox::_BoxQuery(const fBox3& B, const fMatrix4x4& M, u32 flags)
{   return; }
*/

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
CCF_Shape::CCF_Shape(CObject* _owner) : ICollisionForm(_owner, cftShape)
{ }
BOOL CCF_Shape::_RayQuery(const collide::ray_defs& Q, collide::rq_results& R)
{
	// Convert ray into local model space
	fVector3 dS;
	fVector3 dD;
	fMatrix4x4 temp;
	temp.invert(owner->XFORM( ));
	temp.transform_tiny(dS, Q.start);
	temp.transform_dir(dD, Q.dir);

	// 
	if (!bv_sphere.intersect(dS, dD))
	{
		return FALSE;
	}

	BOOL bHIT = FALSE;
	for (u32 el = 0; el < shapes.size( ); el++)
	{
		shape_def& shape = shapes[el];
		f32 range = Q.range;
		switch (shape.type)
		{
			case 0:
			{ // sphere
				fSphere::ERP_Result	rp_res = shape.data.sphere.intersect(dS, dD, range);
				if ((rp_res == fSphere::rpOriginOutside) || (!(Q.flags & CDB::OPT_CULL) && (rp_res == fSphere::rpOriginInside)))
				{
					bHIT = TRUE;
					R.append_result(owner, range, el, Q.flags & CDB::OPT_ONLYNEAREST);
					if (Q.flags & CDB::OPT_ONLYFIRST)
					{
						return TRUE;
					}
				}
			}
			break;
			case 1: // box
			{
				fBox3 box;
				box.identity( );
				fMatrix4x4& B = shape.data.ibox;
				fVector3 S1;
				fVector3 D1;
				fVector3 P;
				B.transform_tiny(S1, dS);
				B.transform_dir(D1, dD);
				fBox3::ERP_Result	rp_res = box.Pick2(S1, D1, P);
				if ((rp_res == fBox3::rpOriginOutside) || (!(Q.flags & CDB::OPT_CULL) && (rp_res == fBox3::rpOriginInside)))
				{
					f32 d = P.distance_to_sqr(dS);
					if (d < range * range)
					{
						range = _sqrt(d);
						bHIT = TRUE;
						R.append_result(owner, range, el, Q.flags & CDB::OPT_ONLYNEAREST);
						if (Q.flags & CDB::OPT_ONLYFIRST)
						{
							return TRUE;
						}
					}
				}
			}
			break;
		}
	}

	return bHIT;
}
/*
void CCF_Shape::_BoxQuery(const fBox3& B, const fMatrix4x4& M, u32 flags)
{   return; }
*/
void CCF_Shape::add_sphere(fSphere& S)
{
	shapes.push_back(shape_def( ));
	shapes.back( ).type = 0;
	shapes.back( ).data.sphere.set(S);
}

void CCF_Shape::add_box(fMatrix4x4& B)
{
	shapes.push_back(shape_def( ));
	shapes.back( ).type = 1;
	shapes.back( ).data.box.set(B);
	shapes.back( ).data.ibox.invert(B);
}

void CCF_Shape::ComputeBounds( )
{
	bv_box.invalidate( );

	BOOL bCalcSphere = (shapes.size( ) > 1);
	for (u32 el = 0; el < shapes.size( ); el++)
	{
		switch (shapes[el].type)
		{
			case 0: // sphere
			{
				fSphere		T = shapes[el].data.sphere;
				fVector3		P;
				P.set(T.P);	P.sub(T.R);	bv_box.modify(P);
				P.set(T.P);	P.add(T.R);	bv_box.modify(P);
				bv_sphere = T;
			}
			break;
			case 1:	// box
			{
				fVector3		A;
				fVector3		B;
				fMatrix4x4& T = shapes[el].data.box;

				// Build points
				A.set(-.5f, -.5f, -.5f); T.transform_tiny(B, A); bv_box.modify(B);
				A.set(-.5f, -.5f, +.5f); T.transform_tiny(B, A); bv_box.modify(B);
				A.set(-.5f, +.5f, +.5f); T.transform_tiny(B, A); bv_box.modify(B);
				A.set(-.5f, +.5f, -.5f); T.transform_tiny(B, A); bv_box.modify(B);
				A.set(+.5f, +.5f, +.5f); T.transform_tiny(B, A); bv_box.modify(B);
				A.set(+.5f, +.5f, -.5f); T.transform_tiny(B, A); bv_box.modify(B);
				A.set(+.5f, -.5f, +.5f); T.transform_tiny(B, A); bv_box.modify(B);
				A.set(+.5f, -.5f, -.5f); T.transform_tiny(B, A); bv_box.modify(B);

				bCalcSphere = TRUE;
			}
			break;
		}
	}

	if (bCalcSphere)
	{
		bv_box.getsphere(bv_sphere.P, bv_sphere.R);
	}
}

BOOL CCF_Shape::Contact(CObject* O)
{
	// Build object-sphere in World-Space
	fSphere			S;
	if (O->Visual( ))
	{
		O->Center(S.P);
		S.R = O->Radius( );
	}
	else if (O->CFORM( ))
	{
		S = O->CFORM( )->getSphere( );
		O->XFORM( ).transform_tiny(S.P);
	}
	else
	{
		return FALSE;
	}

	// Get our matrix
	const fMatrix4x4& XF = Owner( )->XFORM( );

	// Iterate
	for (u32 el = 0; el < shapes.size( ); el++)
	{
		switch (shapes[el].type)
		{
			case 0: // sphere
			{
				fSphere		Q;
				fSphere& T = shapes[el].data.sphere;
				XF.transform_tiny(Q.P, T.P);
				Q.R = T.R;
				if (S.intersect(Q))	return TRUE;
			}
			break;
			case 1:	// box
			{
				fMatrix4x4		Q;
				fMatrix4x4& T = shapes[el].data.box;
				Q.mul_43(XF, T);

				// Build points
				fVector3 A;
				fVector3 B[8];
				fPlane3  P;
				A.set(-.5f, -.5f, -.5f);	Q.transform_tiny(B[0], A);
				A.set(-.5f, -.5f, +.5f);	Q.transform_tiny(B[1], A);
				A.set(-.5f, +.5f, +.5f);	Q.transform_tiny(B[2], A);
				A.set(-.5f, +.5f, -.5f);	Q.transform_tiny(B[3], A);
				A.set(+.5f, +.5f, +.5f);	Q.transform_tiny(B[4], A);
				A.set(+.5f, +.5f, -.5f);	Q.transform_tiny(B[5], A);
				A.set(+.5f, -.5f, +.5f);	Q.transform_tiny(B[6], A);
				A.set(+.5f, -.5f, -.5f);	Q.transform_tiny(B[7], A);

				P.build(B[0], B[3], B[5]);	if (P.classify(S.P) > S.R) break;
				P.build(B[1], B[2], B[3]);	if (P.classify(S.P) > S.R) break;
				P.build(B[6], B[5], B[4]);	if (P.classify(S.P) > S.R) break;
				P.build(B[4], B[2], B[1]);	if (P.classify(S.P) > S.R) break;
				P.build(B[3], B[2], B[4]);	if (P.classify(S.P) > S.R) break;
				P.build(B[1], B[0], B[6]);	if (P.classify(S.P) > S.R) break;
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}
