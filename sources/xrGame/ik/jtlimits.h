/*
  This source code is a part of IKAN.
  Copyright (c) 2000 University of Pennsylvania
  Center for Human Modeling and Simulation
  All Rights Reserved.

  IN NO EVENT SHALL THE UNIVERSITY OF PENNSYLVANIA BE LIABLE TO ANY
  PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
  DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
  SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF PENNSYLVANIA
  HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes,
  without fee, and without a written agreement is hereby granted,
  provided that the above copyright notice and the following three
  paragraphs appear in all copies. For for-profit purposes, please
  contact University of Pennsylvania
 (http://hms.upenn.edu/software/ik/ik.html) for the software license
  agreement.


  THE UNIVERSITY OF PENNSYLVANIA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
  BASIS, AND THE UNIVERSITY OF PENNSYLVANIA HAS NO OBLIGATION
  TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
  MODIFICATIONS.

 */

#ifndef _JTLIMITS
#define _JTLIMITS

#include "eqn.h"

//
// Inverse trig routines defined for various quadrants
//

// -Pi/2 to  Pi/2 (quadrants IV,I)
IC f32 asin1(f32 x)
{
	if (_abs(x) > 1.0f)
	{
//	printf("Domain error in asin1 %lf\n", x);
	x = (x > 0.f) ? 1.0f : -1.0f;
	}
	return angle_normalize(asin(x));
}

//  Pi/2 to -Pi/2 (quadrants II,III)
IC f32 asin2(f32 x)
{   
	if (_abs(x) > 1.0f)
	{
//	printf("Domain error in asin2 %lf\n", x);
	x = (x > 0) ? 1.0f : -1.0f;
	}
	return angle_normalize(M_PI - asin(x));
}


//  0 to Pi   (quadrants I,II)
IC f32 acos1(f32 x)
{
	if (_abs(x) > 1.0f)
	{
//	printf("Domain error in acos1 %lf\n", x);
	x = (x > 0) ? 1.0f : -1.0f;
	}
	return angle_normalize(acos(x));
}

//  Pi to 2Pi (quadrants III,IV)
IC f32 acos2(f32 x)
{
	if (_abs(x) > 1.0f)
	{
//	printf("Domain error in acos2 %lf\n", x);
	x = (x > 0) ? 1.0f : -1.0f;
	}
	return angle_normalize(- acos(x));
}


enum { SinJtLimit, CosJtLimit }; 

//
// A simple joint limit is one of the form
//
// cos(theta) or sin(theta) = a*cos(psi) + b*sin(psi) + c
// 
// where x theta the joint variable and psi is the elbow 
// swivel angle
//
//
// For a given value of psi there are two families of
// solutions for theta.  If the psi equation represents
// the sin of theta, then the two families are in
// quadrants I,IV and quadrants II,III. If the psi
// equation represents the cos of theta, then the
// two families lie in quadrants I,II and quadrants
// III,IV. 
//
// For a given set of bounds on theta there are two 
// sets of bounds on psi one for each family
//

class SimpleJtLimit
{
private:
	int type;
	PsiEquation psi;
	AngleInt    limits;
	f32       sin_low, sin_high;

	f32 theta1_d_aux(f32 v, f32 delta) const;


	void clip(int family, 
			  f32 psi0, f32 psi1,
			  f32 low, f32 high,
		  AngleIntList &a) const; 

public:

	void init(int jt_type, 
			  f32 a, f32 b, f32 c,
			  f32 low, f32 high);

	SimpleJtLimit(int jt_type, 
				  f32 a, f32 b, f32 c,
				  f32 low, f32 high)
	{
	init(jt_type, a, b, c, low, high);
	}

	SimpleJtLimit() {} 

	AngleInt &Limits() { return limits; }

	f32 eval(f32 v)
	{ return psi.eval(v); }

	f32 deriv(f32 v)
	{ return psi.deriv(v); }

	void ResetPsi(f32 a, f32 b, f32 c)
	{ psi.Reset(a,b,c); }

	void ResetJtLimits(f32 low, f32 high)
	{ limits.SetLow(low); limits.SetHigh(high); }

	f32 Low() { return limits.Low(); }
	f32 High() { return limits.High(); }

	void SetLow(f32 v) { limits.SetLow(v); }
	void SetHigh(f32 v) { limits.SetHigh(v); }

	int InRange(f32 t) { return limits.InRange(t); }

	// Reports where discontinuities can occur for theta(family)
	int Discontinuity(int family, f32 psi[2]) const;

	AngleInt &Limit() { return limits; }
	
	
	// Given psi calcuate joint variable. Two solns
	f32 theta1(f32 psi) const;  // family 1
	f32 theta2(f32 psi) const;  // family 2
	f32 theta(int family, f32 psi) const;

	// Derivatives of theta
	f32 theta1_d(f32 psi) const;
	f32 theta2_d(f32 psi) const;
	f32 theta_d(int family, f32 psi) const;

	// Solves for psi st theta(family,psi) = v. 0 to 2 solns
	int Solve(int family, f32 v, f32 sin_v, f32 psi[2]) const;

	
	// Given a joint limit, return a set of joint limits
	// for psi. psi1 contains valid regions for the first
	// family, and psi2 contains valid regions for the
	// second family

	void PsiLimits(AngleIntList &psi1, 
		   AngleIntList &psi2) const;

};



