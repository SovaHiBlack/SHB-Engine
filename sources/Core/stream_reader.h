#pragma once

class CORE_API CStreamReader : public IReaderBase<CStreamReader>
{
private:
	HANDLE	m_file_mapping_handle;
	U32		m_start_offset;
	U32		m_file_size;
	U32		m_archive_size;
	U32		m_window_size;

	U32		m_current_offset_from_start;
	U32		m_current_window_size;
	U8* m_current_map_view_of_file;
	U8* m_start_pointer;
	U8* m_current_pointer;

	void			map(const U32& new_offset);
	inline void			unmap( );
	inline void			remap(const U32& new_offset);

	// should not be called
	inline						CStreamReader(const CStreamReader& object);
	inline CStreamReader& operator=			(const CStreamReader&);

public:
	inline						CStreamReader( );

	virtual void			construct(
		const HANDLE& file_mapping_handle,
		const U32& start_offset,
		const U32& file_size,
		const U32& archive_size,
		const U32& window_size
	);
	virtual void			destroy( );

	inline const HANDLE& file_mapping_handle( ) const;
	inline U32				elapsed( ) const;
	inline const U32& length( ) const;
	inline void			seek(const int& offset);
	inline U32				tell( ) const;
	inline void			close( );

	void			advance(const int& offset);
	void			r(void* buffer, U32 buffer_size);
	CStreamReader* open_chunk(const U32& chunk_id);
};

#include "stream_reader_inline.h"
