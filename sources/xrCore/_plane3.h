#pragma once

template <class T>
class _plane3
{
public:
	using TYPE							= T;
	using Self							= _plane3<TYPE>;
	using SelfRef						= Self&;
	using SelfCRef						= const Self&;

	_vector3<TYPE>						n;
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
	ICF SelfRef		build				(const _vector3<TYPE>& v1, const _vector3<TYPE>& v2, const _vector3<TYPE>& v3)
	{
		_vector3<TYPE>					t1;
		_vector3<TYPE>					t2;
		n.crossproduct					(t1.sub(v1, v2), t2.sub(v1, v3)).normalize( );
		d								= -n.dotproduct(v1);
		return *this;
	}
	ICF SelfRef		build				(const _vector3<TYPE>& _p, const _vector3<TYPE>& _n)
	{
		d								= -n.normalize(_n).dotproduct(_p);
		return *this;
	}
	ICF SelfRef		build_precise		(const _vector3<TYPE>& v1, const _vector3<TYPE>& v2, const _vector3<TYPE>& v3)
	{
		_vector3<TYPE>					t1;
		_vector3<TYPE>					t2;
		n.crossproduct					(t1.sub(v1, v2), t2.sub(v1, v3)); exact_normalize(n);
		d								= -n.dotproduct(v1);
		return *this;
	}
	ICF SelfRef		build_unit_normal	(const _vector3<TYPE>& _p, const _vector3<TYPE>& _n)
	{
		VERIFY							(fsimilar(_n.magnitude( ), 1, EPSILON_5));
		d								= -n.set(_n).dotproduct(_p);
		return *this;
	}

	IC SelfRef		project				(_vector3<TYPE>& pdest, _vector3<TYPE>& psrc)
	{
		pdest.mad						(psrc, n, -classify(psrc));
		return *this;
	}
	ICF TYPE		classify			(const _vector3<TYPE>& v) const
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
	IC TYPE			distance			(const _vector3<TYPE>& v)
	{
		return _abs(classify(v));
	}

	IC BOOL			intersectRayDist	(const _vector3<TYPE>& P, const _vector3<TYPE>& D, TYPE& dist)
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
	ICF BOOL		intersectRayPoint	(const _vector3<TYPE>& P, const _vector3<TYPE>& D, _vector3<TYPE>& dest)
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
	IC BOOL			intersect			(const _vector3<TYPE>& u, const _vector3<TYPE>& v, _vector3<TYPE>& isect)
	{
		TYPE							denom;
		_vector3<TYPE>					t;
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
	IC BOOL			intersect_2			(const _vector3<TYPE>& u, const _vector3<TYPE>& v, _vector3<TYPE>& isect)
	{
		TYPE							dist1;
		TYPE							dist2;
		dist1							= n.dotproduct(u) + d;
		dist2							= n.dotproduct(v) + d;
		if (dist1 * dist2 < 0.0f)
		{
			return FALSE;
		}

		_vector3<TYPE>					t;
		t.sub							(v, u);
		isect.mad						(u, t, dist1 / _abs(dist1 - dist2));
		return TRUE;
	}

	IC SelfRef		transform			(_matrix<TYPE>& M)
	{
		// rotate the normal
		M.transform_dir					(n);
		// slide the offset
		d								-= M.c.dotproduct(n);
		return *this;
	}
};

typedef _plane3<f32>	Fplane;
typedef _plane3<double>	Dplane;

template <class T>
BOOL				_valid				(const _plane3<T>& s)
{
	return (_valid(s.n) && _valid(s.d));
}
