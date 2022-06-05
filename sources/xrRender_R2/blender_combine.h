#pragma once

class CBlender_combine : public IBlender  
{
public:
	virtual		pcstr		getComment()	{ return "INTERNAL: combiner";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_combine();
	virtual ~CBlender_combine();
};
