#pragma once

extern CORE_API void	rtc_initialize( );
extern CORE_API U32		rtc_compress(void* dst, U32 dst_len, const void* src, U32 src_len);
extern CORE_API U32		rtc_decompress(void* dst, U32 dst_len, const void* src, U32 src_len);
extern CORE_API U32		rtc_csize(U32 in);
