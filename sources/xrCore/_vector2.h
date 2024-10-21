#pragma once

// Вектор на плоскости
template <class T>
struct _vector2
{
public:
	using TYPE							= T;
	using Self							= _vector2<TYPE>;
	using SelfRef						= Self&;
	using SelfCRef						= const Self&;

	TYPE								x;
	TYPE								y;

	IC SelfRef		set					(TYPE _u, TYPE _v)
	{
		x = TYPE(_u);
		y = TYPE(_v);
		return *this;
	}
	IC SelfRef		set					(SelfCRef p)
	{
		x = p.x;
		y = p.y;
		return *this;
	}

	IC SelfRef		abs					(SelfCRef p)
	{
		x = _abs(p.x);
		y = _abs(p.y);
		return *this;
	}

	IC SelfRef		min					(SelfCRef p)
	{
		x = _min(x, p.x);
		y = _min(y, p.y);
		return *this;
	}
	IC SelfRef		min					(TYPE _x, TYPE _y)
	{
		x = _min(x, _x);
		y = _min(y, _y);
		return *this;
	}

	IC SelfRef		max					(SelfCRef p)
	{
		x = _max(x, p.x);
		y = _max(y, p.y);
		return *this;
	}
	IC SelfRef		max					(TYPE _x, TYPE _y)
	{
		x = _max(x, _x);
		y = _max(y, _y);
		return *this;
	}

	IC SelfRef		sub					(TYPE p)
	{
		x -= p;
		y -= p;
		return *this;
	}
	IC SelfRef		sub					(SelfCRef p)
	{
		x -= p.x;
		y -= p.y;
		return *this;
	}
	IC SelfRef		sub					(SelfCRef p1, SelfCRef p2)
	{
		x = p1.x - p2.x;
		y = p1.y - p2.y;
		return *this;
	}
	IC SelfRef		sub					(SelfCRef p, f32 d)
	{
		x = p.x - d;
		y = p.y - d;
		return *this;
	}

	IC SelfRef		add					(TYPE p)
	{
		x += p;
		y += p;
		return *this;
	}
	IC SelfRef		add					(SelfCRef p)
	{
		x += p.x;
		y += p.y;
		return *this;
	}
	IC SelfRef		add					(SelfCRef p1, SelfCRef p2)
	{
		x = p1.x + p2.x;
		y = p1.y + p2.y;
		return *this;
	}
	IC SelfRef		add					(SelfCRef p, f32 d)
	{
		x = p.x + d;
		y = p.y + d;
		return *this;
	}

	IC SelfRef		mul					(const TYPE s)
	{
		x *= s;
		y *= s;
		return *this;
	}
	IC SelfRef		mul					(Self& p)
	{
		x *= p.x;
		y *= p.y;
		return *this;
	}

	IC SelfRef		div					(const TYPE s)
	{
		x /= s;
		y /= s;
		return *this;
	}

	IC SelfRef		rot90				( )
	{
		f32 t = -x;
		x = y;
		y = t;
		return *this;
	}
	IC SelfRef		cross				(Self& D)
	{
		x = D.y;
		y = -D.x;
		return *this;
	}

	IC TYPE			dot					(Self& p)
	{
		return x * p.x + y * p.y;
	}
	IC TYPE			dot					(SelfCRef p) const
	{
		return (x * p.x + y * p.y);
	}

	IC SelfRef		norm				( )
	{
		f32 m = _sqrt(x * x + y * y);
		x /= m;
		y /= m;
		return *this;
	}
	IC SelfRef		norm_safe			( )
	{
		f32 m = _sqrt(x * x + y * y);
		if (m)
		{
			x /= m;
			y /= m;
		}

		return *this;
	}

	IC TYPE			distance_to			(SelfCRef p) const
	{
		return _sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
	}
	IC TYPE			square_magnitude	( ) const
	{
		return (x * x + y * y);
	}
	IC TYPE			magnitude			( ) const
	{
		return _sqrt(square_magnitude( ));
	}

	IC SelfRef		mad					(SelfCRef p, SelfCRef d, TYPE r)
	{
		x = p.x + d.x * r;
		y = p.y + d.y * r;
		return *this;
	}

	IC Self			Cross				( )
	{
		// vector3 orthogonal to (x,y) is (y,-x)
		Self kCross;
		kCross.x = y;
		kCross.y = -x;
		return kCross;
	}

	IC bool			similar				(Self& p, TYPE eu, TYPE ev) const
	{
		return (_abs(x - p.x) < eu && _abs(y - p.y) < ev);
	}
	IC bool			similar				(SelfCRef p, f32 E = EPS_L) const
	{
		return (_abs(x - p.x) < E && _abs(y - p.y) < E);
	}

	// average arithmetic
	IC SelfRef		averageA			(Self& p1, Self& p2)
	{
		x = (p1.x + p2.x) * 0.5f;
		y = (p1.y + p2.y) * 0.5f;
		return *this;
	}
	// average geometric
	IC SelfRef		averageG			(Self& p1, Self& p2)
	{
		x = _sqrt(p1.x * p2.x);
		y = _sqrt(p1.y * p2.y);
		return *this;
	}

	TYPE&			operator[]			(s32 i) const
	{
		// assert: 0 <= i < 2; x and y are packed into 2*sizeof(f32) bytes
		return (TYPE&)*(&x + i);
	}

	IC SelfRef		normalize			( )
	{
		return norm( );
	}
	IC SelfRef		normalize_safe		( )
	{
		return norm_safe( );
	}
	IC SelfRef		normalize			(SelfCRef v)
	{
		f32 m = _sqrt(v.x * v.x + v.y * v.y);
		x = v.x / m;
		y = v.y / m;
		return *this;
	}
	IC SelfRef		normalize_safe		(SelfCRef v)
	{
		f32 m = _sqrt(v.x * v.x + v.y * v.y);
		if (m)
		{
			x = v.x / m;
			y = v.y / m;
		}

		return *this;
	}

	IC f32			dotproduct			(SelfCRef p) const
	{
		return dot(p);
	}
	IC f32			crossproduct		(SelfCRef p) const
	{
		return (y * p.x - x * p.y);
	}
	IC f32			getH				( ) const
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
				return (-(atanf(x / y) - PI));
			}
			else
			{
				return (-atanf(x / y));
			}
		}
	}
};

using iVector2							= _vector2<s32>;
using fVector2							= _vector2<f32>;

typedef _vector2<f32>		Fvector2;



template <class T>
BOOL				_valid				(const _vector2<T>& v)
{
	return (_valid((T)v.x) && _valid((T)v.y));
}
