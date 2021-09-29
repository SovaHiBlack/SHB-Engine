//	Module 		: agent_corpse_manager.h
//	Description : Agent corpse manager

#pragma once

#include "MemberCorpse.h"

class CAgentManager;
class CMemberOrder;

class CAgentCorpseManager
{
public:
	using MemberCorpsesVec = xr_vector<CMemberCorpse>;

private:
	MemberCorpsesVec			m_corpses;
	CAgentManager* m_object;

protected:
	inline		CAgentManager& object( ) const;
	bool			process_corpse(CMemberOrder& member);

public:
	inline						CAgentCorpseManager(CAgentManager* object);
	inline		void			register_corpse(CStalker* corpse);
	inline		MemberCorpsesVec& corpses( );
	inline		void			clear( );
	void			react_on_member_death( );
	void			update( );
	void			remove_links(CObject* object);
};

#include "agent_corpse_manager_inline.h"
