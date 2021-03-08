//	Module		: MemberCorpse_inline.h
//	Description	: Member corpse inline functions

#pragma once

inline CMemberCorpse::CMemberCorpse(CStalker* corpse, CStalker* reactor, u32 time)
{
	m_corpse = corpse;
	m_reactor = reactor;
	m_time = time;
}

inline bool CMemberCorpse::operator==		(CStalker* corpse) const
{
	return (m_corpse == corpse);
}

inline void CMemberCorpse::reactor(CStalker* reactor)
{
	m_reactor = reactor;
}

inline CStalker* CMemberCorpse::corpse( ) const
{
	return m_corpse;
}

inline CStalker* CMemberCorpse::reactor( ) const
{
	return m_reactor;
}

inline u32 CMemberCorpse::time( ) const
{
	return m_time;
}
