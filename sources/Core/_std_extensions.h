#pragma once

#define	BREAK_AT_STRCMP

#ifndef DEBUG
#undef BREAK_AT_STRCMP
#endif // DEBUG

#ifdef abs
#undef abs
#endif

#ifdef _MIN
#undef _MIN
#endif

#ifdef _MAX
#undef _MAX
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// token type definition
// ----------------------------------------------------------------------------
struct CORE_API xr_token
{
	const char* name;
	int				id;
};
// ----------------------------------------------------------------------------
inline const char* get_token_name(xr_token* tokens, int key)
{
	for (int k = 0; tokens[k].name; k++)
	{
		if (key == tokens[k].id)
		{
			return tokens[k].name;
		}
	}

	return "";
}
// ----------------------------------------------------------------------------
inline int get_token_id(xr_token* tokens, const char* key)
{
	for (int k = 0; tokens[k].name; k++)
	{
		if (stricmp(tokens[k].name, key) == 0)
		{
			return tokens[k].id;
		}
	}

	return -1;
}
// ----------------------------------------------------------------------------

// generic
template <class T>	inline T		_min(T a, T b)
{
	return a < b ? a : b;
}
template <class T>	inline T		_max(T a, T b)
{
	return a > b ? a : b;
}
template <class T>	inline T		_sqr(T a)
{
	return a * a;
}

// float
inline F32	_abs(F32 x)
{
	return fabsf(x);
}
inline F32	_sqrt(F32 x)
{
	return sqrtf(x);
}
inline F32	_sin(F32 x)
{
	return sinf(x);
}
inline F32	_cos(F32 x)
{
	return cosf(x);
}
inline BOOL		_valid(const F32 x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( –INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls = _fpclass(F64(x));
	if (cls & (_FPCLASS_SNAN + _FPCLASS_QNAN + _FPCLASS_NINF + _FPCLASS_PINF + _FPCLASS_ND + _FPCLASS_PD))
		return	false;

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero
	_FPCLASS_NZ Negative zero ( – 0)
	_FPCLASS_PZ Positive 0 (+0)
	_FPCLASS_PN Positive normalized non-zero
	*/
	return		true;
}


// double
inline F64	_abs(F64 x)
{
	return fabs(x);
}
inline F64	_sqrt(F64 x)
{
	return sqrt(x);
}
inline F64	_sin(F64 x)
{
	return sin(x);
}
inline F64	_cos(F64 x)
{
	return cos(x);
}
inline BOOL		_valid(const F64 x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( –INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls = _fpclass(x);
	if (cls & (_FPCLASS_SNAN + _FPCLASS_QNAN + _FPCLASS_NINF + _FPCLASS_PINF + _FPCLASS_ND + _FPCLASS_PD))
		return false;

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero
	_FPCLASS_NZ Negative zero ( – 0)
	_FPCLASS_PZ Positive 0 (+0)
	_FPCLASS_PN Positive normalized non-zero
	*/
	return		true;
}

// int8
inline signed char		_abs(signed char x)
{
	return ((x >= 0) ? x : signed char(-x));
}
inline signed char		_min(signed char x, signed char y)
{
	return (y + ((x - y) & ((x - y) >> (sizeof(signed char) * 8 - 1))));
}
inline signed char		_max(signed char x, signed char y)
{
	return (x - ((x - y) & ((x - y) >> (sizeof(signed char) * 8 - 1))));
}

// unsigned int8
inline unsigned char		_abs(unsigned char x)
{
	return x;
}

// int16
inline signed short		_abs(signed short x)
{
	return (x >= 0) ? x : signed short(-x);
}
inline signed short		_min(signed short x, signed short y)
{
	return y + ((x - y) & ((x - y) >> (sizeof(signed short) * 8 - 1)));
}
inline signed short		_max(signed short x, signed short y)
{
	return x - ((x - y) & ((x - y) >> (sizeof(signed short) * 8 - 1)));
}

// unsigned int16
inline unsigned short		_abs(unsigned short x)
{
	return x;
}

// int32
inline int		_abs(int x)
{
	return (x >= 0) ? x : int(-x);
}
inline int		_min(int x, int y)
{
	return y + ((x - y) & ((x - y) >> (sizeof(int) * 8 - 1)));
};
inline int		_max(int x, int y)
{
	return x - ((x - y) & ((x - y) >> (sizeof(int) * 8 - 1)));
};

// int64
inline S64		_abs(S64 x)
{
	return (x >= 0) ? x : S64(-x);
}
inline S64		_min(S64 x, S64 y)
{
	return y + ((x - y) & ((x - y) >> (sizeof(S64) * 8 - 1)));
};
inline S64		_max(S64 x, S64 y)
{
	return x - ((x - y) & ((x - y) >> (sizeof(S64) * 8 - 1)));
};

inline U32							xr_strlen(const char* S);

// string management
inline const char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2)
{
	U32 l1 = xr_strlen(S1);
	strcpy_s(dest, dest_sz, S1);
	strcat_s(dest, dest_sz - l1, S2);
	return dest;
//.	return strcat(strcpy(dest,S1),S2);
}

// dest = S1+S2+S3
inline const char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3)
{
	U32 l1 = xr_strlen(S1);
	U32 l2 = xr_strlen(S2);
	strcpy_s(dest, dest_sz, S1);
	strcat_s(dest, dest_sz - l1, S2);
	strcat_s(dest, dest_sz - l1 - l2, S3);

	return dest;
//.	return strcat(strcat(strcpy(dest,S1),S2),S3);
}

