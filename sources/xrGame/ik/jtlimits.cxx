#include "stdafx.h"
#include "aint.h"
#include "jtlimits.h"

void SimpleJtLimit::init(s32 jt_type,
						 f32 a, f32 b, f32 c,
						 f32 low, f32 high)
{
	psi.Reset(a, b, c);
	limits.Set(low, high);
	sin_low = _sin(low);
	sin_high = _sin(high);

	switch (jt_type)
	{
		case SinJtLimit:
		case CosJtLimit:
			type = jt_type;
			break;
		default:
			printf("Invalid Joint Type passed to SimpleJtLimit %d\n", jt_type);
			exit(0);
			break;
	}
}

f32 SimpleJtLimit::theta1(f32 v) const
{
	return type == SinJtLimit ? asin1(psi.eval(v)) : acos1(psi.eval(v));
}

f32 SimpleJtLimit::theta2(f32 v) const
{
	return type == SinJtLimit ? asin2(psi.eval(v)) : acos2(psi.eval(v));
}

f32 SimpleJtLimit::theta(s32 family, f32 v) const
{
	return family == 1 ? theta1(v) : theta2(v);
}

f32 SimpleJtLimit::theta1_d_aux(f32 v, f32 delta) const
{
	f32 d = psi.eval(v);
	f32 d_psi = psi.deriv(v);
	const f32 eps = 1e-8f;

	// denominator is 0 take average of derivative in neighborhood
	if (iszero(d, eps))
	{
		return (theta1_d_aux(v + delta, delta * 10.0f) +
				theta1_d_aux(v - delta, delta * 10.0f)) / 2.0f;
	}

	d = _sqrt(1 - d * d);

	return (type == SinJtLimit) ? (d_psi / d) : (-d_psi / d);
}

f32 SimpleJtLimit::theta1_d(f32 v) const
{
	const f32 delta = 1e-5f;
	return theta1_d_aux(v, delta);
}

f32 SimpleJtLimit::theta2_d(f32 v) const
{
	return -theta1_d(v);
}

f32 SimpleJtLimit::theta_d(s32 family, f32 v) const
{
	return family == 1 ? theta1_d(v) : theta2_d(v);
}


s32 SimpleJtLimit::Solve(s32 family, f32 v, f32 sin_v,
						 f32 solns[2]) const
{
	s32 n = -1;

	v = angle_normalize(v);
	if (type == SinJtLimit)
	{
		if (family == 1)
		{
			// Check if v is in range 0..pi/2 or 3pi/2..2pi
			if ((v > M_PI / 2.0) && (v < 3 * M_PI / 2.0))
				return 0;
		}
		else
		{
			// Check if v is in range pi/2..3*pi/2
			if ((v < M_PI / 2.0) || (v > 3 * M_PI / 2.0))
				return 0;
		}
		n = psi.solve(sin_v, solns);
	}
	else
		printf("Solve not implemented for cos type\n");
	VERIFY(n != -1);
	return n;
}

s32 SimpleJtLimit::Discontinuity(s32 family, f32 x[2]) const
{
	s32 n = -1;
	if (type == SinJtLimit)
	{
		if (family == 2)
			n = 0;

		// Discontinuity occurs when theta = 0 or 2*M_PI => sin(theta) = 0
		else
			n = psi.roots(x);

	}
	else
		printf("Discontinuity not implemented for cos type\n");
	VERIFY(n != -1);
	return n;
}

void sort(s32 up, f32 a[ ])
{
	s32 j;
	s32 lo = 0;

	up--;
	while (up)
	{
		j = lo;
		for (s32 i = lo; i < up; i++)
			if (a[i] > a[i + 1])
			{
				f32 temp = a[i];
				a[i] = a[i + 1];
				a[i + 1] = temp;
				j = i;
			}
		up = j;
	}
}

s32 sort_intersections(s32 m, f32* temp, f32 psi0, f32 psi1)
{
	s32 i;
	s32 j;

	sort(m, temp);

	// eliminate all intersections out of range [psi0,psi1]
	i = 0;
	for (j = 0; j < m; j++)
	{
		if (temp[j] > psi1)
			break;
		if (temp[j] >= psi0)
			temp[i++] = temp[j];
	}

	return i;
}


