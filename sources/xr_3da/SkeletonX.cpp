// SkeletonX.cpp: implementation of the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "Render.h"

#include "SkeletonX.h"
#include "SkeletonCustom.h"
#include "mesh.h"
#include "xrCPU_Pipe.h"

#include "cl_intersect.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
shared_str	s_bones_array_const;

#pragma pack(push,1)


//////////////////////////////////////////////////////////////////////
// Body Part
//////////////////////////////////////////////////////////////////////
void CSkeletonX::AfterLoad(CKinematics* parent, u16 child_idx)
{
	SetParent(parent);
	ChildIDX = child_idx;
}

void CSkeletonX::_Copy(CSkeletonX* B)
{
	Parent = NULL;
	ChildIDX = B->ChildIDX;
	Vertices1W = B->Vertices1W;
	Vertices2W = B->Vertices2W;
	BonesUsed = B->BonesUsed;

	// caution - overlapped (union)
	cache_DiscardID = B->cache_DiscardID;
	cache_vCount = B->cache_vCount;
	cache_vOffset = B->cache_vOffset;
	RenderMode = B->RenderMode;
	RMS_boneid = B->RMS_boneid;
	RMS_bonecount = B->RMS_bonecount;
}
//////////////////////////////////////////////////////////////////////

void CSkeletonX::_Render(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount)
{
	RCache.stat.r.s_dynamic.add(vCount);
	switch (RenderMode)
	{
		case RM_SKINNING_SOFT:
		{
			_Render_soft(hGeom, vCount, iOffset, pCount);
			RCache.stat.r.s_dynamic_sw.add(vCount);
		}
		break;
		case RM_SINGLE:
		{
			fMatrix4x4 W;
			W.mul_43(RCache.xforms.m_w, Parent->LL_GetTransform_R(u16(RMS_boneid)));
			RCache.set_xform_world(W);
			RCache.set_Geometry(hGeom);
			RCache.Render(D3DPT_TRIANGLELIST, 0, 0, vCount, iOffset, pCount);
			RCache.stat.r.s_dynamic_inst.add(vCount);
		}
		break;
		case RM_SKINNING_1B:
		case RM_SKINNING_2B:
		{
			// transfer matrices
			ref_constant			array = RCache.get_c(s_bones_array_const);
			u32						count = RMS_bonecount;
			for (u32 mid = 0; mid < count; mid++)
			{
				fMatrix4x4& M = Parent->LL_GetTransform_R(u16(mid));
				u32			id = mid * 3;
				RCache.set_ca(&*array, id + 0, M._11, M._21, M._31, M._41);
				RCache.set_ca(&*array, id + 1, M._12, M._22, M._32, M._42);
				RCache.set_ca(&*array, id + 2, M._13, M._23, M._33, M._43);
			}

			// render
			RCache.set_Geometry(hGeom);
			RCache.Render(D3DPT_TRIANGLELIST, 0, 0, vCount, iOffset, pCount);
			if (RM_SKINNING_1B == RenderMode)
			{
				RCache.stat.r.s_dynamic_1B.add(vCount);
			}
			else
			{
				RCache.stat.r.s_dynamic_2B.add(vCount);
			}
		}
		break;
	}
}

void CSkeletonX::_Render_soft(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount)
{
	u32 vOffset = cache_vOffset;

	_VertexStream& _VS = RCache.Vertex;
	if (cache_DiscardID != _VS.DiscardID( ) || vCount >= cache_vCount)
	{
		vertRender* Dest = (vertRender*)_VS.Lock(vCount, hGeom->vb_stride, vOffset);
		cache_DiscardID = _VS.DiscardID( );
		cache_vCount = vCount;
		cache_vOffset = vOffset;

		Device.Statistic->RenderDUMP_SKIN.Begin( );
		if (*Vertices1W)
		{
			PSGP.skin1W(
				Dest,										// dest
				*Vertices1W,								// source
				vCount,										// count
				Parent->bone_instances						// bones
			);
		}
		else
		{
			PSGP.skin2W(
				Dest,										// dest
				*Vertices2W,								// source
				vCount,										// count
				Parent->bone_instances						// bones
			);
		}

		Device.Statistic->RenderDUMP_SKIN.End( );
		_VS.Unlock(vCount, hGeom->vb_stride);
	}

	RCache.set_Geometry(hGeom);
	RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, vCount, iOffset, pCount);
}

