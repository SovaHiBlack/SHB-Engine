#pragma once

class CORE_API CStreamReader : public IReaderBase<CStreamReader>
{
private:
	HANDLE													m_file_mapping_handle;
	unsigned int											m_start_offset;
	unsigned int											m_file_size;
	unsigned int											m_archive_size;
	unsigned int											m_window_size;

	unsigned int											m_current_offset_from_start;
	unsigned int											m_current_window_size;
	unsigned char*											m_current_map_view_of_file;
	unsigned char*											m_start_pointer;
	unsigned char*											m_current_pointer;

	void							map						(const unsigned int& new_offset);
	inline void						unmap					( );
	inline void						remap					(const unsigned int& new_offset);

	// should not be called
	inline							CStreamReader			(const CStreamReader& object);
	inline CStreamReader&			operator=				(const CStreamReader&);

public:
	inline							CStreamReader			( );

	virtual void					construct				(const HANDLE& file_mapping_handle,
																const unsigned int& start_offset,
																const unsigned int& file_size,
																const unsigned int& archive_size,
																const unsigned int& window_size);
	virtual void					destroy					( );

	inline const HANDLE&			file_mapping_handle		( ) const;
	inline unsigned int				elapsed					( ) const;
	inline const unsigned int&		length					( ) const;
	inline void						seek					(const int& offset);
	inline unsigned int				tell					( ) const;
	inline void						close					( );

	void							advance					(const int& offset);
	void							r						(void* buffer, unsigned int buffer_size);
	CStreamReader*					open_chunk				(const unsigned int& chunk_id);
};

#include "StreamReader_inline.h"