//
// Given two psi equations that represents equations of the
// form
//
// sin(theta)*(cos(x) or sin(x)) = a1*cos(psi) + b1*sin(psi) + c1 
// cos(theta)*(cos(x) or sin(x)) = a2*cos(psi) + b2*sin(psi) + c2 
//
// Find two sets of psi intervals that satisfy the joint limits
// on theta
//
// psi1 contain the intervals of psi such that
//
// theta = atan1(a1*cos(psi) + b1*sin(psi) + c1,
//               a2*cos(psi) + b2*sin(psi) + c2)
//
// psi2 contain the intervals of psi such that
//
// theta = atan2(a1*cos(psi) + b1*sin(psi) + c1,
//               a2*cos(psi) + b2*sin(psi) + c2)
//
// 
// pos_interval indicates an angle interval for psi in which
// sin(x) or cos(x) is positive
//
// neg_interval indicates an angle interval for psi in which
// sin(x) or cos(x) is negative
		

//
// A complex joint limit is one of the form
//
// sin(theta)*sin(gamma) = a1*cos(psi) + b1*sin(psi) + c1
// cos(theta)*sin(gamma) = a2*cos(psi) + b2*sin(psi) + c2
// cos(gamma) = a3*cos(psi) + b3*sin(psi) + c3


//
// or of the form
// 
// sin(theta)*cos(gamma) = a1*cos(psi) + b1*sin(psi) + c1
// cos(theta)*cos(gamma) = a2*cos(psi) + b2*sin(psi) + c2
// sin(gamma) = a3*cos(psi) + b3*sin(psi) + c3
//

class ComplexJtLimit
{
private:
	PsiEquation cos_eq;  // cos(theta) equation
	PsiEquation sin_eq;  // sin(theta) equation
	PsiEquation eq;	 // gamma equation 
	int type;		 // Whether eq is a sin or cos of gamma
	PsiEquation deriv;   // Derivative of sin_eq/cos_eq without denom
	AngleInt limits;
	f32    tan_low, tan_high;

	f32 theta1_d_aux(f32 v, f32 delta) const;

#if 0
	void clip(int family, 
			  f32 psi0, f32 psi1,
			  f32 low, f32 high,
		  AngleIntList &a) const;
#else
	void clip(f32 low, f32 high,
		  int family,
		  int n,
		  const f32 p[],
		  AngleIntList &f) const;

	void store_intersections(int n,
		 const f32* s,
							 f32 low,
							 f32 high,
							 f32 tan_l,
							 f32 tan_h,
		 int  &n1,
							 f32* f1,
		 int &n2,		
							 f32* f2) const;
#endif

	// Used by Solve and Solve2
	int solve_aux(f32 v, f32 tan_v, f32* solns) const;
public:

	void init(int jt_type, 
			  f32 a1, f32 b1, f32 c1,
			  f32 a2, f32 b2, f32 c2,
			  f32 a3, f32 b3, f32 c3,
			  f32 low, f32 high);

	ComplexJtLimit(int jt_type, 
				   f32 a1, f32 b1, f32 c1,
				   f32 a2, f32 b2, f32 c2,
				   f32 a3, f32 b3, f32 c3,
				   f32 low, f32 high)
	{
	init(jt_type, a1, b1, c1, a2, b2, c2, a3, b3, c3, low, high); 
	}


	ComplexJtLimit() {} 
 
	void ResetCosPsi(f32 a, f32 b, f32 c)
	{ cos_eq.Reset(a,b,c); }

	void ResetSinPsi(f32 a, f32 b, f32 c)
	{ sin_eq.Reset(a,b,c); }

	void Reset(f32 a, f32 b, f32 c)
	{ eq.Reset(a,b,c); }

	void ResetJtLimits(f32 low, f32 high);

	void SetLow(f32 low);

	void SetHigh(f32 high);


	// Given psi calcuate joint variable. Two solns

	f32 theta1(f32 psi) const;  // family 1
	f32 theta2(f32 psi) const;  // family 2
	f32 theta(int, f32 psi) const;

	
	// Derivatives of theta wrt to psi
	f32 theta1_d(f32 psi) const;
	f32 theta2_d(f32 psi) const;
	f32 theta_d(int family, f32 psi) const;



	// Given a joint limit, return a set of joint limits
	// for psi. psi1 contains valid regions for the first
	// family, and psi2 contains valid regions for the
	// second family. 
	
	// For efficiency take in the singular pts as a 
	// paramter in case this routine is called repeatedly
	// the singular pts are only computed once.

	void PsiLimits(int num_singular,
				   f32 singular_pts[],
		   AngleIntList &psi1, 
		   AngleIntList &psi2) const;


	//
	// Returns the values of psi for which tan(theta) = 0
	// 
	int CritPoints(f32 psi[2]) const;

	//
	// Returns the values for which tan(theta) is singular
	//
	int Singularities(f32 psi[4]) const;

	int InRange(f32 t) { return limits.InRange(t); }


	//
	// atan2(eq*sin_eq,eq*cos_eq) = v for psi
	//    returns the number of solutions
	//

	int Solve(int family, f32 v, f32 tan_v, f32 psi[2]) const;
	void Solve2(f32 v, f32 tan_v,
		int &n1, f32 psi_1[2],
		int &n2, f32 psi_2[2]) const;
		   
	f32 Low()  const { return limits.Low(); }
	f32 High() const { return limits.High(); }

	AngleInt & Limits()  { return limits; }
};

#endif
