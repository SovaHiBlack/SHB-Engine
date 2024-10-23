#pragma once

template <class T>
class _plane2
{
public:
	using TYPE							= T;
	using Self							= _plane2<TYPE>;
	using SelfRef						= Self&;
	using SelfCRef						= const Self&;

	_vector2<TYPE>						n;
	TYPE								d;

	IC SelfRef		set					(Self& P)
	{
		n.set							(P.n);
		d								= P.d;
		return *this;
	}
	IC BOOL			similar				(Self& P, TYPE eps_n = EPSILON_5, TYPE eps_d = EPSILON_5)
	{
		return (n.similar(P.n, eps_n) && (_abs(d - P.d) < eps_d));
	}
	IC SelfRef		build				(const _vector2<TYPE>& _p, const _vector2<TYPE>& _n)
	{
		d								= -n.normalize(_n).dotproduct(_p);
		return *this;
	}

	IC SelfRef		project				(_vector2<TYPE>& pdest, _vector2<TYPE>& psrc)
	{
		pdest.mad						(psrc, n, -classify(psrc));
		return *this;
	}
	IC TYPE			classify			(const _vector2<TYPE>& v) const
	{
		return (n.dotproduct(v) + d);
	}
	IC SelfRef		normalize			( )
	{
		TYPE denom						= 1.0f / n.magnitude( );
		n.mul							(denom);
		d								*= denom;
		return *this;
	}
	IC TYPE			distance			(const _vector2<TYPE>& v)
	{
		return _abs(classify(v));
	}

	IC BOOL			intersectRayDist	(const _vector2<TYPE>& P, const _vector2<TYPE>& D, TYPE& dist)
	{
		TYPE numer						= classify(P);
		TYPE denom						= n.dotproduct(D);
		if (_abs(denom) < EPSILON_7) // normal is orthogonal to vector3, cant intersect
		{
			return FALSE;
		}

		dist							= -(numer / denom);
		return ((dist > 0.0f) || fis_zero(dist));
	}
	IC BOOL			intersectRayPoint	(const _vector2<TYPE>& P, const _vector2<TYPE>& D, _vector2<TYPE>& dest)
	{
		TYPE numer						= classify(P);
		TYPE denom						= n.dotproduct(D);
		if (_abs(denom) < EPSILON_7) // normal is orthogonal to vector3, cant intersect
		{
			return FALSE;
		}
		else
		{
			f32 dist					= -(numer / denom);
			dest.mad					(P, D, dist);
			return ((dist > 0.0f) || fis_zero(dist));
		}
	}
	IC BOOL			intersect			(const _vector2<TYPE>& u, const _vector2<TYPE>& v, _vector2<TYPE>& isect)// u, v - segment; isect - intersection point
	{
		TYPE							denom;
		_vector2<TYPE>					t;
		t.sub							(v, u);
		denom							= n.dotproduct(t);
		if (_abs(denom) < EPSILON_5) // they are parallel
		{
			return FALSE;
		}

		TYPE							dist;
		dist							= -(n.dotproduct(u) + d) / denom;
		if (dist < -EPSILON_5 || dist > 1 + EPSILON_5)
		{
			return FALSE;
		}

		isect.mad						(u, t, dist);
		return TRUE;
	}
	IC BOOL			intersect_2			(const _vector2<T>& u, const _vector2<T>& v, _vector2<T>& isect)// u, v - segment; isect - intersection point
	{
		TYPE							dist1;
		TYPE							dist2;
		dist1							= n.dotproduct(u) + d;
		dist2							= n.dotproduct(v) + d;
		if (dist1 * dist2 < 0.0f)
		{
			return FALSE;
		}

		_vector2<T>						t;
		t.sub							(v, u);
		isect.mad						(u, t, dist1 / _abs(dist1 - dist2));
		return TRUE;
	}
};

using fPlane2 = _plane2<f32>;

template <class T>
BOOL				_valid				(const _plane2<T>& s)
{
	return (_valid(s.n) && _valid(s.d));
}