// dest = S1+S2+S3+S4
inline const char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4)
{
	U32 l1 = xr_strlen(S1);
	U32 l2 = xr_strlen(S2);
	U32 l3 = xr_strlen(S3);
	strcpy_s(dest, dest_sz, S1);
	strcat_s(dest, dest_sz - l1, S2);
	strcat_s(dest, dest_sz - l1 - l2, S3);
	strcat_s(dest, dest_sz - l1 - l2 - l3, S4);

	return dest;
//.	return strcat(strcat(strcat(strcpy(dest,S1),S2),S3),S4);
}

// dest = S1+S2+S3+S4+S5
inline const char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4, const char* S5)
{
	U32 l1 = xr_strlen(S1);
	U32 l2 = xr_strlen(S2);
	U32 l3 = xr_strlen(S3);
	U32 l4 = xr_strlen(S4);
	strcpy_s(dest, dest_sz, S1);
	strcat_s(dest, dest_sz - l1, S2);
	strcat_s(dest, dest_sz - l1 - l2, S3);
	strcat_s(dest, dest_sz - l1 - l2 - l3, S4);
	strcat_s(dest, dest_sz - l1 - l2 - l3 - l4, S5);

	return dest;
//.	return strcat(strcat(strcat(strcat(strcpy(dest,S1),S2),S3),S4),S5);
}

// dest = S1+S2+S3+S4+S5+S6
inline const char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4, const char* S5, const char* S6)
{
	U32 l1 = xr_strlen(S1);
	U32 l2 = xr_strlen(S2);
	U32 l3 = xr_strlen(S3);
	U32 l4 = xr_strlen(S4);
	U32 l5 = xr_strlen(S5);
	strcpy_s(dest, dest_sz, S1);
	strcat_s(dest, dest_sz - l1, S2);
	strcat_s(dest, dest_sz - l1 - l2, S3);
	strcat_s(dest, dest_sz - l1 - l2 - l3, S4);
	strcat_s(dest, dest_sz - l1 - l2 - l3 - l4, S5);
	strcat_s(dest, dest_sz - l1 - l2 - l3 - l4 - l5, S6);

	return dest;
	//.	return strcat(strcat(strcat(strcat(strcat(strcpy(dest,S1),S2),S3),S4),S5),S6);
}

// return pointer to ".ext"
inline char* strext(const char* S)
{
	return (char*) strrchr(S, '.');
}

inline U32							xr_strlen(const char* S)
{
	return (U32) strlen(S);
}

inline char* xr_strlwr(char* S)
{
	return strlwr(S);
}

#ifdef BREAK_AT_STRCMP
CORE_API	int					xr_strcmp(const char* S1, const char* S2);
#else
inline int							xr_strcmp(const char* S1, const char* S2)
{
	return (int) strcmp(S1, S2);
}
#endif

CORE_API	char* timestamp(string64& dest);

extern CORE_API U32			crc32(const void* P, U32 len);
