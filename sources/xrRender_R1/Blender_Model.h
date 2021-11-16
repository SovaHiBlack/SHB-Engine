// Blender_Model.h: interface for the Blender_Screen_SET class.

#pragma once

class CBlender_Model : public IBlender
{
public:
	xrP_Integer	oAREF;
	xrP_BOOL	oBlend;

	virtual		const char* getComment()	{ return "MODEL: Default"; }
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }
	
	virtual		void		Save			(IWriter&	fs);
	virtual		void		Load			(IReader&	fs, unsigned short version);
	
	virtual		void		Compile			(CBlender_Compile& C);
	
	CBlender_Model();
	virtual ~CBlender_Model();
};
