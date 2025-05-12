////////////////////////////////////////////////////////////////////////////
//	Module 		: MemberCorpse.h
//	Description : Member corpse class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAI_Stalker;

class CMemberCorpse
{
private:
	CAI_Stalker			*m_corpse;
	CAI_Stalker			*m_reactor;
	u32					m_time;

public:
	IC					CMemberCorpse	(CAI_Stalker *corpse, CAI_Stalker *reactor, u32 time);
	IC	bool			operator==		(CAI_Stalker *corpse) const;
	IC	void			reactor			(CAI_Stalker *reactor);

public:
	IC	CAI_Stalker		*corpse			() const;
	IC	CAI_Stalker		*reactor		() const;
	IC	u32				time			() const;
};

#include "MemberCorpse_inline.h"