//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Load(pcstr N, IReader* data, u32& dwVertCount)
{
	s_bones_array_const = "sbones_array";
#pragma todo("container is created in stack!")
	xr_vector<u16>	bids;

	// Load vertices
	R_ASSERT(data->find_chunk(OGF_VERTICES));

	u16			hw_bones = u16((HW.Caps.geometry.dwRegisters - 22) / 3);
	u16			sw_bones = 0;

	u32			dwVertType, size, it, crc;
	dwVertType = data->r_u32( );
	dwVertCount = data->r_u32( );

	RenderMode = RM_SKINNING_SOFT;
	Render->shader_option_skinning(-1);
	switch (dwVertType)
	{
		case OGF_VERTEXFORMAT_FVF_1L: // 1-Link
		{
			size = dwVertCount * sizeof(vertBoned1W);
			vertBoned1W* VO = (vertBoned1W*)data->pointer( );
			for (it = 0; it < dwVertCount; it++)
			{
				u16		mid = (u16)VO[it].matrix;
				if (bids.end( ) == std::find(bids.begin( ), bids.end( ), mid))	bids.push_back(mid);
				if (mid > sw_bones)	sw_bones = mid;
			}

			if (1 == bids.size( ))
			{
				// HW- single bone
				RenderMode = RM_SINGLE;
				RMS_boneid = *bids.begin( );
				Render->shader_option_skinning(0);
			}
			else if (sw_bones <= hw_bones)
			{
				// HW- one weight
				RenderMode = RM_SKINNING_1B;
				RMS_bonecount = sw_bones + 1;
				Render->shader_option_skinning(1);
			}
			else
			{
				// software
				crc = crc32(data->pointer( ), size);
				Vertices1W.create(crc, dwVertCount, (vertBoned1W*)data->pointer( ));
				Render->shader_option_skinning(-1);
			}
		}
		break;
		case OGF_VERTEXFORMAT_FVF_2L: // 2-Link
		{
			size = dwVertCount * sizeof(vertBoned2W);
			vertBoned2W* VO = (vertBoned2W*)data->pointer( );
			for (it = 0; it < dwVertCount; it++)
			{
				if (VO[it].matrix0 > sw_bones)
				{
					sw_bones = VO[it].matrix0;
				}

				if (VO[it].matrix1 > sw_bones)
				{
					sw_bones = VO[it].matrix1;
				}

				if (bids.end( ) == std::find(bids.begin( ), bids.end( ), VO[it].matrix0))
				{
					bids.push_back(VO[it].matrix0);
				}

				if (bids.end( ) == std::find(bids.begin( ), bids.end( ), VO[it].matrix1))
				{
					bids.push_back(VO[it].matrix1);
				}
			}

			if (sw_bones <= hw_bones)
			{
				// HW- two weights
				RenderMode = RM_SKINNING_2B;
				RMS_bonecount = sw_bones + 1;
				Render->shader_option_skinning(2);
			}
			else
			{
				// software
				crc = crc32(data->pointer( ), size);
				Vertices2W.create(crc, dwVertCount, (vertBoned2W*)data->pointer( ));
				Render->shader_option_skinning(-1);
			}
		}
		break;
		default:
		{
			Debug.fatal(DEBUG_INFO, "Invalid vertex type in skinned model '%s'", N);
		}
		break;
	}

	if (bids.size( ) > 1)
	{
		crc = crc32(&*bids.begin( ), bids.size( ) * sizeof(u16));
		BonesUsed.create(crc, bids.size( ), &*bids.begin( ));
	}
}

BOOL CSkeletonX::has_visible_bones( )
{
	//IRenderVisual*	me	= dynamic_cast<IRenderVisual*>	(this);
	//Msg	("~ has_visible_bones: mode[%d] - count[%d], name=%s",RenderMode,BonesUsed.size(),me->dbg_name.c_str());	//.
	if (RM_SINGLE == RenderMode)
	{
		//	Msg	("* has_visible_bones: bone-list: |%d:%s",RMS_boneid,Parent->LL_BoneName_dbg(RMS_boneid));
		return Parent->LL_GetBoneVisible((u16)RMS_boneid);
	}

	/*
	string4096	all="",single="";
	for (u32 it=0; it<BonesUsed.size(); it++)
	{
		sprintf		(single,"|%d:%s",BonesUsed[it],Parent->LL_BoneName_dbg(BonesUsed[it]));
		strcat		(all,single);
	}
	Msg	("* has_visible_bones: bone-list: %s",all);
	*/

	for (u32 it = 0; it < BonesUsed.size( ); it++)
	{
		if (Parent->LL_GetBoneVisible(BonesUsed[it]))
		{
			// Msg		("* has_visible_bones: visible: %d",	BonesUsed[it]);
			return	TRUE;
		}
	}

	// Msg		("* has_visible_bones: non-visible");
	return	FALSE;
}

//-----------------------------------------------------------------------------------------------------
// Wallmarks
//-----------------------------------------------------------------------------------------------------
BOOL CSkeletonX::_PickBoneSoft1W(fVector3& normal, f32& dist, const fVector3& S, const fVector3& D, u16* indices, CBoneData::FacesVec& faces)
{
	VERIFY(*Vertices1W);
	bool intersect = FALSE;
	for (CBoneData::FacesVecIt it = faces.begin( ); it != faces.end( ); it++)
	{
		fVector3			p[3];
		u32 idx = (*it) * 3;
		for (u32 k = 0; k < 3; k++)
		{
			vertBoned1W& vert = Vertices1W[indices[idx + k]];
			const fMatrix4x4& xform = Parent->LL_GetBoneInstance((u16)vert.matrix).mRenderTransform;
			xform.transform_tiny(p[k], vert.P);
		}

		f32 u;
		f32 v;
		f32 range = flt_max;
		if (CDB::TestRayTri(S, D, p, u, v, range, true) && (range < dist))
		{
			normal.mknormal(p[0], p[1], p[2]);
			dist = range;
			intersect = TRUE;
		}
	}
	return intersect;
}

