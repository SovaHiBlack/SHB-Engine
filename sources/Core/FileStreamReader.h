#pragma once

#include "StreamReader.h"

class CFileStreamReader : public CStreamReader
{
private:
	using inherited						= CStreamReader;

private:
	HANDLE								m_file_handle;

public:
	virtual void		construct		(pcstr file_name, const u32& window_size);
	virtual void		destroy			( );
};
