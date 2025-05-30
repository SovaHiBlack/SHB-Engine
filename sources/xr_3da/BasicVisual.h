#pragma once

#include "vis_common.h"

#define VLOAD_NOVERTICES		(1<<0)
#define VLOAD_NOINDICES			(1<<1)
#define VLOAD_FORCESOFTWARE		(1<<2)

// The class itself
class ENGINE_API				CKinematicsAnimated;
class ENGINE_API				CKinematics;
class ENGINE_API				IParticleCustom;

struct ENGINE_API				IRenderMesh
{
	// format
	ref_geom					rm_geom;

	// verts
	IDirect3DVertexBuffer9* p_rm_Vertices;
	u32							vBase;
	u32							vCount;

	// indices
	IDirect3DIndexBuffer9* p_rm_Indices;
	u32							iBase;
	u32							iCount;
	u32							dwPrimitives;

	IRenderMesh( )
	{
		p_rm_Vertices = 0;
		p_rm_Indices = 0;
	}
	virtual ~IRenderMesh( );

private:
	IRenderMesh(const IRenderMesh& other);
	void	operator=			(const IRenderMesh& other);
};

// The class itself
class ENGINE_API				IRenderVisual
{
public:

#ifdef DEBUG
	shared_str					dbg_name;
#endif // DEBUG

public:
	// Common data for rendering
	u32							Type;				// visual's type
	vis_data					vis;				// visibility-data
	ref_shader					shader;				// pipe state, shared

	virtual void				Render(f32 LOD)
	{ }		// LOD - Level Of Detail  [0..1], Ignored
	virtual void				Load(pcstr N, IReader* data, u32 dwFlags);
	virtual void				Release( );						// Shared memory release
	virtual void				Copy(IRenderVisual* from);
	virtual void				Spawn( )
	{ }
	virtual void				Depart( )
	{ }

	virtual CKinematics* dcast_PKinematics( )
	{
		return 0;
	}
	virtual CKinematicsAnimated* dcast_PKinematicsAnimated( )
	{
		return 0;
	}
	virtual IParticleCustom* dcast_ParticleCustom( )
	{
		return 0;
	}

	IRenderVisual( );
	virtual ~IRenderVisual( );
};
