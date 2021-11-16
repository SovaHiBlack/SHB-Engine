#pragma once

class CBlender_Detail_Still : public IBlender
{
public:
	xrP_BOOL			oBlend;

	virtual const char*	getComment				( )
	{
		return "LEVEL: detail objects";
	}
	virtual BOOL		canBeLMAPped			( )
	{
		return FALSE;
	}

	virtual void		Save					(IWriter& fs);
	virtual void		Load					(IReader& fs, unsigned short version);

	virtual void		Compile					(CBlender_Compile& C);

						CBlender_Detail_Still	( );
	virtual				~CBlender_Detail_Still	( );
};
