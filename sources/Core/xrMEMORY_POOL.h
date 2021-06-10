#pragma once

class xrMemory;

class	MEMPOOL
{
private:
	xrCriticalSection	cs;
	u32					s_sector;		// large-memory sector size
	u32					s_element;		// element size, for example 32
	u32					s_count;		// element count = [s_sector/s_element]
	u32					s_offset;		// header size
	u32					block_count;	// block count
	U8*					list;

private:
	__forceinline void**			access			(void* P)	{ return (void**) ((void*)(P));	}
	void				block_create	();

public:
	void				_initialize		(u32 _element, u32 _sector, u32 _header);

	__forceinline u32				get_block_count	()	{ return block_count; }
	__forceinline u32				get_element		()	{ return s_element; }

	__forceinline void*			create			()
	{
		cs.Enter		();
		if (0==list)	block_create();

		void* E			= list;
		list			= (U8*)*access(list);
		cs.Leave		();
		return			E;
	}
	__forceinline void			destroy			(void* &P)
	{
		cs.Enter		();
		*access(P)		= list;
		list			= (U8*)P;
		cs.Leave		();
	}
};
