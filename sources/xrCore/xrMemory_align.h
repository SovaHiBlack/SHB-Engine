#pragma once

u32		__stdcall xr_aligned_msize(pvoid);
void    __stdcall xr_aligned_free(pvoid);
pvoid __stdcall xr_aligned_malloc(size_t, size_t);
pvoid __stdcall xr_aligned_offset_malloc(size_t, size_t, size_t);
pvoid __stdcall xr_aligned_realloc(pvoid, size_t, size_t);
pvoid __stdcall xr_aligned_offset_realloc(pvoid, size_t, size_t, size_t);
