#pragma once

#include "xr_collide_form.h"
#include "xr_collide_defs.h"

// refs
class ENGINE_API	ISpatial;
class ENGINE_API	ICollisionForm;
class ENGINE_API	CObject;

//-----------------------------------------------------------------------------------------------------------
//Space Area
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API						CObjectSpace
{
private:
	// Debug
	xrCriticalSection					Lock;
	CDB::MODEL							Static;
	fBox3								m_BoundingVolume;
	xrXRC								xrc;				// MT: dangerous
	collide::rq_results					r_temp;				// MT: dangerous
	xr_vector<ISpatial*>				r_spatial;			// MT: dangerous

public:

#ifdef DEBUG
	ref_shader							sh_debug;
	clQueryCollision					q_debug;			// MT: dangerous
	xr_vector<std::pair<fSphere, u32> >	dbg_S;				// MT: dangerous
#endif

private:
	BOOL								_RayTest(const fVector3& start, const fVector3& dir, f32 range, collide::rq_target tgt, collide::ray_cache* cache, CObject* ignore_object);
	BOOL								_RayPick(const fVector3& start, const fVector3& dir, f32 range, collide::rq_target tgt, collide::rq_result& R, CObject* ignore_object);
	BOOL								_RayQuery(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb, CObject* ignore_object);
	BOOL								_RayQuery2(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb, CObject* ignore_object);
	BOOL								_RayQuery3(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb, CObject* ignore_object);

public:
	CObjectSpace( );
	~CObjectSpace( );

	void								Load( );

	// Occluded/No
	BOOL								RayTest(const fVector3& start, const fVector3& dir, f32 range, collide::rq_target tgt, collide::ray_cache* cache, CObject* ignore_object);

	// Game raypick (nearest) - returns object and addititional params
	BOOL								RayPick(const fVector3& start, const fVector3& dir, f32 range, collide::rq_target tgt, collide::rq_result& R, CObject* ignore_object);

	// General collision query
	BOOL								RayQuery(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb, CObject* ignore_object);
	BOOL								RayQuery(collide::rq_results& dest, ICollisionForm* target, const collide::ray_defs& rq);
	// void								BoxQuery			( collide::rq_results& dest, const fBox3& B, const fMatrix4x4& M, u32 flags=clGET_TRIS|clGET_BOXES|clQUERY_STATIC|clQUERY_DYNAMIC);

	s32									GetNearest(xr_vector<CObject*>& q_nearest, ICollisionForm* obj, f32 range);
	s32									GetNearest(xr_vector<CObject*>& q_nearest, const fVector3& point, f32 range, CObject* ignore_object);

	CDB::TRI* GetStaticTris( )
	{
		return Static.get_tris( );
	}
	fVector3* GetStaticVerts( )
	{
		return Static.get_verts( );
	}
	CDB::MODEL* GetStaticModel( )
	{
		return &Static;
	}

	const fBox3& GetBoundingVolume( )
	{
		return m_BoundingVolume;
	}

// Debugging
#ifdef DEBUG
	void								dbgRender( );
	ref_shader							dbgGetShader( )
	{
		return sh_debug;
	}
#endif

};
