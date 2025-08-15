#pragma once

template <class T>
class _vector4
{
public:
	using TYPE							= T;
	using Self							= _vector4<TYPE>;
	using SelfRef						= Self&;
	using SelfCRef						= const Self&;

	TYPE								x;
	TYPE								y;
	TYPE								z;
	TYPE								w;

	IC TYPE&		operator[]			(s32 i)
	{
		return *((TYPE*)this + i);
	}
	IC TYPE&		operator[]			(s32 i) const
	{
		return *((TYPE*)this + i);
	}

	IC SelfRef		set					(TYPE _x, TYPE _y, TYPE _z, TYPE _w = 1)
	{
		x								= _x;
		y								= _y;
		z								= _z;
		w								= _w;
		return *this;
	}
	IC SelfRef		set					(SelfCRef v)
	{
		x								= v.x;
		y								= v.y;
		z								= v.z;
		w								= v.w;
		return *this;
	}

	IC SelfRef		add					(SelfCRef v)
	{
		x								+= v.x;
		y								+= v.y;
		z								+= v.z;
		w								+= v.w;
		return *this;
	}
	IC SelfRef		add					(TYPE s)
	{
		x								+= s;
		y								+= s;
		z								+= s;
		w								+= s;
		return *this;
	}
	IC SelfRef		add					(SelfCRef a, SelfCRef v)
	{
		x								= a.x + v.x;
		y								= a.y + v.y;
		z								= a.z + v.z;
		w								= a.w + v.w;
		return *this;
	}
	IC SelfRef		add					(SelfCRef a, TYPE s)
	{
		x								= a.x + s;
		y								= a.y + s;
		z								= a.z + s;
		w								= a.w + s;
		return *this;
	}

	IC SelfRef		sub					(TYPE _x, TYPE _y, TYPE _z, TYPE _w = 1)
	{
		x								-= _x;
		y								-= _y;
		z								-= _z;
		w								-= _w;
		return *this;
	}
	IC SelfRef		sub					(SelfCRef v)
	{
		x								-= v.x;
		y								-= v.y;
		z								-= v.z;
		w								-= v.w;
		return *this;
	}
	IC SelfRef		sub					(TYPE s)
	{
		x								-= s;
		y								-= s;
		z								-= s;
		w								-= s;
		return *this;
	}
	IC SelfRef		sub					(SelfCRef a, SelfCRef v)
	{
		x								= a.x - v.x;
		y								= a.y - v.y;
		z								= a.z - v.z;
		w								= a.w - v.w;
		return *this;
	}
	IC SelfRef		sub					(SelfCRef a, TYPE s)
	{
		x								= a.x - s;
		y								= a.y - s;
		z								= a.z - s;
		w								= a.w - s;
		return *this;
	}

	IC SelfRef		mul					(TYPE _x, TYPE _y, TYPE _z, TYPE _w = 1)
	{
		x								*= _x;
		y								*= _y;
		z								*= _z;
		w								*= _w;
		return *this;
	}
	IC SelfRef		mul					(SelfCRef v)
	{
		x								*= v.x;
		y								*= v.y;
		z								*= v.z;
		w								*= v.w;
		return *this;
	}
	IC SelfRef		mul					(TYPE s)
	{
		x								*= s;
		y								*= s;
		z								*= s;
		w								*= s;
		return *this;
	}
	IC SelfRef		mul					(SelfCRef a, SelfCRef v)
	{
		x								= a.x * v.x;
		y								= a.y * v.y;
		z								= a.z * v.z;
		w								= a.w * v.w;
		return *this;
	}
	IC SelfRef		mul					(SelfCRef a, T s)
	{
		x								= a.x * s;
		y								= a.y * s;
		z								= a.z * s;
		w								= a.w * s;
		return *this;
	}

	IC SelfRef		div					(SelfCRef v)
	{
		x								/= v.x;
		y								/= v.y;
		z								/= v.z;
		w								/= v.w;
		return *this;
	}
	IC SelfRef		div					(TYPE s)
	{
		x								/= s;
		y								/= s;
		z								/= s;
		w								/= s;
		return *this;
	}
	IC SelfRef		div					(SelfCRef a, SelfCRef v)
	{
		x								= a.x / v.x;
		y								= a.y / v.y;
		z								= a.z / v.z;
		w								= a.w / v.w;
		return *this;
	}
	IC SelfRef		div					(SelfCRef a, TYPE s)
	{
		x								= a.x / s;
		y								= a.y / s;
		z								= a.z / s;
		w								= a.w / s;
		return *this;
	}

	IC BOOL			similar				(SelfCRef v, TYPE E = EPSILON_3)
	{
		return (_abs(x - v.x) < E && _abs(y - v.y) < E && _abs(z - v.z) < E && _abs(w - v.w) < E);
	}

	IC TYPE			magnitude_sqr		( )
	{
		return (x * x + y * y + z * z + w * w);
	}
	IC TYPE			magnitude			( )
	{
		return _sqrt(magnitude_sqr( ));
	}
	IC SelfRef		normalize			( )
	{
		return mul(1 / magnitude( ));
	}

	IC SelfRef		lerp				(SelfCRef p1, SelfCRef p2, TYPE t)
	{
		TYPE invt						= 1.0f - t;
		x								= p1.x * invt + p2.x * t;
		y								= p1.y * invt + p2.y * t;
		z								= p1.z * invt + p2.z * t;
		w								= p1.w * invt + p2.w * t;
		return *this;
	}
};

using iVector4							= _vector4<s32>;
using fVector4							= _vector4<f32>;

template <class T>
BOOL				_valid				(const _vector4<T>& v)
{
	return (_valid((T)v.x) && _valid((T)v.y) && _valid((T)v.z) && _valid((T)v.w));
}
