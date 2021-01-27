#pragma once

extern CORE_API void	rtc_initialize		( );
extern CORE_API u32		rtc_compress		(void* dst, u32 dst_len, const void* src, u32 src_len);
extern CORE_API u32		rtc_decompress		(void* dst, u32 dst_len, const void* src, u32 src_len);
extern CORE_API u32		rtc_csize			(u32 in);
