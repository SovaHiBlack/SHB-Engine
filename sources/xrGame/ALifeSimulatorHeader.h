//	Module 		: ALifeSimulatorHeader.h
//	Description : ALife Simulator header

#pragma once

#include "object_interfaces.h"
#include "ALife_space.h"

class CALifeSimulatorHeader
{
protected:
	u32								m_version;

public:
	inline								CALifeSimulatorHeader(const char* section);
	virtual							~CALifeSimulatorHeader( );
	virtual void					save(IWriter& tMemoryStream);
	virtual void					load(IReader& tFileStream);
	inline u32						version( ) const;
	bool					valid(IReader& file_stream) const;
};

#include "ALifeSimulatorHeader_inline.h"
