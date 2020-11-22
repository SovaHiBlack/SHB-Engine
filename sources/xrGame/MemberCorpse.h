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
	IC					CMemberCorpse(CStalker* corpse, CStalker* reactor, u32 time);
	IC	bool			operator==		(CStalker* corpse) const;
	IC	void			reactor(CStalker* reactor);

	IC	CStalker* corpse( ) const;
	IC	CStalker* reactor( ) const;
	IC	u32				time( ) const;
};

#include "MemberCorpse_inline.h"
