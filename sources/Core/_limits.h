// ========================================== SHB_Engine ==========================================
// Projekt		: Core
// Module		: _limits.h
// Author		: Anahoret
// Description	: type limits definition
// ========================================= SovaHiBlack© =========================================

#pragma once
#ifndef CORE__LIMITS_H_INCLUDED
#define CORE__LIMITS_H_INCLUDED

#ifdef FLT_MAX
#undef FLT_MAX
#undef FLT_MIN
#endif

// Type limits
#define type_max(T)		(std::numeric_limits<T>::max())
#define type_min(T)		(-std::numeric_limits<T>::max())
#define type_zero(T)	(std::numeric_limits<T>::min())
#define type_epsilon(T)	(std::numeric_limits<T>::epsilon())

#define int_max			type_max(s32)
#define int_min			type_min(int)
#define int_zero		type_zero(int)

#define flt_max			type_max(f32)
#define flt_min			type_min(f32)
#define flt_zero		type_zero(f32)
#define flt_eps			type_epsilon(f32)

#define FLT_MAX			flt_max
#define FLT_MIN			flt_min

#endif // ndef CORE__LIMITS_H_INCLUDED