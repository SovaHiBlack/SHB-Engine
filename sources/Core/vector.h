#pragma once

// Define namespaces (CPU & FPU)
#include "_math.h"
#include "_bitwise.h"
#include "_std_extensions.h"

// comparisions
IC BOOL fsimilar(f32 a, f32 b, f32 cmp = EPSILON_5)
{
	return (_abs(a - b) < cmp);
}

IC BOOL fis_zero(f32 val, f32 cmp = EPSILON_7)
{
	return (_abs(val) < cmp);
}

// degree 2 radians and vice-versa

//namespace implement
//{
//	template <class T>
//	ICF T deg2rad(T val)
//	{
//		return (val * T(M_PI) / T(180));
//	}
//
//	template <class T>
//	ICF T rad2deg(T val)
//	{
//		return (val * T(180) / T(M_PI));
//	}
//};

ICF f32 deg2rad(f32 val)
{
	return (val * M_PI / 180.0f); /*implement::deg2rad(val);*/
}
//ICF double deg2rad(double val)
//{
//	return implement::deg2rad(val);
//}

ICF f32	rad2deg(f32 val)
{
	return (val * 180.0f / M_PI); /*implement::rad2deg(val);*/
}
//ICF double	rad2deg(double val)
//{
//	return implement::rad2deg(val);
//}

// clamping
template <class T>
IC void clamp(T& val, const T& _low, const T& _high)
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
IC T clampr(const T& val, const T& _low, const T& _high)
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

//IC f32 snapto(f32 value, f32 snap)
//{
//	if (snap <= 0.0f)
//	{
//		return value;
//	}
//
//	return (f32(iFloor((value + (snap * 0.5f)) / snap)) * snap);
//}

// pre-definitions
template <class T>
class _quaternion;

#pragma pack(push)
#pragma pack(1)
#include "_random.h"

#include "_color.h"
#include "_vector3.h"
#include "_vector2.h"
#include "_vector4.h"
#include "_matrix4x4.h"
#include "_matrix3x3.h"
#include "_quaternion.h"
#include "_rect.h"
#include "_box2.h"
#include "_box3.h"
#include "_obb.h"
#include "_sphere.h"
#include "_cylinder.h"
#include "_compressed_normal.h"
#include "_plane3.h"
#include "_plane2.h"
#include "_flags.h"
#pragma pack(pop)

// normalize angle (0..2PI)
ICF f32 angle_normalize_always(f32 a)
{
	f32 div = a / PI_MUL_2;
	s32 rnd = (div > 0) ? iFloor(div) : iCeil(div);
	f32 frac = div - rnd;
	if (frac < 0.0f)
	{
		frac += 1.0f;
	}

	return (frac * PI_MUL_2);
}

// normalize angle (0..2PI)
ICF f32 angle_normalize(f32 a)
{
	if (a >= 0.0f && a <= PI_MUL_2)
	{
		return a;
	}
	else
	{
		return angle_normalize_always(a);
	}
}

// -PI .. +PI
ICF f32 angle_normalize_signed(f32 a)
{
	if (a >= (-PI) && a <= PI)
	{
		return a;
	}

	f32 angle = angle_normalize_always(a);
	if (angle > PI)
	{
		angle -= PI_MUL_2;
	}

	return angle;
}