//
// Returns the intervals between psi0 to psi1 
// for which low <= theta(family,psi) <= high
//
void SimpleJtLimit::clip(s32 family,
						 f32 psi0, f32 psi1,
						 f32 low, f32 high,
						 AngleIntList& a) const
{
	f32 psis[6], * temp;
	s32 i;
	s32 m;

	// Inspect intervals (psi0,ip1),(ip1,ip2),...(ipn,psi1) where ip's
	// are the intersection points of the curve with low and high

	temp = psis + 1;
	m = Solve(family, low, sin_low, temp);
	m += Solve(family, high, sin_high, temp + m);

	m = sort_intersections(m, temp, psi0, psi1);
	psis[0] = psi0;
	psis[m + 1] = psi1;
	m += 2;

	// Check the midpoint of each interval to see if it is a valid
	// interval or invalid one

	for (i = 0; i < m - 1; i++)
	{
		f32 t = theta(family, (psis[i] + psis[i + 1]) / 2.0f);
		if ((t >= low) && (t <= high))
			a.Add(psis[i], psis[i + 1]);
	}
}

//
// Given a psi equation that represents the sin of an angle
// ie: sin(theta) = a*cos(psi) + b*sin(psi) + xi 
// and a set of joint limits on theta find the set of 
// psi intervals that satisfy the joint limits
// 
// psi1 contain the intervals of psi such that
//
// theta = asin1(a*cos(psi) + b*sin(psi) + xi)
// 
// satisfy the joint limits
//
// psi2 contain the intervals of psi such that
//
// theta = asin2(a*cos(psi) + b*sin(psi) + xi)
//

void SimpleJtLimit::PsiLimits(AngleIntList& psi1,
							  AngleIntList& psi2) const
{
	const f32 eps = AINT_BIG_EPSILON / 5.0f;
	psi1.Clear( );
	psi2.Clear( );

	// Split the joint limits into two families of joint limits

	if (type == SinJtLimit)
	{
		f32 y[4];
		s32   n;
		f32 low = eps;
		f32 high = 2 * M_PI - eps;

		y[0] = low;
		n = Discontinuity(1, y + 1);
		n = sort_intersections(n, y + 1, low, high);
		y[n + 1] = high;
		n += 2;

		if (limits.Low( ) > limits.High( ))
		{
			for (s32 i = 0; i < (n - 1); i++)
			{
			// Points are closer than 2 eps then interval is nonexistent
				if (_abs(y[i] - y[i + 1]) < 2 * eps)
					continue;

				clip(1, y[i] + eps, y[i + 1] - eps, limits.Low( ), 2 * M_PI, psi1);
				clip(1, y[i] + eps, y[i + 1] - eps, 0, limits.High( ), psi1);
			}
			clip(2, eps, 2 * M_PI - eps, limits.Low( ), 2 * M_PI, psi2);
			clip(2, eps, 2 * M_PI - eps, 0, limits.High( ), psi2);
		}
		else
		{
			for (s32 i = 0; i < (n - 1); i++)
			{
			// Points are closer than 2 eps then interval is nonexistent
				if (_abs(y[i] - y[i + 1]) < 2 * eps)
					continue;

				clip(1, y[i] + eps, y[i + 1] - eps, limits.Low( ), limits.High( ), psi1);
			}

			clip(2, eps, 2 * M_PI - eps, limits.Low( ), limits.High( ), psi2);
		}
	}
	else
		printf("psi limits not yet implemented for CosJtLimit\n");
}

IC f32 mytan(f32 v)
{
	const f32 eps = 1e-5f;
	const f32 big_eps = 1e-4f;

	if (equal(v, M_PI / 2, eps))
	{
		if (v > M_PI / 2)
			v = M_PI / 2 + big_eps;
		else
			v = M_PI / 2 - big_eps;
	}
	else if (equal(v, 3 * M_PI / 2, eps))
	{
		if (v > 3 * M_PI / 2)
			v = 3 * M_PI / 2 + big_eps;
		else
			v = 3 * M_PI / 2 - big_eps;
	}
	return tan(v);
}

