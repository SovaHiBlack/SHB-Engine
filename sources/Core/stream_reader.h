#pragma once

class CORE_API CStreamReader : public IReaderBase<CStreamReader> {
private:
	HANDLE	m_file_mapping_handle;
	u32		m_start_offset;
	u32		m_file_size;
	u32		m_archive_size;
	u32		m_window_size;

private:
	u32		m_current_offset_from_start;
	u32		m_current_window_size;
	u8		*m_current_map_view_of_file;
	u8		*m_start_pointer;
	u8		*m_current_pointer;

private:
			void			map					(const u32 &new_offset);
			inline		void			unmap				();
			inline		void			remap				(const u32 &new_offset);

private:
	// should not be called
	inline						CStreamReader		(const CStreamReader &object);
	inline		CStreamReader	&operator=			(const CStreamReader &);

public:
	inline						CStreamReader		();

public:
	virtual	void			construct			(
								const HANDLE &file_mapping_handle,
								const u32 &start_offset,
								const u32 &file_size,
								const u32 &archive_size,
								const u32 &window_size
							);
	virtual	void			destroy				();

public:
	inline		const HANDLE	&file_mapping_handle() const;
	inline		u32				elapsed				() const;
	inline		const u32		&length				() const;
	inline		void			seek				(const int &offset);
	inline		u32				tell				() const;
	inline		void			close				();

public:
			void			advance				(const int &offset);
			void			r					(void *buffer, u32 buffer_size);
			CStreamReader	*open_chunk			(const u32 &chunk_id);
};

#include "stream_reader_inline.h"
