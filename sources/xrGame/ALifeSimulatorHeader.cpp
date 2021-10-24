//	Module 		: ALifeSimulatorHeader.cpp
//	Description : ALife Simulator header

#include "stdafx.h"

#include "ALifeSimulatorHeader.h"

CALifeSimulatorHeader::~CALifeSimulatorHeader( )
{ }

void CALifeSimulatorHeader::save(IWriter& memory_stream)
{
	memory_stream.open_chunk(ALIFE_CHUNK_DATA);
	memory_stream.w_u32(ALIFE_VERSION);
	memory_stream.close_chunk( );
}

void CALifeSimulatorHeader::load(IReader& file_stream)
{
	R_ASSERT2(file_stream.find_chunk(ALIFE_CHUNK_DATA), "Can't find chunk ALIFE_CHUNK_DATA");
	m_version = file_stream.r_u32( );

	R_ASSERT2(m_version >= 0x0002, "ALife version mismatch! (Delete saved game and try again)");
};

bool CALifeSimulatorHeader::valid(IReader& file_stream) const
{
	if (!file_stream.find_chunk(ALIFE_CHUNK_DATA))
	{
		return false;
	}

	u32 version;
	file_stream.r(&version, sizeof(version));
	return (version >= 2);
}