void ComplexJtLimit::init(s32 jt_type,
						  f32 a1, f32 b1, f32 c1,
						  f32 a2, f32 b2, f32 c2,
						  f32 a3, f32 b3, f32 c3,
						  f32 low, f32 high)
{
	sin_eq.Reset(a1, b1, c1);
	cos_eq.Reset(a2, b2, c2);
	eq.Reset(a3, b3, c3);
	limits.Set(low, high);
	deriv.Reset(-c1 * b2 + b1 * c2, c1 * a2 - a1 * c2, -a1 * b2 + b1 * a2);

	// Singular tan cases
	tan_low = mytan(low);
	tan_high = mytan(high);

	switch (jt_type)
	{
		case SinJtLimit:
		case CosJtLimit:
			type = jt_type;
			break;
		default:
			printf("Invalid Joint Type passed to ComplexJtLimit %d\n", jt_type);
			exit(0);
			break;
	}
}

//
// Intervals where cos and sin are positive
//
static AngleInt PosSin(0, M_PI);
static AngleInt PosCos(-M_PI / 2, M_PI / 2);

f32 ComplexJtLimit::theta1(f32 v) const
{
	return angle_normalize(atan2(sin_eq.eval(v), cos_eq.eval(v)));
}

f32 ComplexJtLimit::theta2(f32 v) const
{
	return angle_normalize(atan2(-sin_eq.eval(v), -cos_eq.eval(v)));
}

f32 ComplexJtLimit::theta(s32 family, f32 psi) const
{
	// printf("theta %d %lf\n", family, psi);
	return family == 1 ? theta1(psi) : theta2(psi);
}

IC s32 sign(f32 x)
{
	return x < 0 ? -1 : 1;
}

f32 ComplexJtLimit::theta1_d_aux(f32 v, f32 delta) const
{
	f32 e1, e2, e1d, e2d, e3;
	const f32 eps = 1e-6f;

	e1 = sin_eq.eval(v);
	e1d = sin_eq.deriv(v);
	e2 = cos_eq.eval(v);
	e2d = cos_eq.deriv(v);

	e3 = eq.eval(v);
	e3 = 1 - e3 * e3;

	// denominator is close to 0, near a singularity
	if (iszero(e3, eps))
	{
		f32 left_d = theta1_d_aux(v + delta, delta * 10.0f);
		f32 right_d = theta1_d_aux(v - delta, delta * 10.0f);
		if (sign(left_d) != sign(right_d))
		{
			printf("signs are different in special case %lf %lf\n",
				   left_d, right_d);
			return 0.0f;
		}
		return (left_d + right_d) / 2.0f;
	}

	return (e1d * e2 - e1 * e2d) / e3;
}

f32 ComplexJtLimit::theta1_d(f32 v) const
{
	const f32 delta = 1e-3f;
	return theta1_d_aux(v, delta);
}

f32 ComplexJtLimit::theta2_d(f32 v) const
{
	return theta1_d(v);
}

f32 ComplexJtLimit::theta_d(s32 family, f32 psi) const
{
	return family == 1 ? theta1_d(psi) : theta2_d(psi);
}

//
// Critical points are where deriv equation is 0
// 
s32 ComplexJtLimit::CritPoints(f32 p[2]) const
{
	return deriv.roots(p);
}

