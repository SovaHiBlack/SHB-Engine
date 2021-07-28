#pragma once

#include "fhierrarhyvisual.h"
#include "bone.h"

// consts
const u32					MAX_BONE_PARAMS = 4;
const u32					UCalc_Interval = 100;	// 10 fps
extern xrCriticalSection	UCalc_Mutex;

// refs
class ENGINE_API CKinematics;
class ENGINE_API CIniFile;
class ENGINE_API CBoneData;
class ENGINE_API CBoneInstance;
struct SEnumVerticesCallback;

// t-defs
typedef xr_vector<CBoneData*>		vecBones;
typedef vecBones::iterator			vecBonesIt;

// callback
typedef void (*BoneCallback)		(CBoneInstance* P);
typedef void (*UpdateCallback)		(CKinematics* P);

// MT-locker
struct UCalc_mtlock
{
	UCalc_mtlock( )
	{
		UCalc_Mutex.Enter( );
	}
	~UCalc_mtlock( )
	{
		UCalc_Mutex.Leave( );
	}
};

//*** Bone Instance *******************************************************************************
#pragma pack(push,8)
class ENGINE_API		CBoneInstance
{
public:
	// data
	Fmatrix				mTransform;							// final x-form matrix (local to model)
	Fmatrix				mRenderTransform;					// final x-form matrix (model_base -> bone -> model)
	BoneCallback		Callback;
	void* Callback_Param;
	BOOL				Callback_overwrite;					// performance hint - don't calc anims
	float				param[MAX_BONE_PARAMS];	// 
	u32					Callback_type;						//
	// methods
	void				construct( );
	void				set_callback(u32 Type, BoneCallback C, void* Param, BOOL overwrite = FALSE);
	void				reset_callback( );
	void				set_param(u32 idx, float data);
	float				get_param(u32 idx);

	u32					mem_usage( )
	{
		return sizeof(*this);
	}
};
#pragma pack(pop)

//*** Shared Bone Data ****************************************************************************
class ENGINE_API		CBoneData
{
protected:
	U16					SelfID;
	U16					ParentID;

public:
	shared_str			name;

	vecBones			children;		// bones which are slaves to this
	Fobb				obb;

	Fmatrix				bind_transform;
	Fmatrix				m2b_transform;	// model to bone conversion transform
	SBoneShape			shape;
	shared_str			game_mtl_name;
	U16					game_mtl_idx;
	SJointIKData		IK_data;
	float				mass;
	Fvector3				center_of_mass;

//	DEFINE_VECTOR(U16, FacesVec, FacesVecIt);
	using FacesVec = xr_vector<U16>;
	using FacesVecIt = FacesVec::iterator;
//	DEFINE_VECTOR(FacesVec, ChildFacesVec, ChildFacesVecIt);
	using ChildFacesVec = xr_vector<FacesVec>;
	using ChildFacesVecIt = ChildFacesVec::iterator;
	ChildFacesVec		child_faces;	// shared

public:
	CBoneData(U16 ID) :SelfID(ID)
	{
		VERIFY(SelfID != BI_NONE);
	}
	virtual				~CBoneData( )
	{ }
#ifdef DEBUG
	typedef svector<int, 128>	BoneDebug;
	void						DebugQuery(BoneDebug& L);
#endif

	inline void				SetParentID(U16 id)
	{
		ParentID = id;
	}

	inline U16				GetSelfID( ) const
	{
		return SelfID;
	}
	inline U16				GetParentID( ) const
	{
		return ParentID;
	}

	// assign face
	void				AppendFace(U16 child_idx, U16 idx)
	{
		child_faces[child_idx].push_back(idx);
	}
	// Calculation
	void				CalculateM2B(const Fmatrix& Parent);

	virtual u32			mem_usage( )
	{
		u32 sz = sizeof(*this) + sizeof(vecBones::value_type) * children.size( );
		for (ChildFacesVecIt c_it = child_faces.begin( ); c_it != child_faces.end( ); c_it++)
			sz += c_it->size( ) * sizeof(FacesVec::value_type) + sizeof(*c_it);
		return			sz;
	}
};

