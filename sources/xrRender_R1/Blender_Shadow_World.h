#pragma once

class CBlender_ShWorld : public IBlender
{
public:
	virtual		const char* getComment()	{ return "INTERNAL: shadow projecting";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, unsigned short version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_ShWorld();
	virtual ~CBlender_ShWorld();
};

//BLENDER_SHADOW_WORLD_H