//
// Note you cannot determine the singularities by looking where
// the derivative is 0 because sometimes a singularity occurs
// at a place where the derivative is not defined or discontinous
// 
s32 ComplexJtLimit::Singularities(f32 psi[2]) const
{
	s32 m;
	f32 u[4];
	const f32 eps = 1e-4f;

	if (type != SinJtLimit)
		printf("Singularities not implemented for cos type\n");

		// Solve sin(gamma) = a*cos(psi) + b*sin(psi) + c =  1 
		// and   sin(gamma) = a*cos(psi) + b*sin(psi) + c =  -1 
		// 
		// There is at most one solution to each equation so average 
		// multiple solutions. Also because of numercial problems
		// solve 1-eps and -1-eps

	switch (eq.solve(-1.0f + eps, u))
	{
		case 1:
			psi[0] = u[0];
			m = 1;
			break;
		case 2:
			psi[0] = (u[0] + u[1]) / 2.0f;
			m = 1;
			break;
		default:
			m = 0;
			break;
	}

	switch (eq.solve(1.0f - eps, u + m))
	{
		case 1:
			psi[m++] = u[0];
			break;
		case 2:
			psi[m++] = (u[0] + u[1]) / 2.0f;
			break;
		default:
			break;
	}

	// Sort if necessary
	if ((m == 2) && (psi[0] > psi[1]))
	{
		f32 t = psi[0];
		psi[0] = psi[1];
		psi[1] = t;
	}

	return m;
}

//
// Find all solutions for psi that satisfies
//
// theta = atan2(sin_eq(psi), cos_eq(psi)) = v
// 0 <= v <= 2pi, v != pi/2 and v != 3pi/2
// 

s32 ComplexJtLimit::solve_aux(f32 v, f32 tan_v, f32* solns) const
{
	f32 a, b, c;
	const f32 eps = 1e-5f;
	s32 n;

	// Special cases are when cos_eq(psi) = 0 or sin_eq(psi) = 0
	if (equal(v, M_PI / 2, eps) || equal(v, 3 * M_PI / 2, eps))
		n = cos_eq.roots(solns);

	else if (iszero(v, eps) || equal(v, M_PI, eps) || equal(v, 2 * M_PI, eps))
		n = sin_eq.roots(solns);

	else
	{
		a = sin_eq.alpha - tan_v * cos_eq.alpha;
		b = sin_eq.beta - tan_v * cos_eq.beta;
		c = sin_eq.xi - tan_v * cos_eq.xi;

		PsiEquation p(a, b, c);

		n = p.roots(solns);
	}
	return n;
}


//
// Find a solution for psi that satisfies
//
// theta = atan2(sin_eq(psi), cos_eq(psi)) = v
// 
// 0 <= v <= 2pi, v != pi/2 and v != 3pi/2
//
// for a specified family
//
// Assumes that we will not be given a value of v for which 
// the solution lies on a singularity. ie where eq(psi) = +/- 1  
//

s32 ComplexJtLimit::Solve(s32 family, f32 v, f32 tan_v, f32 psi[2]) const
{
	f32 temp[4];

	s32 n = solve_aux(v, tan_v, temp);

	// Since u = tan(v) is not unique, throw away spurious solns that
	// correspond to other family. For eg, 0 and Pi both produce u = 0.
	s32 m = 0;
	for (s32 i = 0; i < n; i++)
	{
		f32 t = theta(family, temp[i]);

		if (equal(t, v, 1e-4f))
			psi[m++] = temp[i];
	}

	return m;
}

s32 angleequal(f32 x, f32 y, f32 eps)
{
	// Handle case where x = 2*MPI, y = 0 or vice versa 
	if (equal(x, 2 * M_PI, eps) && _abs(y) < eps)
		return 1;

	if (_abs(x) < eps && equal(y, 2 * M_PI, eps))
		return 1;

	return equal(x, y, eps);
}


//
// Same as Solve but solve for both families
//

void ComplexJtLimit::Solve2(f32 v, f32 tan_v,
							s32& n1, f32 psi_1[2],
							s32& n2, f32 psi_2[2]) const
{
	f32 temp[4];

	s32 n = solve_aux(v, tan_v, temp);

	// Assign solutions to correct family

	n1 = n2 = 0;
	for (s32 i = 0; i < n; i++)
	{
		f32 t = theta(1, temp[i]);
		f32 t2 = angle_normalize(t + M_PI);

		if (angleequal(t, v, 1e-4f))
			psi_1[n1++] = temp[i];
		else if (angleequal(t2, v, 1e-4f))
			psi_2[n2++] = temp[i];
		else
			printf("%lf not equal to %lf or %lf\n", v, t, t2);
	/*
		else
			psi_2[n2++] = temp[i];
	*/
	}
}

