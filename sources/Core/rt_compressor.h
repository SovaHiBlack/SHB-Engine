#pragma once

extern CORE_API void	rtc_initialize( );
extern CORE_API unsigned int		rtc_compress(void* dst, unsigned int dst_len, const void* src, unsigned int src_len);
extern CORE_API unsigned int		rtc_decompress(void* dst, unsigned int dst_len, const void* src, unsigned int src_len);
extern CORE_API unsigned int		rtc_csize(unsigned int in);
