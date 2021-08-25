#pragma once

// Undef some macros
#ifdef M_PI
#undef M_PI
#endif

#ifdef PI
#undef PI
#endif

//#ifdef FLT_MAX
//#undef FLT_MAX
//#undef FLT_MIN
//#endif

// Constants
const F32 EPS_S = 0.0000001f;
const F32 EPS = 0.00001f;
const F32 EPS_L = 0.001f;

const F32 M_PI = 3.1415926535897932384626433832795f;
const F32 PI = 3.1415926535897932384626433832795f;
const F32 PI_MUL_2 = 6.2831853071795864769252867665590f;
const F32 PI_MUL_3 = 9.4247779607693797153879301498385f;
const F32 PI_MUL_4 = 12.566370614359172953850573533118f;
const F32 PI_MUL_6 = 18.849555921538759430775860299677f;
const F32 PI_MUL_8 = 25.132741228718345907701147066236f;
const F32 PI_DIV_2 = 1.5707963267948966192313216916398f;
const F32 PI_DIV_3 = 1.0471975511965977461542144610932f;
const F32 PI_DIV_4 = 0.7853981633974483096156608458199f;
const F32 PI_DIV_6 = 0.5235987755982988730771072305466f;
const F32 PI_DIV_8 = 0.3926990816987241548078304229099f;

// Define namespaces (CPU & FPU)
#include	"_math.h"
#include	"_bitwise.h"
#include	"_std_extensions.h"

// comparisions
inline BOOL fsimilar(F32 a, F32 b, F32 cmp = EPS)
{
	return _abs(a - b) < cmp;
}

inline BOOL fis_zero(F32 val, F32 cmp = EPS_S)
{
	return _abs(val) < cmp;
}

// degree 2 radians and vice-versa
namespace implement
{
	template <class T>
	__forceinline T	deg2rad(T val)
	{
		return (val * T(M_PI) / T(180));
	}

	template <class T>
	__forceinline T	rad2deg(T val)
	{
		return (val * T(180) / T(M_PI));
	}
};
__forceinline F32	deg2rad(F32 val)
{
	return implement::deg2rad(val);
}
__forceinline F64	deg2rad(F64 val)
{
	return implement::deg2rad(val);
}
__forceinline F32	rad2deg(F32 val)
{
	return implement::rad2deg(val);
}
__forceinline F64	rad2deg(F64 val)
{
	return implement::rad2deg(val);
}

// clamping/snapping
template <class T>
inline void clamp(T& val, const T& _low, const T& _high)
{
	if (val < _low)
	{
		val = _low;
	}
	else if (val > _high)
	{
		val = _high;
	}
}

template <class T>
inline T clampr(const T& val, const T& _low, const T& _high)
{
	if (val < _low)
	{
		return _low;
	}
	else if (val > _high)
	{
		return _high;
	}
	else
	{
		return val;
	}
}

inline F32 snapto(F32 value, F32 snap)
{
	if (snap <= 0.f)
	{
		return value;
	}

	return F32(iFloor((value + (snap * 0.5f)) / snap)) * snap;
}

// pre-definitions
template <class T> struct _quaternion;

#pragma pack(push)
#pragma pack(1)
#include "_random.h"
#include "_color.h"
#include "_vector2.h"
#include "_vector3.h"
#include "_vector4.h"
#include "_matrix.h"
#include "_matrix33.h"
#include "_quaternion.h"
#include "_rect.h"
#include "_box2.h"
#include "_box3.h"
#include "_obb.h"
#include "_sphere.h"
#include "_cylinder.h"
#include "_compressed_normal.h"
#include "_plane.h"
#include "_plane2.h"
#include "_flags.h"
#pragma pack(pop)

// normalize angle (0..2PI)
__forceinline F32		angle_normalize_always(F32 a)
{
	F32 div = a / PI_MUL_2;
	int rnd = (div > 0) ? iFloor(div) : iCeil(div);
	F32 frac = div - rnd;
	if (frac < 0)
	{
		frac += 1.0f;
	}

	return frac * PI_MUL_2;
}

// normalize angle (0..2PI)
__forceinline F32 angle_normalize(F32 a)
{
	if (a >= 0 && a <= PI_MUL_2)
	{
		return a;
	}
	else
	{
		return angle_normalize_always(a);
	}
}

// -PI .. +PI
__forceinline F32 angle_normalize_signed(F32 a)
{
	if (a >= (-PI) && a <= PI)
	{
		return a;
	}

	F32 angle = angle_normalize_always(a);
	if (angle > PI)
	{
		angle -= PI_MUL_2;
	}

	return angle;
}