BOOL CSkeletonX::_PickBoneSoft2W(fVector3& normal, f32& dist, const fVector3& S, const fVector3& D, u16* indices, CBoneData::FacesVec& faces)
{
	VERIFY(*Vertices2W);
	bool intersect = FALSE;
	for (CBoneData::FacesVecIt it = faces.begin( ); it != faces.end( ); it++)
	{
		fVector3			p[3];
		u32 idx = (*it) * 3;
		for (u32 k = 0; k < 3; k++)
		{
			fVector3		P0;
			fVector3		P1;
			vertBoned2W& vert = Vertices2W[indices[idx + k]];
			fMatrix4x4& xform0 = Parent->LL_GetBoneInstance(vert.matrix0).mRenderTransform;
			fMatrix4x4& xform1 = Parent->LL_GetBoneInstance(vert.matrix1).mRenderTransform;
			xform0.transform_tiny(P0, vert.P);
			xform1.transform_tiny(P1, vert.P);
			p[k].lerp(P0, P1, vert.w);
		}

		f32 u;
		f32 v;
		f32 range = flt_max;
		if (CDB::TestRayTri(S, D, p, u, v, range, true) && (range < dist))
		{
			normal.mknormal(p[0], p[1], p[2]);
			dist = range;
			intersect = TRUE;
		}
	}
	return intersect;
}

// Fill Vertices
void CSkeletonX::_FillVerticesSoft1W(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, u16* indices, CBoneData::FacesVec& faces)
{
	VERIFY(*Vertices1W);
	for (CBoneData::FacesVecIt it = faces.begin( ); it != faces.end( ); it++)
	{
		fVector3			p[3];
		u32 idx = (*it) * 3;
		CSkeletonWallmark::WMFace F;
		for (u32 k = 0; k < 3; k++)
		{
			vertBoned1W& vert = Vertices1W[indices[idx + k]];
			F.bone_id[k][0] = (u16)vert.matrix;
			F.bone_id[k][1] = F.bone_id[k][0];
			F.weight[k] = 0.f;
			const fMatrix4x4& xform = Parent->LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform;
			F.vert[k].set(vert.P);
			xform.transform_tiny(p[k], F.vert[k]);
		}

		fVector3 test_normal;
		test_normal.mknormal(p[0], p[1], p[2]);
		f32 cosa = test_normal.dotproduct(normal);
		if (cosa < EPSILON_5)
		{
			continue;
		}

		if (CDB::TestSphereTri(wm.ContactPoint( ), size, p))
		{
			fVector3				UV;
			for (u32 k = 0; k < 3; k++)
			{
				fVector2& uv = F.uv[k];
				view.transform_tiny(UV, p[k]);
				uv.x = (1 + UV.x) * .5f;
				uv.y = (1 - UV.y) * .5f;
			}

			wm.m_Faces.push_back(F);
		}
	}
}
void CSkeletonX::_FillVerticesSoft2W(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, u16* indices, CBoneData::FacesVec& faces)
{
	VERIFY(*Vertices2W);
	for (CBoneData::FacesVecIt it = faces.begin( ); it != faces.end( ); it++)
	{
		fVector3			p[3];
		u32 idx = (*it) * 3;
		CSkeletonWallmark::WMFace F;
		for (u32 k = 0; k < 3; k++)
		{
			fVector3		P0;
			fVector3		P1;
			vertBoned2W& vert = Vertices2W[indices[idx + k]];
			F.bone_id[k][0] = vert.matrix0;
			F.bone_id[k][1] = vert.matrix1;
			F.weight[k] = vert.w;
			fMatrix4x4& xform0 = Parent->LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform;
			fMatrix4x4& xform1 = Parent->LL_GetBoneInstance(F.bone_id[k][1]).mRenderTransform;
			F.vert[k].set(vert.P);
			xform0.transform_tiny(P0, F.vert[k]);
			xform1.transform_tiny(P1, F.vert[k]);
			p[k].lerp(P0, P1, F.weight[k]);
		}

		fVector3 test_normal;
		test_normal.mknormal(p[0], p[1], p[2]);
		f32 cosa = test_normal.dotproduct(normal);
		if (cosa < EPSILON_5)
		{
			continue;
		}

		if (CDB::TestSphereTri(wm.ContactPoint( ), size, p))
		{
			fVector3				UV;
			for (u32 k = 0; k < 3; k++)
			{
				fVector2& uv = F.uv[k];
				view.transform_tiny(UV, p[k]);
				uv.x = (1 + UV.x) * .5f;
				uv.y = (1 - UV.y) * .5f;
			}

			wm.m_Faces.push_back(F);
		}
	}
}
