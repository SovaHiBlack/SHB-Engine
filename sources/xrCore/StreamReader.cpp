#include "stdafx.h"
#include "StreamReader.h"

void CStreamReader::construct(const HANDLE& file_mapping_handle,
							  const u32& start_offset,
							  const u32& file_size,
							  const u32& archive_size,
							  const u32& window_size)
{
	m_file_mapping_handle = file_mapping_handle;
	m_start_offset = start_offset;
	m_file_size = file_size;
	m_archive_size = archive_size;
	m_window_size = _max(window_size, FS.dwAllocGranularity);

	map(0);
}

void CStreamReader::destroy( )
{
	unmap( );
}

void CStreamReader::map(const u32& new_offset)
{
	VERIFY(new_offset <= m_file_size);
	m_current_offset_from_start = new_offset;

	u32 granularity = FS.dwAllocGranularity;
	u32 start_offset = m_start_offset + new_offset;
	u32 pure_start_offset = start_offset;
	start_offset = (start_offset / granularity) * granularity;

	VERIFY(pure_start_offset >= start_offset);
	u32 pure_end_offset = m_window_size + pure_start_offset;
	u32 end_offset = pure_end_offset / granularity;
	if (pure_end_offset % granularity)
	{
		++end_offset;
	}

	end_offset *= granularity;
	if (end_offset > m_archive_size)
	{
		end_offset = m_archive_size;
	}

	m_current_window_size = end_offset - start_offset;
	m_current_map_view_of_file = (u8*) MapViewOfFile(m_file_mapping_handle, FILE_MAP_READ, 0, start_offset, m_current_window_size);
	m_current_pointer = m_current_map_view_of_file;

	u32 difference = pure_start_offset - start_offset;
	m_current_window_size -= difference;
	m_current_pointer += difference;
	m_start_pointer = m_current_pointer;
}

void CStreamReader::advance(const s32& offset)
{
	VERIFY(m_current_pointer >= m_start_pointer);
	VERIFY(u32(m_current_pointer - m_start_pointer) <= m_current_window_size);

	s32 offset_inside_window = s32(m_current_pointer - m_start_pointer);
	if (offset_inside_window + offset >= (s32) m_current_window_size)
	{
		remap(m_current_offset_from_start + offset_inside_window + offset);
		return;
	}

	if (offset_inside_window + offset < 0)
	{
		remap(m_current_offset_from_start + offset_inside_window + offset);
		return;
	}

	m_current_pointer += offset;
}

void CStreamReader::r(pvoid _buffer, u32 buffer_size)
{
	VERIFY(m_current_pointer >= m_start_pointer);
	VERIFY(u32(m_current_pointer - m_start_pointer) <= m_current_window_size);

	s32 offset_inside_window = s32(m_current_pointer - m_start_pointer);
	if (offset_inside_window + buffer_size < m_current_window_size)
	{
		Memory.mem_copy(_buffer, m_current_pointer, buffer_size);
		m_current_pointer += buffer_size;
		return;
	}

	u8* buffer = (u8*) _buffer;
	u32 elapsed_in_window = m_current_window_size - (m_current_pointer - m_start_pointer);
	do
	{
		Memory.mem_copy(buffer, m_current_pointer, elapsed_in_window);
		buffer += elapsed_in_window;
		buffer_size -= elapsed_in_window;
		advance(elapsed_in_window);

		elapsed_in_window = m_current_window_size;
	}
	while (m_current_window_size < buffer_size);

	Memory.mem_copy(buffer, m_current_pointer, buffer_size);
	advance(buffer_size);
}

CStreamReader* CStreamReader::open_chunk(const u32& chunk_id)
{
	BOOL compressed;
	u32 size = find_chunk(chunk_id, &compressed);
	if (!size)
	{
		return (0);
	}

	R_ASSERT2(!compressed, "cannot use CStreamReader on compressed chunks");
	CStreamReader* result = xr_new<CStreamReader>( );
	result->construct(file_mapping_handle( ), m_start_offset + tell( ), size, m_archive_size, m_window_size);
	return (result);
}