const f32 eps = AINT_BIG_EPSILON / 5.0f;

void ComplexJtLimit::clip(f32 low,
						  f32 high,
						  s32 family,
						  s32 n,
						  const f32 p[ ],
						  AngleIntList& f) const
{
	// Check the midpoint of each interval to see if it is a valid
	// interval or invalid one

	for (s32 i = 0; i < (n - 1); i++)
	{
		if (_abs(p[i] - p[i + 1]) < 2 * eps)
			continue;

		f32 t = theta(family, (p[i] + eps + p[i + 1] - eps) / 2.0f);

		if ((t >= low) && (t <= high))
			f.Add(p[i], p[i + 1]);
	}
}

void ComplexJtLimit::store_intersections(s32 n,
										 const f32* s,
										 f32 low,
										 f32 high,
										 f32 tan_l,
										 f32 tan_h,
										 s32& n1,
										 f32* f1,
										 s32& n2,
										 f32* f2) const
{
	s32 m1;
	s32 m2;

	for (s32 i = 0; i < n; i++)
	{
		f1[i] = f2[i] = s[i];
	}

	Solve2(low, tan_l, n1, f1 + n, n2, f2 + n);
	n1 += n;
	n2 += n;
	Solve2(high, tan_h, m1, f1 + n1, m2, f2 + n2);
	n1 += m1;
	n2 += m2;

	sort(n1, f1);
	sort(n2, f2);
}

const f32 tan_0 = tan(eps / 2.0f);
const f32 tan_two_pi = (2 * M_PI - eps / 2.0f);

void ComplexJtLimit::PsiLimits(s32 num_s,
							   f32 s[ ],
							   AngleIntList& psi1,
							   AngleIntList& psi2) const
{
	psi1.Clear( );
	psi2.Clear( );

	f32 y[10], p1[16], p2[16], * temp;
	s32 n;
	s32 n1;
	s32 n2;

	temp = y + 1;
	for (s32 i = 0; i < num_s; i++)
		temp[i] = s[i];

	n = num_s;

	if (type == SinJtLimit)
	{
		n += sin_eq.roots(temp + n);
		y[0] = eps / 2.0f;
		y[n + 1] = 2 * M_PI - eps / 2.0f;
		n += 2;

		if (limits.Low( ) > limits.High( ))
		{
			store_intersections(n, y, limits.Low( ), 2 * M_PI,
								tan_low, tan_two_pi,
								n1, p1, n2, p2);
			clip(limits.Low( ), 2 * M_PI, 1, n1, p1, psi1);
			clip(limits.Low( ), 2 * M_PI, 2, n2, p2, psi2);

			store_intersections(n, y, 0, limits.High( ),
								tan_0, tan_high,
								n1, p1, n2, p2);
			clip(0, limits.High( ), 1, n1, p1, psi1);
			clip(0, limits.High( ), 2, n2, p2, psi2);
		}
		else
		{
			store_intersections(n, y, limits.Low( ), limits.High( ),
								tan_low, tan_high,
								n1, p1, n2, p2);
			clip(limits.Low( ), limits.High( ), 1, n1, p1, psi1);
			clip(limits.Low( ), limits.High( ), 2, n2, p2, psi2);
		}
	}
	else
	{
		printf("psi limits not yet implemented for CosJtLimit\n");
	}
}

void ComplexJtLimit::ResetJtLimits(f32 low, f32 high)
{
	limits.SetLow(low);
	limits.SetHigh(high);
	tan_low = mytan(low);
	tan_high = mytan(high);
}

void ComplexJtLimit::SetLow(f32 low)
{
	limits.SetLow(low);
	tan_low = mytan(low);
}

void ComplexJtLimit::SetHigh(f32 high)
{
	limits.SetHigh(high);
	tan_high = mytan(high);
}
