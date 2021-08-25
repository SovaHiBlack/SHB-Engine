#pragma once

// Type defs
using S8 = signed char;				// s8
using U8 = unsigned char;			// u8

using S16 = signed short;			// s16
using U16 = unsigned short;			// u16

using S32 = signed int;				// s32
using U32 = unsigned int;

typedef unsigned int		u32;

using S64 = signed __int64;			// s64
using U64 = unsigned __int64;		// u64

using F32 = float;					// f32
using F64 = double;					// f64

// Type limits
#define type_max(T)		(std::numeric_limits<T>::max())
#define type_min(T)		(-std::numeric_limits<T>::max())
#define type_zero(T)	(std::numeric_limits<T>::min())
#define type_epsilon(T)	(std::numeric_limits<T>::epsilon())

#define int_max			type_max(int)
#define int_min			type_min(int)
#define int_zero		type_zero(int)

#define flt_max			type_max(F32)
#define flt_min			type_min(F32)

//#define FLT_MAX			flt_max
//#define FLT_MIN			flt_min

#define flt_zero		type_zero(F32)
#define flt_eps			type_epsilon(F32)

#define dbl_max			type_max(F64)
#define dbl_min			type_min(F64)
#define dbl_zero		type_zero(F64)
#define dbl_eps			type_epsilon(F64)

typedef	char	string16[16];
typedef	char	string32[32];
typedef	char	string64[64];
typedef	char	string128[128];
typedef	char	string256[256];
typedef	char	string512[512];
typedef	char	string1024[1024];
typedef	char	string2048[2048];
typedef	char	string4096[4096];

typedef	char	string_path[_MAX_PATH];