// -PI..PI
__forceinline F32 angle_difference_signed(F32 a, F32 b)
{
	F32 diff = angle_normalize_signed(a) - angle_normalize_signed(b);
	if (diff > 0)
	{
		if (diff > PI)
		{
			diff -= PI_MUL_2;
		}
	}
	else
	{
		if (diff < -PI)
		{
			diff += PI_MUL_2;
		}
	}

	return diff;
}

// 0..PI
__forceinline F32 angle_difference(F32 a, F32 b)
{
	return _abs(angle_difference_signed(a, b));
}

// c=current, t=target, s=speed, dt=dt
inline bool angle_lerp(F32& c, F32 t, F32 s, F32 dt)
{
	F32 diff = t - c;
	if (diff > 0)
	{
		if (diff > PI)
		{
			diff -= PI_MUL_2;
		}
	}
	else
	{
		if (diff < -PI)
		{
			diff += PI_MUL_2;
		}
	}

	F32 diff_a = _abs(diff);

	if (diff_a < EPS_S)
	{
		return true;
	}

	F32 mot = s * dt;
	if (mot > diff_a)
	{
		mot = diff_a;
	}

	c += (diff / diff_a) * mot;

	if (c < 0)
	{
		c += PI_MUL_2;
	}
	else if (c > PI_MUL_2)
	{
		c -= PI_MUL_2;
	}

	return false;
}

// Just lerp :) expects normalized angles in range [0..2PI)
__forceinline F32 angle_lerp(F32 A, F32 B, F32 f)
{
	F32 diff = B - A;
	if (diff > PI)
	{
		diff -= PI_MUL_2;
	}
	else if (diff < -PI)
	{
		diff += PI_MUL_2;
	}

	return A + diff * f;
}

inline F32 angle_inertion(F32 src, F32 tgt, F32 speed, F32 clmp, F32 dt)
{
	F32 a = angle_normalize_signed(tgt);
	angle_lerp(src, a, speed, dt);
	src = angle_normalize_signed(src);
	F32 dH = angle_difference_signed(src, a);
	F32 dCH = clampr(dH, -clmp, clmp);
	src -= dH - dCH;
	return src;
}

inline F32 angle_inertion_var(F32 src, F32 tgt, F32 min_speed, F32 max_speed, F32 clmp, F32 dt)
{
	tgt = angle_normalize_signed(tgt);
	src = angle_normalize_signed(src);
	F32 speed = _abs((max_speed - min_speed) * angle_difference(tgt, src) / clmp) + min_speed;
	angle_lerp(src, tgt, speed, dt);
	src = angle_normalize_signed(src);
	F32 dH = angle_difference_signed(src, tgt);
	F32 dCH = clampr(dH, -clmp, clmp);
	src -= dH - dCH;
	return src;
}

template <class T>
inline _matrix<T>& _matrix<T>::rotation(const _quaternion<T>& Q)
{
	T xx = Q.x * Q.x;
	T yy = Q.y * Q.y;
	T zz = Q.z * Q.z;
	T xy = Q.x * Q.y;
	T xz = Q.x * Q.z;
	T yz = Q.y * Q.z;
	T wx = Q.w * Q.x;
	T wy = Q.w * Q.y;
	T wz = Q.w * Q.z;

	_11 = 1 - 2 * (yy + zz);
	_12 = 2 * (xy - wz);
	_13 = 2 * (xz + wy);
	_14 = 0;
	_21 = 2 * (xy + wz);
	_22 = 1 - 2 * (xx + zz);
	_23 = 2 * (yz - wx);
	_24 = 0;
	_31 = 2 * (xz - wy);
	_32 = 2 * (yz + wx);
	_33 = 1 - 2 * (xx + yy);
	_34 = 0;
	_41 = 0;
	_42 = 0;
	_43 = 0;
	_44 = 1;

	return *this;
}

template <class T>
inline _matrix<T>& _matrix<T>::mk_xform(const _quaternion<T>& Q, const Tvector& V)
{
	T xx = Q.x * Q.x;
	T yy = Q.y * Q.y;
	T zz = Q.z * Q.z;
	T xy = Q.x * Q.y;
	T xz = Q.x * Q.z;
	T yz = Q.y * Q.z;
	T wx = Q.w * Q.x;
	T wy = Q.w * Q.y;
	T wz = Q.w * Q.z;

	_11 = 1 - 2 * (yy + zz);
	_12 = 2 * (xy - wz);
	_13 = 2 * (xz + wy);
	_14 = 0;
	_21 = 2 * (xy + wz);
	_22 = 1 - 2 * (xx + zz);
	_23 = 2 * (yz - wx);
	_24 = 0;
	_31 = 2 * (xz - wy);
	_32 = 2 * (yz + wx);
	_33 = 1 - 2 * (xx + yy);
	_34 = 0;
	_41 = V.x;
	_42 = V.y;
	_43 = V.z;
	_44 = 1;

	return *this;
}

