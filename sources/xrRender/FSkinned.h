// SkeletonX.h: interface for the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Visual.h"
#include "Progressive.h"
#include "..\XR_3DA\SkeletonX.h"

struct	SEnumVerticesCallback;
class		CSkeletonX_ext	: public CSkeletonX	// shared code for SkeletonX derivates
{
protected:
	virtual void			_Load_hw			(CVisual& V,		void *		data);
	virtual void			_CollectBoneFaces	(CVisual* V,		u32 iBase,	u32 iCount);
			void			_EnumBoneVertices	( SEnumVerticesCallback &C, CVisual* V, u16 bone_id, u32 iBase, u32 iCount ) const;
	virtual void			_FillVerticesHW1W	(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, CVisual* V, u16* indices, CBoneData::FacesVec& faces);
	virtual void			_FillVerticesHW2W	(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, CVisual* V, u16* indices, CBoneData::FacesVec& faces);
	virtual void			_FillVertices		(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, CVisual* V, u16 bone_id, u32 iBase, u32 iCount);

	virtual BOOL			_PickBoneHW1W		(fVector3& normal, f32& range, const fVector3& S, const fVector3& D, CVisual* V, u16* indices, CBoneData::FacesVec& faces);
	virtual BOOL			_PickBoneHW2W		(fVector3& normal, f32& range, const fVector3& S, const fVector3& D, CVisual* V, u16* indices, CBoneData::FacesVec& faces);
	virtual BOOL			_PickBone			(fVector3& normal, f32& range, const fVector3& S, const fVector3& D, CVisual* V, u16 bone_id, u32 iBase, u32 iCount);
};

class CSkeletonX_ST			: public CVisual, public CSkeletonX_ext
{
private:
	typedef CVisual			inherited1;
	typedef CSkeletonX_ext	inherited2;
public:
							CSkeletonX_ST	()	{}
	virtual					~CSkeletonX_ST	()	{}
	virtual void			Render			(f32 LOD);
	virtual void			Load			(pcstr N, IReader *data, u32 dwFlags);
	virtual void			Copy(IRenderVisual* pFrom);
	virtual void			Release			();
	virtual void			AfterLoad		(CKinematics* parent, u16 child_idx);
	virtual void			EnumBoneVertices(SEnumVerticesCallback &C, u16 bone_id);
	virtual BOOL			PickBone		(fVector3& normal, f32& dist, const fVector3& start, const fVector3& dir, u16 bone_id);
	virtual void			FillVertices	(const fMatrix4x4& view, CSkeletonWallmark& wm, const fVector3& normal, f32 size, u16 bone_id);
private:
	CSkeletonX_ST				(const CSkeletonX_ST& other);
	void	operator=			( const CSkeletonX_ST& other);
};

class CSkeletonX_PM : public CProgressive, public CSkeletonX_ext
{
private:
	typedef CProgressive	inherited1;
	typedef CSkeletonX_ext	inherited2;
public:
							CSkeletonX_PM	()	{}
	virtual					~CSkeletonX_PM	()	{}
	virtual void			Render			(f32 LOD);
	virtual void			Load			(pcstr N, IReader *data, u32 dwFlags);
	virtual void			Copy(IRenderVisual* pFrom);
	virtual void			Release			();
	virtual void			AfterLoad		(CKinematics* parent, u16 child_idx);
	virtual void			EnumBoneVertices(SEnumVerticesCallback &C, u16 bone_id);
	virtual BOOL			PickBone		(Fvector& normal, f32& dist, const Fvector& start, const Fvector& dir, u16 bone_id);
	virtual void			FillVertices	(const fMatrix4x4& view, CSkeletonWallmark& wm, const Fvector& normal, f32 size, u16 bone_id);
private:
	CSkeletonX_PM				(const CSkeletonX_PM& other);
	void	operator=			( const CSkeletonX_PM& other);
};
