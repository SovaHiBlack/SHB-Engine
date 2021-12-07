#pragma once

#include "vis_common.h"

#define VLOAD_NOVERTICES		(1<<0)
#define VLOAD_NOINDICES			(1<<1)
#define VLOAD_FORCESOFTWARE		(1<<2)

// The class itself
class	ENGINE_API				CKinematicsAnimated;
class	ENGINE_API				CKinematics;
class	ENGINE_API				IParticleCustom;

struct	ENGINE_API				IRender_Mesh
{
	// format
	ref_geom					rm_geom;

	// verts
	IDirect3DVertexBuffer9* p_rm_Vertices;
	unsigned int							vBase;
	unsigned int							vCount;

	// indices
	IDirect3DIndexBuffer9* p_rm_Indices;
	unsigned int							iBase;
	unsigned int							iCount;
	unsigned int							dwPrimitives;

	IRender_Mesh( )
	{
		p_rm_Vertices = 0;
		p_rm_Indices = 0;
	}
	virtual ~IRender_Mesh( );
private:
	IRender_Mesh(const IRender_Mesh& other);
	void	operator=			(const IRender_Mesh& other);
};

// The class itself
class	ENGINE_API				IRender_Visual
{
public:

#ifdef DEBUG
	CSharedString					dbg_name;
#endif // DEBUG

public:
	// Common data for rendering
	unsigned int							Type;				// visual's type
	vis_data					vis;				// visibility-data
	ref_shader					shader;				// pipe state, shared

	virtual void				Render(float LOD)
	{ }		// LOD - Level Of Detail  [0..1], Ignored
	virtual void				Load(const char* N, IReader* data, unsigned int dwFlags);
	virtual void				Release( );						// Shared memory release
	virtual void				Copy(IRender_Visual* from);
	virtual void				Spawn( )
	{ }
	virtual void				Depart( )
	{ }

	virtual	CKinematics* dcast_PKinematics( )
	{
		return 0;
	}
	virtual	CKinematicsAnimated* dcast_PKinematicsAnimated( )
	{
		return 0;
	}
	virtual IParticleCustom* dcast_ParticleCustom( )
	{
		return 0;
	}

	IRender_Visual( );
	virtual ~IRender_Visual( );
};
