#pragma once

#include "stream_reader.h"

class CFileStreamReader : public CStreamReader
{
private:
	using inherited				= CStreamReader;
	HANDLE						m_file_handle;

public:
	virtual void	construct	(const char* file_name, const unsigned int& window_size);
	virtual void	destroy		( );
};
