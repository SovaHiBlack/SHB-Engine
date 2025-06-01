////////////////////////////////////////////////////////////////////////////
//	Module 		: member_corpse_inline.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Member corpse inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CMemberCorpse::CMemberCorpse(CStalker* corpse, CStalker* reactor, u32 time)
{
	m_corpse = corpse;
	m_reactor = reactor;
	m_time = time;
}

IC	bool CMemberCorpse::operator==		(CStalker* corpse) const
{
	return		(m_corpse == corpse);
}

IC	void CMemberCorpse::reactor(CStalker* reactor)
{
	m_reactor = reactor;
}

IC	CStalker* CMemberCorpse::corpse( ) const
{
	return		(m_corpse);
}

IC	CStalker* CMemberCorpse::reactor( ) const
{
	return		(m_reactor);
}

IC	u32	CMemberCorpse::time( ) const
{
	return		(m_time);
}
