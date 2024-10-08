// ========================================== SHB_Engine ==========================================
// Projekt		: Core
// Module		: _types.h
// Author		: Anahoret
// Description	: type definition
// ===================================== SovaHiBlack© - 2022 ======================================

#pragma once
#ifndef CORE__TYPES_H_INCLUDED
#define CORE__TYPES_H_INCLUDED

typedef	signed		char	s8;
typedef	unsigned	char	u8;

typedef	signed		short	s16;
typedef	unsigned	short	u16;

typedef	signed		int		s32;
typedef	unsigned	int		u32;

typedef	signed		__int64	s64;
typedef	unsigned	__int64	u64;

typedef float				f32;
typedef double				f64;
using F32 = float;

using pstr = char*;
using pcstr = const char*;

using pvoid = void*;
using pcvoid = const void*;

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

#endif // ndef CORE__TYPES_H_INCLUDED