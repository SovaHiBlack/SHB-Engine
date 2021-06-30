#include "stdafx.h"

#include "rt_lzo1x.h"

#define HEAP_ALLOC(var,size)	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

__declspec(thread) HEAP_ALLOC(rtc_wrkmem, LZO1X_1_MEM_COMPRESS);

void	rtc_initialize( )
{
	VERIFY(lzo_init( ) == LZO_E_OK);
}

U32		rtc_csize(U32 in)
{
	VERIFY(in);
	return			in + in / 64 + 16 + 3;
}

U32		rtc_compress(Pvoid dst, U32 dst_len, Pcvoid src, U32 src_len)
{
	U32		out_size = dst_len;
	int r = lzo1x_1_compress(
		(const lzo_byte*) src, (lzo_uint) src_len,
		(lzo_byte*) dst, (lzo_uintp) &out_size,
		rtc_wrkmem);
	VERIFY(r == LZO_E_OK);
	return	out_size;
}

U32		rtc_decompress(Pvoid dst, U32 dst_len, Pcvoid src, U32 src_len)
{
	U32		out_size = dst_len;
	int r = lzo1x_decompress(
		(const lzo_byte*) src, (lzo_uint) src_len,
		(lzo_byte*) dst, (lzo_uintp) &out_size,
		rtc_wrkmem);
	VERIFY(r == LZO_E_OK);
	return	out_size;
}
