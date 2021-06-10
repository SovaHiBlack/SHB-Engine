#pragma once

extern CORE_API unsigned	_writeLZ		(int hf, void* d, unsigned size);
extern CORE_API unsigned	_readLZ			(int hf, void* &d, unsigned size);

extern CORE_API void		_compressLZ		(U8** dest, unsigned* dest_sz, void* src, unsigned src_sz);
extern CORE_API void		_decompressLZ	(U8** dest, unsigned* dest_sz, void* src, unsigned src_sz);
