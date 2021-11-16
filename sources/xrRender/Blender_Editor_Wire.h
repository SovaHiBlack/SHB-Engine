#pragma once

class CBlender_Editor_Wire : public IBlender  
{
	string64	oT_Factor;
public:
	virtual		const char* getComment()	{ return "EDITOR: wire";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, unsigned short version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Editor_Wire();
	virtual ~CBlender_Editor_Wire();
};
