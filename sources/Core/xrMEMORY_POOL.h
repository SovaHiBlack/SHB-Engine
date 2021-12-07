#pragma once

class xrMemory;

class	MEMPOOL
{
private:
	xrCriticalSection	cs;
	unsigned int					s_sector;		// large-memory sector size
	unsigned int					s_element;		// element size, for example 32
	unsigned int					s_count;		// element count = [s_sector/s_element]
	unsigned int					s_offset;		// header size
	unsigned int					block_count;	// block count
	unsigned char* list;

private:
	__forceinline void** access(void* P)
	{
		return (void**) ((void*) (P));
	}
	void				block_create( );

public:
	void				_initialize(unsigned int _element, unsigned int _sector, unsigned int _header);

	__forceinline unsigned int				get_block_count( )
	{
		return block_count;
	}
	__forceinline unsigned int				get_element( )
	{
		return s_element;
	}

	__forceinline void* create( )
	{
		cs.Enter( );
		if (0 == list)
		{
			block_create( );
		}

		void* E = list;
		list = (unsigned char*) *access(list);
		cs.Leave( );
		return			E;
	}
	__forceinline void			destroy(void*& P)
	{
		cs.Enter( );
		*access(P) = list;
		list = (unsigned char*) P;
		cs.Leave( );
	}
};
