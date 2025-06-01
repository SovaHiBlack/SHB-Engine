////////////////////////////////////////////////////////////////////////////
//	Module 		: member_corpse.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Member corpse class
////////////////////////////////////////////////////////////////////////////

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

public:
	IC	CStalker* corpse( ) const;
	IC	CStalker* reactor( ) const;
	IC	u32				time( ) const;
};

#include "member_corpse_inline.h"
