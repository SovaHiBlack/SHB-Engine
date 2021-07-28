#pragma once

using CLASS_ID = U64;

#define MK_CLSID(a, b, c, d, e, f, g, h)	CLASS_ID((CLASS_ID(a)<<CLASS_ID(56))|(CLASS_ID(b)<<CLASS_ID(48))|(CLASS_ID(c)<<CLASS_ID(40))|(CLASS_ID(d)<<CLASS_ID(32))|(CLASS_ID(e)<<CLASS_ID(24))|(CLASS_ID(f)<<CLASS_ID(16))|(CLASS_ID(g)<<CLASS_ID(8))|(CLASS_ID(h)))

extern CORE_API void		__stdcall CLSID2TEXT(CLASS_ID id, char* text);
extern CORE_API CLASS_ID	__stdcall TEXT2CLSID(const char* text);
