#pragma once

extern CORE_API unsigned	_writeLZ(int hf, Pvoid d, unsigned size);
extern CORE_API unsigned	_readLZ(int hf, Pvoid& d, unsigned size);

extern CORE_API void		_compressLZ(U8** dest, unsigned* dest_sz, Pvoid src, unsigned src_sz);
extern CORE_API void		_decompressLZ(U8** dest, unsigned* dest_sz, Pvoid src, unsigned src_sz);
