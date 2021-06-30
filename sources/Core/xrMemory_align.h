#pragma once

U32 __stdcall xr_aligned_msize(Pvoid);
void __stdcall xr_aligned_free(Pvoid);
Pvoid __stdcall xr_aligned_malloc(size_t, size_t);
Pvoid __stdcall xr_aligned_offset_malloc(size_t, size_t, size_t);
Pvoid __stdcall xr_aligned_realloc(Pvoid, size_t, size_t);
Pvoid __stdcall xr_aligned_offset_realloc(Pvoid, size_t, size_t, size_t);
