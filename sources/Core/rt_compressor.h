#pragma once

extern CORE_API void		rtc_initialize	();
extern CORE_API u32		rtc_compress	(pvoid dst, u32 dst_len, pcvoid src, u32 src_len);
extern CORE_API u32		rtc_decompress	(pvoid dst, u32 dst_len, pcvoid src, u32 src_len);
extern CORE_API u32		rtc_csize		(u32 in);
