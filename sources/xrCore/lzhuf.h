#pragma once

extern XRCORE_API u32		_writeLZ		(int hf, pvoid d, u32 size);
extern XRCORE_API u32		_readLZ			(int hf, pvoid& d, u32 size);

extern XRCORE_API void		_compressLZ		(u8** dest, u32* dest_sz, pvoid src, u32 src_sz);
extern XRCORE_API void		_decompressLZ	(u8** dest, u32* dest_sz, pvoid src, u32 src_sz);
