#pragma once

class CBlender_deffer_flat : public IBlender  
{
public:
	virtual		const char* getComment()	{ return "LEVEL: defer-base-normal";	}
	virtual		BOOL		canBeDetailed()	{ return TRUE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Save			(IWriter&	fs);
	virtual		void		Load			(IReader&	fs, unsigned short version);
	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_deffer_flat();
	virtual ~CBlender_deffer_flat();
};
