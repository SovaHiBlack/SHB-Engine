#pragma once

extern CORE_API u32		_writeLZ		(int hf, pvoid d, u32 size);
extern CORE_API u32		_readLZ			(int hf, pvoid& d, u32 size);

extern CORE_API void		_compressLZ		(u8** dest, u32* dest_sz, pvoid src, u32 src_sz);
extern CORE_API void		_decompressLZ	(u8** dest, u32* dest_sz, pvoid src, u32 src_sz);
