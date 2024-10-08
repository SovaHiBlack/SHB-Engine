// ========================================== SHB_Engine ==========================================
// Projekt		: Core
// Module		: _limits.h
// Author		: Anahoret
// Description	: type limits definition
// ===================================== SovaHiBlack© - 2022 ======================================

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

#define int_max			type_max(int)
#define int_min			type_min(int)
#define int_zero		type_zero(int)

#define flt_max			type_max(F32)
#define flt_min			type_min(F32)
//#define FLT_MAX         3.402823466e+38F        /* max value */
//#define FLT_MIN         1.175494351e-38F        /* min positive value */
#define FLT_MAX			flt_max
#define FLT_MIN			flt_min

#define flt_zero		type_zero(F32)
#define flt_eps			type_epsilon(F32)

#define dbl_max			type_max(double)
#define dbl_min			type_min(double)
#define dbl_zero		type_zero(double)
#define dbl_eps			type_epsilon(double)

#endif // ndef CORE__LIMITS_H_INCLUDED