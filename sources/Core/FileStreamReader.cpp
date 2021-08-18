#include "stdafx.h"

#include "FileStreamReader.h"

void CFileStreamReader::construct(const char* file_name, const unsigned int& window_size)
{
	m_file_handle =
		CreateFile(
		file_name,
		GENERIC_READ,
		FILE_SHARE_READ,
		0,
		OPEN_EXISTING,
		0,
		0
		);

	VERIFY(m_file_handle != INVALID_HANDLE_VALUE);
	unsigned int						file_size = (unsigned int) GetFileSize(m_file_handle, NULL);

	HANDLE					file_mapping_handle =
		CreateFileMapping(
		m_file_handle,
		0,
		PAGE_READONLY,
		0,
		0,
		0
		);
	VERIFY(file_mapping_handle != INVALID_HANDLE_VALUE);

	inherited::construct(file_mapping_handle, 0, file_size, file_size, window_size);
}

void CFileStreamReader::destroy( )
{
	HANDLE					file_mapping_handle = this->file_mapping_handle( );
	inherited::destroy( );
	CloseHandle(file_mapping_handle);
	CloseHandle(m_file_handle);
}
