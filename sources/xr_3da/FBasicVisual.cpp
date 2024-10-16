// IRender_Visual.cpp: implementation of the IRender_Visual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "render.h"

#include "fbasicvisual.h"
#include "fmesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IRender_Mesh::~IRender_Mesh()		
{ 
	_RELEASE(p_rm_Vertices); 
	_RELEASE(p_rm_Indices);		
}

IRender_Visual::IRender_Visual		()
{
	Type				= 0;
	shader				= 0;
	vis.clear			();
}

IRender_Visual::~IRender_Visual		()
{
}

void IRender_Visual::Release		()
{
}

CStatTimer						tscreate;

void IRender_Visual::Load		(pcstr N, IReader *data, u32 )
{

#ifdef DEBUG
	dbg_name	= N;
#endif // DEBUG

	// header
	VERIFY		(data);
	ogf_header	hdr;
	if (data->r_chunk_safe(OGF_HEADER,&hdr,sizeof(hdr)))
	{
		R_ASSERT2			(hdr.format_version==xrOGF_FormatVersion, "Invalid visual version");
		Type				= hdr.type;
		if (hdr.shader_id)	shader	= ::Render->getShader	(hdr.shader_id);
		vis.box.set			(hdr.bb.min,hdr.bb.max	);
		vis.sphere.set		(hdr.bs.c,	hdr.bs.r	);
	} else {
		FATAL				("Invalid visual");
	}

	// Shader
	if (data->find_chunk(OGF_TEXTURE)) {
		string256		fnT,fnS;
		data->r_stringZ	(fnT,sizeof(fnT));
		data->r_stringZ	(fnS,sizeof(fnS));
		shader.create	(fnS,fnT);
	}
}

#define PCOPY(a)	a = pFrom->a
void	IRender_Visual::Copy(IRender_Visual *pFrom)
{
	PCOPY(Type);
	PCOPY(shader);
	PCOPY(vis);

#ifdef DEBUG
	PCOPY(dbg_name);
#endif // DEBUG

}