#pragma warning(push)
#pragma warning(disable:4275)
class ENGINE_API CSkeletonWallmark : public intrusive_base // 4+4+4+12+4+16+16 = 60 + 4 = 64
{
#pragma warning(pop)
	CKinematics* m_Parent;		// 4
	const Fmatrix* m_XForm;		// 4
	ref_shader			m_Shader;		// 4
	Fvector3			m_ContactPoint;	// 12		model space
	float				m_fTimeStart;	// 4
public:
#ifdef DEBUG
	u32					used_in_render;
#endif
	Fsphere				m_LocalBounds;	// 16		model space
	struct WMFace
	{
		Fvector3		vert[3];
		Fvector2		uv[3];
		U16				bone_id[3][2];
		float			weight[3];
	};
//	DEFINE_VECTOR(WMFace, WMFacesVec, WMFacesVecIt);
	using WMFacesVec = xr_vector<WMFace>;
	using WMFacesVecIt = WMFacesVec::iterator;
	WMFacesVec			m_Faces;		// 16 

public:
	Fsphere				m_Bounds;		// 16		world space

	CSkeletonWallmark(CKinematics* p, const Fmatrix* m, ref_shader s, const Fvector3& cp, float ts) :
		m_Parent(p), m_XForm(m), m_Shader(s), m_fTimeStart(ts), m_ContactPoint(cp)
	{
#ifdef DEBUG
		used_in_render = u32(-1);
#endif

	}
	~CSkeletonWallmark( )
#ifdef DEBUG
		;
#else
	{
	}
#endif

	inline CKinematics* Parent( )
	{
		return m_Parent;
	}
	inline u32				VCount( )
	{
		return m_Faces.size( ) * 3;
	}
	inline bool				Similar(ref_shader& sh, const Fvector3& cp, float eps)
	{
		return (m_Shader == sh) && m_ContactPoint.similar(cp, eps);
	}
	inline float			TimeStart( )
	{
		return m_fTimeStart;
	}
	inline const Fmatrix* XFORM( )
	{
		return m_XForm;
	}
	inline const Fvector3& ContactPoint( )
	{
		return m_ContactPoint;
	}
	inline ref_shader		Shader( )
	{
		return m_Shader;
	}
};

//DEFINE_VECTOR(intrusive_ptr<CSkeletonWallmark>, SkeletonWMVec, SkeletonWMVecIt);
using SkeletonWMVec = xr_vector<intrusive_ptr<CSkeletonWallmark>>;
using SkeletonWMVecIt = SkeletonWMVec::iterator;

// sanity check
#ifdef DEBUG
struct dbg_marker
{
	BOOL* lock;
	dbg_marker(BOOL* b)
	{
		lock = b; VERIFY(*lock == FALSE); *lock = TRUE;
	}
	~dbg_marker( )
	{
		*lock = FALSE;
	}
};
#	define _DBG_SINGLE_USE_MARKER	dbg_marker	_dbg_marker(&dbg_single_use_marker)
#else
#	define _DBG_SINGLE_USE_MARKER
#endif

class ENGINE_API CKinematics : public FHierrarhyVisual
{
	typedef FHierrarhyVisual	inherited;
	friend class				CBoneData;
	friend class				CSkeletonX;
public:
#ifdef DEBUG
	BOOL						dbg_single_use_marker;
#endif
	virtual void				Bone_Calculate(CBoneData* bd, Fmatrix* parent);
	virtual void				OnCalculateBones( )
	{ }

	typedef xr_vector<std::pair<shared_str, U16>> accel;

	IRender_Visual* m_lod;

protected:
	SkeletonWMVec				wallmarks;
	u32							wm_frame;

	xr_vector<IRender_Visual*>	children_invisible;

	// Globals
	CIniFile* pUserData;
	CBoneInstance* bone_instances;	// bone instances
	vecBones* bones;			// all bones	(shared)
	U16							iRoot;			// Root bone index

	// Fast search
	accel* bone_map_N;		// bones  associations	(shared)	- sorted by name
	accel* bone_map_P;		// bones  associations	(shared)	- sorted by name-pointer

	BOOL						Update_Visibility;
	u32							UCalc_Time;
	int							UCalc_Visibox;

	Flags64						visimask;

	CSkeletonX* LL_GetChild(u32 idx);

	// internal functions
	virtual CBoneData* CreateBoneData(U16 ID)
	{
		return xr_new<CBoneData>(ID);
	}
	virtual void				IBoneInstances_Create( );
	virtual void				IBoneInstances_Destroy( );
	void						Visibility_Invalidate( )
	{
		Update_Visibility = TRUE;
	};
	void						Visibility_Update( );

