//	Module		: MemberCorpse.h
//	Description	: Member corpse class

#pragma once

class CStalker;

class CMemberCorpse
{
private:
	CStalker* m_corpse;
	CStalker* m_reactor;
	u32					m_time;

public:
	inline					CMemberCorpse(CStalker* corpse, CStalker* reactor, u32 time);
	inline	bool			operator==		(CStalker* corpse) const;
	inline	void			reactor(CStalker* reactor);

	inline	CStalker* corpse( ) const;
	inline	CStalker* reactor( ) const;
	inline	u32				time( ) const;
};

#include "MemberCorpse_inline.h"
