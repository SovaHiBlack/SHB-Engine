#pragma once

template <class T>
class _vector4
{
public:
	using TYPE								= T;
	using Self								= _vector4<TYPE>;
	using SelfRef							= Self&;
	using SelfCRef							= const Self&;

	TYPE									x;
	TYPE									y;
	TYPE									z;
	TYPE									w;

	inline TYPE&		operator []			(int i)
	{
		return *((TYPE*) this + i);
	}
	inline TYPE&		operator []			(int i) const
	{
		return *((TYPE*) this + i);
	}

	inline SelfRef		set					(TYPE _x, TYPE _y, TYPE _z, TYPE _w = 1)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
		return *this;
	}
	inline SelfRef		set					(SelfCRef v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	inline SelfRef		add					(SelfCRef v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	inline SelfRef		add					(TYPE s)
	{
		x += s;
		y += s;
		z += s;
		w += s;
		return *this;
	}
	inline SelfRef		add					(SelfCRef a, SelfCRef v)
	{
		x = a.x + v.x;
		y = a.y + v.y;
		z = a.z + v.z;
		w = a.w + v.w;
		return *this;
	}
	inline SelfRef		add					(SelfCRef a, TYPE s)
	{
		x = a.x + s;
		y = a.y + s;
		z = a.z + s;
		w = a.w + s;
		return *this;
	}

	inline SelfRef		sub					(TYPE _x, TYPE _y, TYPE _z, TYPE _w = 1)
	{
		x -= _x;
		y -= _y;
		z -= _z;
		w -= _w;
		return *this;
	}
	inline SelfRef		sub					(SelfCRef v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	inline SelfRef		sub					(TYPE s)
	{
		x -= s;
		y -= s;
		z -= s;
		w -= s;
		return *this;
	}
	inline SelfRef		sub					(SelfCRef a, SelfCRef v)
	{
		x = a.x - v.x;
		y = a.y - v.y;
		z = a.z - v.z;
		w = a.w - v.w;
		return *this;
	}
	inline SelfRef		sub					(SelfCRef a, TYPE s)
	{
		x = a.x - s;
		y = a.y - s;
		z = a.z - s;
		w = a.w - s;
		return *this;
	}

	inline SelfRef		mul					(TYPE _x, TYPE _y, TYPE _z, TYPE _w = 1)
	{
		x *= _x;
		y *= _y;
		z *= _z;
		w *= _w;
		return *this;
	}
	inline SelfRef		mul					(SelfCRef v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}
	inline SelfRef		mul					(TYPE s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}
	inline SelfRef		mul					(SelfCRef a, SelfCRef v)
	{
		x = a.x * v.x;
		y = a.y * v.y;
		z = a.z * v.z;
		w = a.w * v.w;
		return *this;
	}
	inline SelfRef		mul					(SelfCRef a, TYPE s)
	{
		x = a.x * s;
		y = a.y * s;
		z = a.z * s;
		w = a.w * s;
		return *this;
	}

	inline SelfRef		div					(SelfCRef v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}
	inline SelfRef		div					(TYPE s)
	{
		x /= s;
		y /= s;
		z /= s;
		w /= s;
		return *this;
	}
	inline SelfRef		div					(SelfCRef a, SelfCRef v)
	{
		x = a.x / v.x;
		y = a.y / v.y;
		z = a.z / v.z;
		w = a.w / v.w;
		return *this;
	}
	inline SelfRef		div					(SelfCRef a, TYPE s)
	{
		x = a.x / s;
		y = a.y / s;
		z = a.z / s;
		w = a.w / s;
		return *this;
	}

	inline BOOL			similar				(SelfCRef v, T E = EPS_L)
	{
		return _abs(x - v.x) < E && _abs(y - v.y) < E && _abs(z - v.z) < E && _abs(w - v.w) < E;
	}

	inline TYPE			magnitude_sqr		( )
	{
		return x * x + y * y + z * z + w * w;
	}
	inline TYPE			magnitude			( )
	{
		return _sqrt(magnitude_sqr( ));
	}
	inline SelfRef		normalize			( )
	{
		return mul(1 / magnitude( ));
	}

	inline SelfRef		lerp				(SelfCRef p1, SelfCRef p2, TYPE t)
	{
		TYPE invt = 1.0f - t;
		x = p1.x * invt + p2.x * t;
		y = p1.y * invt + p2.y * t;
		z = p1.z * invt + p2.z * t;
		w = p1.w * invt + p2.w * t;
		return *this;
	}
};

using Ivector4								= _vector4<int>;
using Fvector4								= _vector4<float>;

template <class T>
BOOL					_valid				(const _vector4<T>& v)
{
	return _valid((T) v.x) && _valid((T) v.y) && _valid((T) v.z) && _valid((T) v.w);
}