#define TRACE_QZERO_TOLERANCE	0.1f
template <class T>
inline _quaternion<T>& _quaternion<T>::set(const _matrix<T>& M)
{
	F32 trace;
	F32 s;

	trace = M._11 + M._22 + M._33;
	if (trace > 0.0f)
	{
		s = _sqrt(trace + 1.0f);
		w = s * 0.5f;
		s = 0.5f / s;

		x = (M._32 - M._23) * s;
		y = (M._13 - M._31) * s;
		z = (M._21 - M._12) * s;
	}
	else
	{
		int biggest;
		enum
		{
			A,
			E,
			I
		};

		if (M._11 > M._22)
		{
			if (M._33 > M._11)
			{
				biggest = I;
			}
			else
			{
				biggest = A;
			}
		}
		else
		{
			if (M._33 > M._11)
			{
				biggest = I;
			}
			else
			{
				biggest = E;
			}
		}

		// in the unusual case the original trace fails to produce a good sqrt, try others...
		switch (biggest)
		{
			case A:
			{
				s = _sqrt(M._11 - (M._22 + M._33) + 1.0f);
				if (s > TRACE_QZERO_TOLERANCE)
				{
					x = s * 0.5f;
					s = 0.5f / s;
					w = (M._32 - M._23) * s;
					y = (M._12 + M._21) * s;
					z = (M._13 + M._31) * s;
					break;
				}
				// I
				s = _sqrt(M._33 - (M._11 + M._22) + 1.0f);
				if (s > TRACE_QZERO_TOLERANCE)
				{
					z = s * 0.5f;
					s = 0.5f / s;
					w = (M._21 - M._12) * s;
					x = (M._31 + M._13) * s;
					y = (M._32 + M._23) * s;
					break;
				}
				// E
				s = _sqrt(M._22 - (M._33 + M._11) + 1.0f);
				if (s > TRACE_QZERO_TOLERANCE)
				{
					y = s * 0.5f;
					s = 0.5f / s;
					w = (M._13 - M._31) * s;
					z = (M._23 + M._32) * s;
					x = (M._21 + M._12) * s;
					break;
				}
			}
			break;
			case E:
			{
				s = _sqrt(M._22 - (M._33 + M._11) + 1.0f);
				if (s > TRACE_QZERO_TOLERANCE)
				{
					y = s * 0.5f;
					s = 0.5f / s;
					w = (M._13 - M._31) * s;
					z = (M._23 + M._32) * s;
					x = (M._21 + M._12) * s;
					break;
				}
				// I
				s = _sqrt(M._33 - (M._11 + M._22) + 1.0f);
				if (s > TRACE_QZERO_TOLERANCE)
				{
					z = s * 0.5f;
					s = 0.5f / s;
					w = (M._21 - M._12) * s;
					x = (M._31 + M._13) * s;
					y = (M._32 + M._23) * s;
					break;
				}
				// A
				s = _sqrt(M._11 - (M._22 + M._33) + 1.0f);
				if (s > TRACE_QZERO_TOLERANCE)
				{
					x = s * 0.5f;
					s = 0.5f / s;
					w = (M._32 - M._23) * s;
					y = (M._12 + M._21) * s;
					z = (M._13 + M._31) * s;
					break;
				}
			}
			break;
			case I:
			{
				s = _sqrt(M._33 - (M._11 + M._22) + 1.0f);
				if (s > TRACE_QZERO_TOLERANCE)
				{
					z = s * 0.5f;
					s = 0.5f / s;
					w = (M._21 - M._12) * s;
					x = (M._31 + M._13) * s;
					y = (M._32 + M._23) * s;
					break;
				}
				// A
				s = _sqrt(M._11 - (M._22 + M._33) + 1.0f);
				if (s > TRACE_QZERO_TOLERANCE)
				{
					x = s * 0.5f;
					s = 0.5f / s;
					w = (M._32 - M._23) * s;
					y = (M._12 + M._21) * s;
					z = (M._13 + M._31) * s;
					break;
				}
				// E
				s = _sqrt(M._22 - (M._33 + M._11) + 1.0f);
				if (s > TRACE_QZERO_TOLERANCE)
				{
					y = s * 0.5f;
					s = 0.5f / s;
					w = (M._13 - M._31) * s;
					z = (M._23 + M._32) * s;
					x = (M._21 + M._12) * s;
					break;
				}
			}
			break;
		}
	}

	return *this;
}
