#pragma once

template <class T>
class _plane2
{
public:
	typedef T			TYPE;
	typedef _plane2<T>	Self;
	typedef Self& SelfRef;
	typedef const Self& SelfCRef;
public:
	_vector2<T>	n;
	T			d;
public:
	IC	SelfRef	set(Self& P)
	{
		n.set(P.n);
		d = P.d;
		return *this;
	}
	IC	 BOOL 	similar(Self& P, T eps_n = EPSILON_5, T eps_d = EPSILON_5)
	{
		return (n.similar(P.n, eps_n) && (_abs(d - P.d) < eps_d));
	}
	IC	SelfRef	build(const _vector2<T>& _p, const _vector2<T>& _n)
	{
		d = -n.normalize(_n).dotproduct(_p);
		return *this;
	}
	IC	SelfRef	project(_vector2<T>& pdest, _vector2<T>& psrc)
	{
		pdest.mad(psrc, n, -classify(psrc));
		return *this;
	}
	IC	T		classify(const _vector2<T>& v) const
	{
		return n.dotproduct(v) + d;
	}
	IC	SelfRef	normalize()
	{
		T denom = 1.f / n.magnitude();
		n.mul(denom);
		d *= denom;
		return *this;
	}
	IC	T		distance(const _vector2<T>& v)
	{
		return _abs(classify(v));
	}
	IC BOOL intersectRayDist(const _vector2<T>& P, const _vector2<T>& D, T& dist)
	{
		T numer = classify(P);
		T denom = n.dotproduct(D);

		if (_abs(denom) < EPSILON_7)  // normal is orthogonal to vector3, cant intersect
			return FALSE;

		dist = -(numer / denom);
		return ((dist > 0.f) || fis_zero(dist));
	}
	IC BOOL intersectRayPoint(const _vector2<T>& P, const _vector2<T>& D, _vector2<T>& dest)
	{
		T numer = classify(P);
		T denom = n.dotproduct(D);

		if (_abs(denom) < EPSILON_7) return FALSE; // normal is orthogonal to vector3, cant intersect
		else
		{
			f32 dist = -(numer / denom);
			dest.mad(P, D, dist);
			return 		((dist > 0.f) || fis_zero(dist));
		}
	}
	IC	BOOL	intersect(
		const _vector2<T>& u, const _vector2<T>& v,	// segment
		_vector2<T>& isect)                  // intersection point
	{
		T		denom, dist;
		_vector2<T>		t;

		t.sub(v, u);
		denom = n.dotproduct(t);
		if (_abs(denom) < EPSILON_5) return false; // they are parallel

		dist = -(n.dotproduct(u) + d) / denom;
		if (dist < -EPSILON_5 || dist > 1 + EPSILON_5) return false;
		isect.mad(u, t, dist);
		return true;
	}

	IC	BOOL	intersect_2(
		const _vector2<T>& u, const _vector2<T>& v,				// segment
		_vector2<T>& isect)						// intersection point
	{
		T		dist1, dist2;
		_vector2<T>		t;

		dist1 = n.dotproduct(u) + d;
		dist2 = n.dotproduct(v) + d;

		if (dist1 * dist2 < 0.0f)
			return false;

		t.sub(v, u);
		isect.mad(u, t, dist1 / _abs(dist1 - dist2));

		return true;
	}
};

typedef _plane2<f32>	Fplane2;
typedef _plane2<double>	Dplane2;

template <class T>
BOOL	_valid(const _plane2<T>& s)
{
	return _valid(s.n) && _valid(s.d);
}
