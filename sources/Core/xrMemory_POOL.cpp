#include "stdafx.h"

#include "xrMemory_POOL.h"
#include "xrMemory_align.h"

void	MEMPOOL::block_create( )
{
	// Allocate
	R_ASSERT(0 == list);
	list = (unsigned char*) xr_aligned_offset_malloc(s_sector, 16, s_offset);

	// Partition
	for (unsigned int it = 0; it < (s_count - 1); it++)
	{
		unsigned char* E = list + it * s_element;
		*access(E) = E + s_element;
	}
	*access(list + (s_count - 1) * s_element) = NULL;
	block_count++;
}

void	MEMPOOL::_initialize(unsigned int _element, unsigned int _sector, unsigned int _header)
{
	R_ASSERT(_element < _sector / 2);
	s_sector = _sector;
	s_element = _element;
	s_count = s_sector / s_element;
	s_offset = _header;
	list = NULL;
	block_count = 0;
}
