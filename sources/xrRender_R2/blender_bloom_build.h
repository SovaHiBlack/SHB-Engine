#pragma once

class CBlender_bloom_build : public IBlender  
{
public:
	virtual		const char* getComment()	{ return "INTERNAL: combine to bloom target";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_bloom_build();
	virtual ~CBlender_bloom_build();
};
