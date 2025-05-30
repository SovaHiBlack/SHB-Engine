// SkeletonX.h: interface for the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "SkeletonCustom.h"

// refs
class ENGINE_API	CKinematics;
class CVisual;

#pragma pack(push,4)
struct vertBoned1W			// (3+3+3+3+2+1)*4 = 15*4 = 60 bytes
{
	fVector3	P;
	fVector3	N;
	fVector3	T;
	fVector3	B;
	f32			u;
	f32			v;
	u32			matrix;
	void	get_pos(fVector3& p)
	{
		p.set(P);
	}
};
struct vertBoned2W			// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	u16			matrix0;
	u16			matrix1;
	fVector3	P;
	fVector3	N;
	fVector3	T;
	fVector3	B;
	f32			w;
	f32			u;
	f32			v;
	void	get_pos(fVector3& p)
	{
		p.set(P);
	}
};
struct vertRender			// T&B are not skinned, because in R2 skinning occurs always in hardware
{
	fVector3	P;
	fVector3	N;
	f32			u;
	f32			v;
};
#pragma pack(pop)

struct SEnumVerticesCallback;
class ENGINE_API	CSkeletonX
{
protected:
	enum
	{
		vertRenderFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
	};

	enum
	{
		RM_SKINNING_SOFT,
		RM_SINGLE,
		RM_SKINNING_1B,
		RM_SKINNING_2B
	};

	CKinematics* Parent;		// setted up by parent
	ref_smem<vertBoned1W>	Vertices1W;		// shared
	ref_smem<vertBoned2W>	Vertices2W;		// shared
	ref_smem<u16>			BonesUsed;		// actual bones which have influence on vertices

	u16						RenderMode;
	u16						ChildIDX;

	// render-mode specifics
	union
	{
		struct
		{			// soft-skinning only
			u32				cache_DiscardID;
			u32				cache_vCount;
			u32				cache_vOffset;
		};
		u32					RMS_boneid;			// single-bone-rendering
		u32					RMS_bonecount;		// skinning, maximal bone ID
	};

	void					_Copy(CSkeletonX* V);
	void					_Render_soft(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount);
	void					_Render(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount);
	void					_Load(pcstr N, IReader* data, u32& dwVertCount);

	virtual void			_Load_hw(CVisual& V, pvoid data) = 0;
	virtual void			_CollectBoneFaces(CVisual* V, u32 iBase, u32 iCount) = 0;

	void					_FillVerticesSoft1W(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, u16* indices, CBoneData::FacesVec& faces);
	void					_FillVerticesSoft2W(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, u16* indices, CBoneData::FacesVec& faces);
	virtual void			_FillVerticesHW1W(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, CVisual* V, u16* indices, CBoneData::FacesVec& faces) = 0;
	virtual void			_FillVerticesHW2W(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, CVisual* V, u16* indices, CBoneData::FacesVec& faces) = 0;
	virtual void			_FillVertices(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, CVisual* V, u16 bone_id, u32 iBase, u32 iCount) = 0;

	BOOL					_PickBoneSoft1W(fVector3& normal, f32& range, const fVector3& S, const fVector3& D, u16* indices, CBoneData::FacesVec& faces);
	BOOL					_PickBoneSoft2W(fVector3& normal, f32& range, const fVector3& S, const fVector3& D, u16* indices, CBoneData::FacesVec& faces);
	virtual BOOL			_PickBoneHW1W(fVector3& normal, f32& range, const fVector3& S, const fVector3& D, CVisual* V, u16* indices, CBoneData::FacesVec& faces) = 0;
	virtual BOOL			_PickBoneHW2W(fVector3& normal, f32& range, const fVector3& S, const fVector3& D, CVisual* V, u16* indices, CBoneData::FacesVec& faces) = 0;
	virtual BOOL			_PickBone(fVector3& normal, f32& range, const fVector3& S, const fVector3& D, CVisual* V, u16 bone_id, u32 iBase, u32 iCount) = 0;

public:
	BOOL					has_visible_bones( );
	CSkeletonX( )
	{
		Parent = 0;
		ChildIDX = u16(-1);
	}

	virtual void			SetParent(CKinematics* K)
	{
		Parent = K;
	}
	virtual void			AfterLoad(CKinematics* parent, u16 child_idx) = 0;
	virtual void			EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id) = 0;
	virtual BOOL			PickBone(fVector3& normal, f32& dist, const fVector3& start, const fVector3& dir, u16 bone_id) = 0;
	virtual void			FillVertices(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, u16 bone_id) = 0;
};
