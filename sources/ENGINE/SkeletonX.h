// SkeletonX.h: interface for the CSkeletonX class.
#pragma once

#include "SkeletonCustom.h"

// refs
class ENGINE_API CKinematics;
class Fvisual;

#pragma pack(push, 4)
struct vertBoned1W		// (3+3+3+3+2+1)*4 = 15*4 = 60 bytes
{
	Fvector3				P;
	Fvector3				N;
	Fvector3				T;
	Fvector3				B;
	float				u;
	float				v;
	u32					matrix;
	void	get_pos		(Fvector3& p)
	{
		p.set(P);
	}
};

struct vertBoned2W		// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	unsigned short					matrix0;
	unsigned short					matrix1;
	Fvector3				P;
	Fvector3				N;
	Fvector3				T;
	Fvector3				B;
	float				w;
	float				u;
	float				v;
	void	get_pos		(Fvector3& p)
	{
		p.set(P);
	}
};

struct vertRender		// T&B are not skinned, because in R2 skinning occurs always in hardware
{
	Fvector3				P;
	Fvector3				N;
	float				u;
	float				v;
};
#pragma pack(pop)

struct SEnumVerticesCallback;

class ENGINE_API CSkeletonX
{
protected:
	enum
	{
		vertRenderFVF													= D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
	};
	enum
	{
		RM_SKINNING_SOFT,
		RM_SINGLE,
		RM_SKINNING_1B,
		RM_SKINNING_2B
	};

	CKinematics*									Parent;				// setted up by parent
	ref_smem<vertBoned1W>							Vertices1W;			// shared
	ref_smem<vertBoned2W>							Vertices2W;			// shared
	ref_smem<unsigned short>									BonesUsed;			// actual bones which have influence on vertices

	unsigned short												RenderMode;
	unsigned short												ChildIDX;

	// render-mode specifics
	union
	{
		struct
		{	// soft-skinning only
			u32										cache_DiscardID;
			u32										cache_vCount;
			u32										cache_vOffset;
		};
		u32											RMS_boneid;			// single-bone-rendering
		u32											RMS_bonecount;		// skinning, maximal bone ID
	};

	void					_Copy					(CSkeletonX* V);
	void					_Render_soft			(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount);
	void					_Render					(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount);
	void					_Load					(const char* N, IReader* data, u32& dwVertCount);

	virtual void			_Load_hw				(Fvisual& V, void* data) = 0;
	virtual void			_CollectBoneFaces		(Fvisual* V, u32 iBase, u32 iCount) = 0;

	void					_FillVerticesSoft1W		(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, float size, unsigned short* indices, CBoneData::FacesVec& faces);
	void					_FillVerticesSoft2W		(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, float size, unsigned short* indices, CBoneData::FacesVec& faces);
	virtual void			_FillVerticesHW1W		(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, float size, Fvisual* V, unsigned short* indices, CBoneData::FacesVec& faces) = 0;
	virtual void			_FillVerticesHW2W		(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, float size, Fvisual* V, unsigned short* indices, CBoneData::FacesVec& faces) = 0;
	virtual void			_FillVertices			(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, float size, Fvisual* V, unsigned short bone_id, u32 iBase, u32 iCount) = 0;

	BOOL					_PickBoneSoft1W			(Fvector3& normal, float& range, const Fvector3& S, const Fvector3& D, unsigned short* indices, CBoneData::FacesVec& faces);
	BOOL					_PickBoneSoft2W			(Fvector3& normal, float& range, const Fvector3& S, const Fvector3& D, unsigned short* indices, CBoneData::FacesVec& faces);
	virtual BOOL			_PickBoneHW1W			(Fvector3& normal, float& range, const Fvector3& S, const Fvector3& D, Fvisual* V, unsigned short* indices, CBoneData::FacesVec& faces) = 0;
	virtual BOOL			_PickBoneHW2W			(Fvector3& normal, float& range, const Fvector3& S, const Fvector3& D, Fvisual* V, unsigned short* indices, CBoneData::FacesVec& faces) = 0;
	virtual BOOL			_PickBone				(Fvector3& normal, float& range, const Fvector3& S, const Fvector3& D, Fvisual* V, unsigned short bone_id, u32 iBase, u32 iCount) = 0;

public:
	BOOL					has_visible_bones		( );
							CSkeletonX				( )
	{
		Parent = 0;
		ChildIDX = unsigned short(-1);
	}

	virtual void			SetParent				(CKinematics* K)
	{
		Parent = K;
	}
	virtual void			AfterLoad				(CKinematics* parent, unsigned short child_idx) = 0;
	virtual void			EnumBoneVertices		(SEnumVerticesCallback& C, unsigned short bone_id) = 0;
	virtual BOOL			PickBone				(Fvector3& normal, float& dist, const Fvector3& start, const Fvector3& dir, unsigned short bone_id) = 0;
	virtual void			FillVertices			(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, float size, unsigned short bone_id) = 0;
};
