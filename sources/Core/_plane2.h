#pragma once

template <class T>
class _plane2
{
public:
	typedef T			TYPE;
	typedef _plane2<TYPE>	Self;
	typedef Self& SelfRef;
	typedef const Self& SelfCRef;

	_vector2<TYPE>	n;
	TYPE			d;

	inline	SelfRef	set(SelfRef P)
	{
		n.set(P.n);
		d = P.d;
		return *this;
	}
	inline	 BOOL 	similar(SelfRef P, TYPE eps_n = EPS, TYPE eps_d = EPS)
	{
		return (n.similar(P.n, eps_n) && (_abs(d - P.d) < eps_d));
	}
	inline	SelfRef	build(const _vector2<TYPE>& _p, const _vector2<TYPE>& _n)
	{
		d = -n.normalize(_n).dotproduct(_p);
		return *this;
	}
	inline	SelfRef	project(_vector2<TYPE>& pdest, _vector2<TYPE>& psrc)
	{
		pdest.mad(psrc, n, -classify(psrc));
		return *this;
	}
	inline	TYPE		classify(const _vector2<TYPE>& v) const
	{
		return n.dotproduct(v) + d;
	}
	inline	SelfRef	normalize( )
	{
		TYPE denom = 1.0f / n.magnitude( );
		n.mul(denom);
		d *= denom;
		return *this;
	}
	inline	TYPE		distance(const _vector2<TYPE>& v)
	{
		return _abs(classify(v));
	}
	inline BOOL intersectRayDist(const _vector2<TYPE>& P, const _vector2<TYPE>& D, TYPE& dist)
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
	inline BOOL intersectRayPoint(const _vector2<TYPE>& P, const _vector2<TYPE>& D, _vector2<TYPE>& dest)
	{
		TYPE numer = classify(P);
		TYPE denom = n.dotproduct(D);

		if (_abs(denom) < EPS_S) return FALSE; // normal is orthogonal to vector3, cant intersect
		else
		{
			F32 dist = -(numer / denom);
			dest.mad(P, D, dist);
			return 		((dist > 0.0f) || fis_zero(dist));
		}
	}
	inline	BOOL	intersect(
		const _vector2<TYPE>& u, const _vector2<TYPE>& v,	// segment
		_vector2<TYPE>& isect)                  // intersection point
	{
		TYPE		denom, dist;
		_vector2<TYPE>		t;

		t.sub(v, u);
		denom = n.dotproduct(t);
		if (_abs(denom) < EPS) return false; // they are parallel

		dist = -(n.dotproduct(u) + d) / denom;
		if (dist < -EPS || dist > 1 + EPS)
		{
			return false;
		}

		isect.mad(u, t, dist);
		return true;
	}

	inline	BOOL	intersect_2(
		const _vector2<TYPE>& u, const _vector2<TYPE>& v,				// segment
		_vector2<TYPE>& isect)						// intersection point
	{
		TYPE		dist1, dist2;
		_vector2<TYPE>		t;

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
};

typedef _plane2<F32>	Fplane2;

template <class T>
BOOL	_valid(const _plane2<T>& s)
{
	return _valid(s.n) && _valid(s.d);
}
