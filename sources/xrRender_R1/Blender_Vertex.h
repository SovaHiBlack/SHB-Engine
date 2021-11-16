// Blender_Vertex.h: interface for the CBlender_Vertex class.

#pragma once

class CBlender_Vertex : public IBlender  
{
public:
	virtual		const char* getComment()	{ return "LEVEL: diffuse*base";	}
	virtual		BOOL		canBeDetailed()	{ return TRUE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, unsigned short version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Vertex();
	virtual ~CBlender_Vertex();
};
