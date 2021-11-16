// Blender_Screen_GRAY.h: interface for the CBlender_Screen_GRAY class.

#pragma once

class CBlender_Screen_GRAY : public IBlender  
{
public:
	virtual		const char* getComment()	{ return "INTERNAL: gray-scale effect"; }
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }
	
	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, unsigned short version);
	
	virtual		void		Compile			(CBlender_Compile& C);
	
	CBlender_Screen_GRAY();
	virtual ~CBlender_Screen_GRAY();
};
