// Blender_Vertex_aref.h: interface for the CBlender_Vertex_aref class.

#pragma once

class CBlender_Vertex_aref : public IBlender  
{
public:
	xrP_Integer	oAREF;
	xrP_BOOL	oBlend;

	virtual		const char* getComment()	{ return "LEVEL: diffuse*base.aref";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&	fs);
	virtual		void		Load			(IReader&	fs, unsigned short version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Vertex_aref();
	virtual ~CBlender_Vertex_aref();
};