	void						LL_Validate( );
public:
	UpdateCallback				Update_Callback;
	void* Update_Callback_Param;
public:
	// wallmarks
	void						AddWallmark(const Fmatrix* parent, const Fvector3& start, const Fvector3& dir, ref_shader shader, float size);
	void						CalculateWallmarks( );
	void						RenderWallmark(intrusive_ptr<CSkeletonWallmark> wm, FVF::LIT*& verts);
	void						ClearWallmarks( );

	bool			PickBone(const Fmatrix& parent_xform, Fvector3& normal, float& dist, const Fvector3& start, const Fvector3& dir, U16 bone_id);
	virtual		void			EnumBoneVertices(SEnumVerticesCallback& C, U16 bone_id);

	CKinematics( );
	virtual						~CKinematics( );

	// Low level interface
	U16							LL_BoneID(const char* B);
	U16							LL_BoneID(const shared_str& B);
	const char* LL_BoneName_dbg(U16 ID);

	CIniFile* LL_UserData( )
	{
		return pUserData;
	}
	accel* LL_Bones( )
	{
		return bone_map_N;
	}
	__forceinline CBoneInstance& LL_GetBoneInstance(U16 bone_id)
	{
		VERIFY(bone_id < LL_BoneCount( )); VERIFY(bone_instances); return bone_instances[bone_id];
	}
	CBoneData& LL_GetData(U16 bone_id)
	{
		VERIFY(bone_id < LL_BoneCount( )); VERIFY(bones);			return *((*bones)[bone_id]);
	}
	U16							LL_BoneCount( )
	{
		return U16(bones->size( ));
	}
	U16							LL_VisibleBoneCount( )
	{
		U64 F = visimask.flags & ((U64(1) << U64(LL_BoneCount( ))) - 1);
		return (U16) btwCount1(F);
	}
	__forceinline Fmatrix& LL_GetTransform(U16 bone_id)
	{
		return LL_GetBoneInstance(bone_id).mTransform;
	}
	__forceinline Fmatrix& LL_GetTransform_R(U16 bone_id)
	{
		return LL_GetBoneInstance(bone_id).mRenderTransform;
	}	// rendering only
	Fobb& LL_GetBox(U16 bone_id)
	{
		VERIFY(bone_id < LL_BoneCount( ));	return (*bones)[bone_id]->obb;
	}
	void						LL_GetBindTransform(xr_vector<Fmatrix>& matrices);
	int							LL_GetBoneGroups(xr_vector<xr_vector<U16>>& groups);

	U16							LL_GetBoneRoot( )
	{
		return iRoot;
	}
	void						LL_SetBoneRoot(U16 bone_id)
	{
		VERIFY(bone_id < LL_BoneCount( ));	iRoot = bone_id;
	}

	BOOL						LL_GetBoneVisible(U16 bone_id)
	{
		VERIFY(bone_id < LL_BoneCount( ));
		return visimask.is(U64(1) << bone_id);
	}
	void						LL_SetBoneVisible(U16 bone_id, BOOL val, BOOL bRecursive);
	U64							LL_GetBonesVisible( )
	{
		return visimask.get( );
	}
	void						LL_SetBonesVisible(U64 mask);

	// Main functionality
	virtual void				CalculateBones(BOOL bForceExact = FALSE);		// Recalculate skeleton
	void						CalculateBones_Invalidate( );
	void						Callback(UpdateCallback C, void* Param)
	{
		Update_Callback = C; Update_Callback_Param = Param;
	}

// debug
#ifdef DEBUG
	void						DebugRender(Fmatrix& XFORM);
#endif

	// General "Visual" stuff
	virtual void				Copy(IRender_Visual* pFrom);
	virtual void				Load(const char* N, IReader* data, u32 dwFlags);
	virtual void 				Spawn( );
	virtual void				Depart( );
	virtual void 				Release( );

	virtual	CKinematics* dcast_PKinematics( )
	{
		return this;
	}

	virtual u32					mem_usage(bool bInstance)
	{
		u32 sz = sizeof(*this);
		sz += bone_instances ? bone_instances->mem_usage( ) : 0;
		if (!bInstance)
		{
//			sz					+= pUserData?pUserData->mem_usage():0;
			for (vecBonesIt b_it = bones->begin( ); b_it != bones->end( ); b_it++)
				sz += sizeof(vecBones::value_type) + (*b_it)->mem_usage( );
		}
		return sz;
	}
};

inline CKinematics* PKinematics(IRender_Visual* V)
{
	return V ? V->dcast_PKinematics( ) : 0;
}
