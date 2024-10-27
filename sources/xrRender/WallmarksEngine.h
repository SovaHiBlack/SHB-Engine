// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

namespace WallmarksEngine {
	struct wm_slot;
}

class CWallmarksEngine
{
public:
	typedef WallmarksEngine::wm_slot	wm_slot;

public:
	struct static_wallmark 
	{
		Fsphere				bounds;
		xr_vector<FVF::LIT>	verts;
		f32				ttl;
	};
	DEFINE_VECTOR		(static_wallmark*,StaticWMVec,StaticWMVecIt);
	DEFINE_VECTOR		(wm_slot*,WMSlotVec,WMSlotVecIt);
private:
	StaticWMVec			static_pool;
	WMSlotVec			marks;
	ref_geom			hGeom;

	fVector3				sml_normal;
	CFrustum			sml_clipper;
	sPoly				sml_poly_dest;
	sPoly				sml_poly_src;

	xrXRC				xrc;
	CDB::Collector		sml_collector;
	xr_vector<u32>		sml_adjacency;

	xrCriticalSection	lock;
private:
	wm_slot*			FindSlot				(ref_shader shader);
	wm_slot*			AppendSlot				(ref_shader shader);
private:
	void				BuildMatrix				(fMatrix4x4& dest, f32 invsz, const fVector3& from);
	void				RecurseTri				(u32 T, fMatrix4x4& mView, static_wallmark	&W);
	void				AddWallmark_internal	(CDB::TRI* pTri, const fVector3* pVerts, const fVector3& contact_point, ref_shader hTexture, f32 sz);

	static_wallmark*	static_wm_allocate		();
	void				static_wm_render		(static_wallmark*	W, FVF::LIT* &V);
	void				static_wm_destroy		(static_wallmark*	W	);

public:
						CWallmarksEngine		();
						~CWallmarksEngine		();
	// edit wallmarks
	void				AddStaticWallmark		(CDB::TRI* pTri, const fVector3* pVerts, const fVector3& contact_point, ref_shader hTexture, f32 sz);
	void				AddSkeletonWallmark		(intrusive_ptr<CSkeletonWallmark> wm);
	void				AddSkeletonWallmark		(const fMatrix4x4* xf, CKinematics* obj, ref_shader& sh, const fVector3& start, const fVector3& dir, f32 size);

	// render
	void				Render					();

	void				clear					();
};
