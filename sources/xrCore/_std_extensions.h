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
struct XRCORE_API xr_token
{
	pcstr		name;
	int				id;
};
// ----------------------------------------------------------------------------
IC pcstr get_token_name(xr_token* tokens, int key)
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
IC int get_token_id(xr_token* tokens, pcstr key)
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
template <class T>	IC T		_min(T a, T b)
{
	return a < b ? a : b;
}
template <class T>	IC T		_max(T a, T b)
{
	return a > b ? a : b;
}
template <class T>	IC T		_sqr(T a)
{
	return a * a;
}

// float
IC F32	_abs(F32 x)
{
	return fabsf(x);
}
IC F32	_sqrt(F32 x)
{
	return sqrtf(x);
}
IC F32	_sin(F32 x)
{
	return sinf(x);
}
IC F32	_cos(F32 x)
{
	return cosf(x);
}
IC BOOL		_valid(const F32 x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( –INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls = _fpclass(double(x));
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
IC double	_abs(double x)
{
	return fabs(x);
}
IC double	_sqrt(double x)
{
	return sqrt(x);
}
IC double	_sin(double x)
{
	return sin(x);
}
IC double	_cos(double x)
{
	return cos(x);
}
IC BOOL		_valid(const double x)
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
IC s8		_abs(s8  x)
{
	return (x >= 0) ? x : s8(-x);
}
IC s8 		_min(s8  x, s8  y)
{
	return y + ((x - y) & ((x - y) >> (sizeof(s8) * 8 - 1)));
};
IC s8 		_max(s8  x, s8  y)
{
	return x - ((x - y) & ((x - y) >> (sizeof(s8) * 8 - 1)));
};

// unsigned int8
IC u8		_abs(u8 x)
{
	return x;
}

// int16
IC s16		_abs(s16 x)
{
	return (x >= 0) ? x : s16(-x);
}
IC s16		_min(s16 x, s16 y)
{
	return y + ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1)));
};
IC s16		_max(s16 x, s16 y)
{
	return x - ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1)));
};

// unsigned int16
IC u16		_abs(u16 x)
{
	return x;
}

// int32
IC s32		_abs(s32 x)
{
	return (x >= 0) ? x : s32(-x);
}
IC s32		_min(s32 x, s32 y)
{
	return y + ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1)));
};
IC s32		_max(s32 x, s32 y)
{
	return x - ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1)));
};

// int64
IC s64		_abs(s64 x)
{
	return (x >= 0) ? x : s64(-x);
}
IC s64		_min(s64 x, s64 y)
{
	return y + ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1)));
};
IC s64		_max(s64 x, s64 y)
{
	return x - ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1)));
};

IC u32							xr_strlen(pcstr S);

// string management
IC pcstr						strconcat(int dest_sz, pstr dest, pcstr S1, pcstr S2)
{
	u32 l1 = xr_strlen(S1);
	strcpy_s(dest, dest_sz, S1);
	strcat_s(dest, dest_sz - l1, S2);
	return dest;
	//.	return strcat(strcpy(dest,S1),S2);
}

// dest = S1+S2+S3
IC pcstr						strconcat(int dest_sz, pstr dest, pcstr S1, pcstr S2, pcstr S3)
{
	u32 l1 = xr_strlen(S1);
	u32 l2 = xr_strlen(S2);
	strcpy_s(dest, dest_sz, S1);
	strcat_s(dest, dest_sz - l1, S2);
	strcat_s(dest, dest_sz - l1 - l2, S3);

	return dest;
	//.	return strcat(strcat(strcpy(dest,S1),S2),S3);
}

// dest = S1+S2+S3+S4
IC pcstr						strconcat(int dest_sz, pstr dest, pcstr S1, pcstr S2, pcstr S3, pcstr S4)
{
	u32 l1 = xr_strlen(S1);
	u32 l2 = xr_strlen(S2);
	u32 l3 = xr_strlen(S3);
	strcpy_s(dest, dest_sz, S1);
	strcat_s(dest, dest_sz - l1, S2);
	strcat_s(dest, dest_sz - l1 - l2, S3);
	strcat_s(dest, dest_sz - l1 - l2 - l3, S4);

	return dest;
	//.	return strcat(strcat(strcat(strcpy(dest,S1),S2),S3),S4);
}

// dest = S1+S2+S3+S4+S5
IC pcstr						strconcat(int dest_sz, pstr dest, pcstr S1, pcstr S2, pcstr S3, pcstr S4, pcstr S5)
{
	u32 l1 = xr_strlen(S1);
	u32 l2 = xr_strlen(S2);
	u32 l3 = xr_strlen(S3);
	u32 l4 = xr_strlen(S4);
	strcpy_s(dest, dest_sz, S1);
	strcat_s(dest, dest_sz - l1, S2);
	strcat_s(dest, dest_sz - l1 - l2, S3);
	strcat_s(dest, dest_sz - l1 - l2 - l3, S4);
	strcat_s(dest, dest_sz - l1 - l2 - l3 - l4, S5);

	return dest;
	//.	return strcat(strcat(strcat(strcat(strcpy(dest,S1),S2),S3),S4),S5);
}

// dest = S1+S2+S3+S4+S5+S6
IC pcstr						strconcat(int dest_sz, pstr dest, pcstr S1, pcstr S2, pcstr S3, pcstr S4, pcstr S5, pcstr S6)
{
	u32 l1 = xr_strlen(S1);
	u32 l2 = xr_strlen(S2);
	u32 l3 = xr_strlen(S3);
	u32 l4 = xr_strlen(S4);
	u32 l5 = xr_strlen(S5);
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
IC pstr strext(pcstr S)
{
	return (pstr)strrchr(S, '.');
}

IC u32							xr_strlen(pcstr S)
{
	return (u32)strlen(S);
}

IC pstr xr_strlwr(pstr S)
{
	return strlwr(S);
}

#ifdef BREAK_AT_STRCMP
XRCORE_API	int					xr_strcmp(pcstr S1, pcstr S2);
#else
IC int							xr_strcmp(pcstr S1, pcstr S2)
{
	return (int)strcmp(S1, S2);
}
#endif

XRCORE_API	pstr timestamp(string64& dest);

extern XRCORE_API u32			crc32(pcvoid P, u32 len);