// -PI..PI
ICF f32		angle_difference_signed(f32 a, f32 b)
{
	f32 diff = angle_normalize_signed(a) - angle_normalize_signed(b);
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
ICF f32		angle_difference(f32 a, f32 b)
{
	return _abs(angle_difference_signed(a, b));
}

// c=current, t=target, s=speed, dt=dt
IC bool			angle_lerp(f32& c, f32 t, f32 s, f32 dt)
{
	f32 diff = t - c;
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

	f32 diff_a = _abs(diff);

	if (diff_a < EPSILON_7)
	{
		return true;
	}

	f32 mot = s * dt;
	if (mot > diff_a)
	{
		mot = diff_a;
	}

	c += (diff / diff_a) * mot;

	if (c < 0.0f)
	{
		c += PI_MUL_2;
	}
	else if (c > PI_MUL_2)
	{
		c -= PI_MUL_2;
	}

	return false;
}

// Just lerp :)	expects normalized angles in range [0..2PI)
ICF f32		angle_lerp(f32 A, f32 B, f32 f)
{
	f32 diff = B - A;
	if (diff > PI)
	{
		diff -= PI_MUL_2;
	}
	else if (diff < -PI)
	{
		diff += PI_MUL_2;
	}

	return (A + diff * f);
}

//IC f32 angle_inertion(f32 src, f32 tgt, f32 speed, f32 clmp, f32 dt)
//{
//	f32 a = angle_normalize_signed(tgt);
//	angle_lerp(src, a, speed, dt);
//	src = angle_normalize_signed(src);
//	f32 dH = angle_difference_signed(src, a);
//	f32 dCH = clampr(dH, -clmp, clmp);
//	src -= dH - dCH;
//	return src;
//}

IC f32 angle_inertion_var(f32 src, f32 tgt, f32 min_speed, f32 max_speed, f32 clmp, f32 dt)
{
	tgt = angle_normalize_signed(tgt);
	src = angle_normalize_signed(src);
	f32 speed = _abs((max_speed - min_speed) * angle_difference(tgt, src) / clmp) + min_speed;
	angle_lerp(src, tgt, speed, dt);
	src = angle_normalize_signed(src);
	f32 dH = angle_difference_signed(src, tgt);
	f32 dCH = clampr(dH, -clmp, clmp);
	src -= dH - dCH;
	return src;
}

template <class T>
IC _matrix4x4<T>& _matrix4x4<T>::rotation(const _quaternion<T>& Q)
{
	T xx = Q.x * Q.x; T yy = Q.y * Q.y; T zz = Q.z * Q.z;
	T xy = Q.x * Q.y; T xz = Q.x * Q.z; T yz = Q.y * Q.z;
	T wx = Q.w * Q.x; T wy = Q.w * Q.y; T wz = Q.w * Q.z;

	_11 = 1 - 2 * (yy + zz);	_12 = 2 * (xy - wz);		_13 = 2 * (xz + wy);		_14 = 0;
	_21 = 2 * (xy + wz);		_22 = 1 - 2 * (xx + zz);	_23 = 2 * (yz - wx);		_24 = 0;
	_31 = 2 * (xz - wy);		_32 = 2 * (yz + wx);		_33 = 1 - 2 * (xx + yy);	_34 = 0;
	_41 = 0;					_42 = 0;					_43 = 0;					_44 = 1;
	return *this;
}

template <class T>
IC _matrix4x4<T>& _matrix4x4<T>::mk_xform(const _quaternion<T>& Q, const Tvector& V)
{
	T xx = Q.x * Q.x; T yy = Q.y * Q.y; T zz = Q.z * Q.z;
	T xy = Q.x * Q.y; T xz = Q.x * Q.z; T yz = Q.y * Q.z;
	T wx = Q.w * Q.x; T wy = Q.w * Q.y; T wz = Q.w * Q.z;

	_11 = 1 - 2 * (yy + zz);	_12 = 2 * (xy - wz);		_13 = 2 * (xz + wy);		_14 = 0;
	_21 = 2 * (xy + wz);		_22 = 1 - 2 * (xx + zz);	_23 = 2 * (yz - wx);		_24 = 0;
	_31 = 2 * (xz - wy);		_32 = 2 * (yz + wx);		_33 = 1 - 2 * (xx + yy);	_34 = 0;
	_41 = V.x;					_42 = V.y;					_43 = V.z;					_44 = 1;
	return *this;
}

#define TRACE_QZERO_TOLERANCE	0.1f
template <class T>
IC _quaternion<T>& _quaternion<T>::set(const _matrix4x4<T>& M)
{
	f32 trace;
	f32 s;

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
		s32 biggest;
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

//----------------------------------------------------------------------------------------------
// Deprecate some features
#ifndef CORE_EXPORTS
#pragma deprecated("MIN","MAX","ABS",fabs,fabsf,sqrt,sqrtf,malloc,free,calloc,realloc,memmove,memset,strdup,strlen,strcmp,sin,cos,sinf,cosf)
#endif
