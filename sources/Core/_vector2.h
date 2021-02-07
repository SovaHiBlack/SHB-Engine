#pragma once

template <class T>
class _vector2
{
public:
	using TYPE									= T;
	using Self									= _vector2<TYPE>;
	using SelfRef								= Self&;
	using SelfCRef								= const Self&;

	TYPE										x;
	TYPE										y;

	inline SelfRef		set						(float _u, float _v)
	{
		x = TYPE(_u);
		y = TYPE(_v);
		return *this;
	}
	inline SelfRef		set						(double _u, double _v)
	{
		x = TYPE(_u);
		y = TYPE(_v);
		return *this;
	}
	inline SelfRef		set						(int _u, int _v)
	{
		x = TYPE(_u);
		y = TYPE(_v);
		return *this;
	}
	inline SelfRef		set						(SelfCRef p)
	{
		x = p.x;
		y = p.y;
		return *this;
	}
	inline SelfRef		abs						(SelfCRef p)
	{
		x = _abs(p.x);
		y = _abs(p.y);
		return *this;
	}
	inline SelfRef		min						(SelfCRef p)
	{
		x = _min(x, p.x);
		y = _min(y, p.y);
		return *this;
	}
	inline SelfRef		min						(TYPE _x, TYPE _y)
	{
		x = _min(x, _x);
		y = _min(y, _y);
		return *this;
	}
	inline SelfRef		max						(SelfCRef p)
	{
		x = _max(x, p.x);
		y = _max(y, p.y);
		return *this;
	}
	inline SelfRef		max						(TYPE _x, TYPE _y)
	{
		x = _max(x, _x);
		y = _max(y, _y);
		return *this;
	}
	inline SelfRef		sub						(TYPE p)
	{
		x -= p;
		y -= p;
		return *this;
	}
	inline SelfRef		sub						(SelfCRef p)
	{
		x -= p.x;
		y -= p.y;
		return *this;
	}
	inline SelfRef		sub						(SelfCRef p1, SelfCRef p2)
	{
		x = p1.x - p2.x;
		y = p1.y - p2.y;
		return *this;
	}
	inline SelfRef		sub						(SelfCRef p, float d)
	{
		x = p.x - d;
		y = p.y - d;
		return *this;
	}
	inline SelfRef		add						(TYPE p)
	{
		x += p;
		y += p;
		return *this;
	}
	inline SelfRef		add						(SelfCRef p)
	{
		x += p.x;
		y += p.y;
		return *this;
	}
	inline SelfRef		add						(SelfCRef p1, SelfCRef p2)
	{
		x = p1.x + p2.x;
		y = p1.y + p2.y;
		return *this;
	}
	inline SelfRef		add						(SelfCRef p, float d)
	{
		x = p.x + d;
		y = p.y + d;
		return *this;
	}
	inline SelfRef		mul						(const TYPE s)
	{
		x *= s;
		y *= s;
		return *this;
	}
	inline SelfRef		mul						(Self& p)
	{
		x *= p.x;
		y *= p.y;
		return *this;
	}
	inline SelfRef		div						(const TYPE s)
	{
		x /= s;
		y /= s;
		return *this;
	}
	inline SelfRef		rot90					( )
	{
		float t = -x;
		x = y;
		y = t;
		return *this;
	}
	inline SelfRef		cross					(Self& D)
	{
		x = D.y;
		y = -D.x;
		return *this;
	}
	inline TYPE			dot						(Self& p)
	{
		return x * p.x + y * p.y;
	}
	inline TYPE			dot						(SelfCRef p) const
	{
		return x * p.x + y * p.y;
	}
	inline SelfRef		norm					( )
	{
		float m = _sqrt(x * x + y * y);
		x /= m;
		y /= m;
		return *this;
	}
	inline SelfRef		norm_safe				( )
	{
		float m = _sqrt(x * x + y * y);
		if (m)
		{
			x /= m;
			y /= m;
		}

		return *this;
	}
	inline TYPE			distance_to				(SelfCRef p) const
	{
		return _sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
	}
	inline TYPE			square_magnitude		( ) const
	{
		return x * x + y * y;
	}
	inline TYPE			magnitude				( ) const
	{
		return _sqrt(square_magnitude( ));
	}

	inline SelfRef		mad						(SelfCRef p, SelfCRef d, TYPE r)
	{
		x = p.x + d.x * r;
		y = p.y + d.y * r;
		return *this;
	}
	inline Self			Cross					( )
	{
		// vector3 orthogonal to (x,y) is (y,-x)
		Self kCross;
		kCross.x = y;
		kCross.y = -x;
		return kCross;
	}

	inline bool			similar					(Self& p, TYPE eu, TYPE ev) const
	{
		return _abs(x - p.x) < eu && _abs(y - p.y) < ev;
	}

	inline bool			similar					(SelfCRef p, float E = EPS_L) const
	{
		return _abs(x - p.x) < E && _abs(y - p.y) < E;
	}

	// average arithmetic
	inline SelfRef		averageA				(Self& p1, Self& p2)
	{
		x = (p1.x + p2.x) * 0.5f;
		y = (p1.y + p2.y) * 0.5f;
		return *this;
	}
	// average geometric
	inline SelfRef		averageG				(Self& p1, Self& p2)
	{
		x = _sqrt(p1.x * p2.x);
		y = _sqrt(p1.y * p2.y);
		return *this;
	}

	TYPE&				operator []				(int i) const
	{
		// assert:  0 <= i < 2; x and y are packed into 2*sizeof(float) bytes
		return (TYPE&) *(&x + i);
	}

	inline SelfRef		normalize				( )
	{
		return norm( );
	}
	inline SelfRef		normalize_safe			( )
	{
		return norm_safe( );
	}
	inline SelfRef		normalize				(SelfCRef v)
	{
		float m = _sqrt(v.x * v.x + v.y * v.y);
		x = v.x / m;
		y = v.y / m;
		return *this;
	}
	inline SelfRef		normalize_safe			(SelfCRef v)
	{
		float m = _sqrt(v.x * v.x + v.y * v.y);
		if (m)
		{
			x = v.x / m;
			y = v.y / m;
		}

		return *this;
	}
	inline float		dotproduct				(SelfCRef p) const
	{
		return dot(p);
	}
	inline float		crossproduct			(SelfCRef p) const
	{
		return y * p.x - x * p.y;
	}
	inline float		getH					( ) const
	{
		if (fis_zero(y))
		{
			if (fis_zero(x))
			{
				return 0.0f;
			}
			else
			{
				return ((x > 0.0f) ? -PI_DIV_2 : PI_DIV_2);
			}
		}
		else
		{
			if (y < 0.0f)
			{
				return -(atanf(x / y) - PI);
			}
			else
			{
				return -atanf(x / y);
			}
		}
	}
};

using Ivector2									= _vector2<int>;
using Fvector2									= _vector2<float>;

template <class T>
BOOL					_valid					(const _vector2<T>& v)
{
	return _valid((T) v.x) && _valid((T) v.y);
}
