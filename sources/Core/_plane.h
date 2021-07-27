#pragma once

template <class T>
class _plane
{
public:
	typedef T			TYPE;
	typedef _plane<TYPE>	Self;
	typedef Self& SelfRef;
	typedef const Self& SelfCRef;

	_vector3<TYPE>	n;
	TYPE			d;

	inline SelfRef	set(SelfRef P)
	{
		n.set(P.n);
		d = P.d;
		return *this;
	}
	inline BOOL 	similar(SelfRef P, TYPE eps_n = EPS, TYPE eps_d = EPS)
	{
		return (n.similar(P.n, eps_n) && (_abs(d - P.d) < eps_d));
	}
	__forceinline SelfRef	build(const _vector3<TYPE>& v1, const _vector3<TYPE>& v2, const _vector3<TYPE>& v3)
	{
		_vector3<TYPE> t1, t2;
		n.crossproduct(t1.sub(v1, v2), t2.sub(v1, v3)).normalize( );
		d = -n.dotproduct(v1);
		return *this;
	}
	__forceinline SelfRef	build_precise(const _vector3<TYPE>& v1, const _vector3<TYPE>& v2, const _vector3<TYPE>& v3)
	{
		_vector3<TYPE> t1, t2;
		n.crossproduct(t1.sub(v1, v2), t2.sub(v1, v3));
		exact_normalize(n);
		d = -n.dotproduct(v1);
		return *this;
	}
	__forceinline SelfRef	build(const _vector3<TYPE>& _p, const _vector3<TYPE>& _n)
	{
		d = -n.normalize(_n).dotproduct(_p);
		return *this;
	}
	__forceinline SelfRef	build_unit_normal(const _vector3<TYPE>& _p, const _vector3<TYPE>& _n)
	{
		VERIFY(fsimilar(_n.magnitude( ), 1, EPS));
		d = -n.set(_n).dotproduct(_p);
		return *this;
	}
	inline SelfRef	project(_vector3<TYPE>& pdest, _vector3<TYPE>& psrc)
	{
		pdest.mad(psrc, n, -classify(psrc));
		return *this;
	}
	__forceinline TYPE		classify(const _vector3<TYPE>& v) const
	{
		return n.dotproduct(v) + d;
	}
	inline SelfRef	normalize( )
	{
		TYPE denom = 1.0f / n.magnitude( );
		n.mul(denom);
		d *= denom;
		return *this;
	}
	inline TYPE	distance(const _vector3<TYPE>& v)
	{
		return _abs(classify(v));
	}
	inline BOOL intersectRayDist(const _vector3<TYPE>& P, const _vector3<TYPE>& D, TYPE& dist)
	{
		TYPE numer = classify(P);
		TYPE denom = n.dotproduct(D);

		if (_abs(denom) < EPS_S)  // normal is orthogonal to vector3, cant intersect
		{
			return FALSE;
		}

		dist = -(numer / denom);
		return ((dist > 0.f) || fis_zero(dist));
	}
	__forceinline BOOL intersectRayPoint(const _vector3<TYPE>& P, const _vector3<TYPE>& D, _vector3<TYPE>& dest)
	{
		TYPE numer = classify(P);
		TYPE denom = n.dotproduct(D);

		if (_abs(denom) < EPS_S) return FALSE; // normal is orthogonal to vector3, cant intersect
		else
		{
			F32 dist = -(numer / denom);
			dest.mad(P, D, dist);
			return 		((dist > 0.f) || fis_zero(dist));
		}
	}
	inline BOOL	intersect(
		const _vector3<TYPE>& u, const _vector3<TYPE>& v,	// segment
		_vector3<TYPE>& isect)                  // intersection point
	{
		TYPE		denom, dist;
		_vector3<TYPE>		t;

		t.sub(v, u);
		denom = n.dotproduct(t);
		if (_abs(denom) < EPS) return false; // they are parallel

		dist = -(n.dotproduct(u) + d) / denom;
		if (dist < -EPS || dist > 1 + EPS) return false;
		isect.mad(u, t, dist);
		return true;
	}

	inline	BOOL	intersect_2(
		const _vector3<TYPE>& u, const _vector3<TYPE>& v,				// segment
		_vector3<TYPE>& isect)						// intersection point
	{
		TYPE		dist1, dist2;
		_vector3<TYPE>		t;

		dist1 = n.dotproduct(u) + d;
		dist2 = n.dotproduct(v) + d;

		if (dist1 * dist2 < 0.0f)
		{
			return false;
		}

		t.sub(v, u);
		isect.mad(u, t, dist1 / _abs(dist1 - dist2));

		return true;
	}
	inline SelfRef	transform(_matrix<TYPE>& M)
	{
		// rotate the normal
		M.transform_dir(n);
		// slide the offset
		d -= M.c.dotproduct(n);
		return *this;
	}
};

typedef _plane<F32>	Fplane;

template <class T>
BOOL	_valid(const _plane<T>& s)
{
	return _valid(s.n) && _valid(s.d);
}
