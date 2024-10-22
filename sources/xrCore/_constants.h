// ========================================== SHB_Engine ==========================================
// Projekt		: Core
// Module		: _constants.h
// Author		: Anahoret
// Description	: Constants definition
// ===================================== SovaHiBlack© - 2022 ======================================

#pragma once
#ifndef CORE__CONSTANTS_H_INCLUDED
#define CORE__CONSTANTS_H_INCLUDED

// Undef some macros
#ifdef M_PI
#undef M_PI
#endif

#ifdef PI
#undef PI
#endif

// Constants
const	f32		EPSILON_7 = 0.0000001f;
const	f32		EPSILON_5 = 0.00001f;
const	f32		EPSILON_4 = 0.0001f;
const	f32		EPSILON_3 = 0.001f;

const	f32		M_PI = 3.1415926535897932384626433832795f;
const	f32		PI = 3.1415926535897932384626433832795f;
const	f32		PI_MUL_2 = 6.2831853071795864769252867665590f;
const	f32		PI_MUL_3 = 9.4247779607693797153879301498385f;
const	f32		PI_MUL_4 = 12.566370614359172953850573533118f;
const	f32		PI_MUL_6 = 18.849555921538759430775860299677f;
const	f32		PI_MUL_8 = 25.132741228718345907701147066236f;
const	f32		PI_DIV_2 = 1.5707963267948966192313216916398f;
const	f32		PI_DIV_3 = 1.0471975511965977461542144610932f;
const	f32		PI_DIV_4 = 0.7853981633974483096156608458199f;
const	f32		PI_DIV_6 = 0.5235987755982988730771072305466f;
const	f32		PI_DIV_8 = 0.3926990816987241548078304229099f;

#endif // ndef CORE__CONSTANTS_H_INCLUDED