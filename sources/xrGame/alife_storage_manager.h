////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_storage_manager.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator storage manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_simulator_base.h"

class CNetPacket;

class CALifeStorageManager : public virtual CALifeSimulatorBase
{
	friend class CALifeUpdatePredicate;

protected:
	typedef CALifeSimulatorBase inherited;

protected:
	string_path		m_save_name;
	pcstr			m_section;

private:
	void	prepare_objects_for_save( );
	void	load(pvoid buffer, const u32& buffer_size, pcstr file_name);

public:
	IC				CALifeStorageManager(xrServer* server, pcstr section);
	virtual			~CALifeStorageManager( );
	bool	load(pcstr	save_name = 0);
	void	save(pcstr	save_name = 0, bool update_name = true);
	void	save(CNetPacket& net_packet);
};

#include "alife_storage_manager_inline.h"
